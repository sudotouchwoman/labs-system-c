#include "manager.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <pthread.h>

static int set_thread_attrs(pthread_attr_t *const attrs) {
    if (attrs == NULL) return EMPTY_PTR_ERROR;
    pthread_attr_init(attrs);
    pthread_attr_setscope(attrs, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(attrs, PTHREAD_CREATE_JOINABLE);
    return OK;
}

static int init_barrier(pthread_barrier_t *const barrier, const size_t n_threads) {
    if (barrier == NULL) return EMPTY_PTR_ERROR;
    pthread_barrier_init(barrier, NULL, n_threads);
    return OK;
}

static void fill_pool(managed_pool_t *const pool, const grid_t grid) {
    init_barrier(pool->start_barrier, pool->n_threads + 1);
    init_barrier(pool->end_barrier, pool->n_threads + 1);
    set_thread_attrs(pool->worker_attrs);

    *pool->quit = 0;

    const size_t height = grid.h;
    const size_t rows_per_worker = height / pool->n_threads;

    for (size_t i = 0; i < pool->n_threads; ++i) {
        pool->rows_start[i] = rows_per_worker * i;
        pool->rows_end[i] = rows_per_worker * (i + 1);
    }
}

managed_pool_t* init_pool(const physics_t ex, const size_t n_threads, const grid_t grid) {
    // EVERYTHING PASSED AS ARGUMENT TO WORKERS
    // SHOULD NOT BE STACK'ED
    // THUS ALL THE VARIABLES IN POOL
    // SHOULD BE ALLOCATED

    managed_pool_t *buffer = malloc(sizeof(managed_pool_t));
    if (buffer == NULL) return NULL;

    // POOL CONTAINS:
    // 1) barriers, starting and ending
    // 2) thread id for each worker
    // 3) setup variables
    // 4) quit flag
    // 5) number of threads (variable)
    // 6) working area for each thread
    // SKREW GLOBAL VARIABLES

    pthread_t *const tids = calloc(n_threads, sizeof(pthread_t));
    pthread_barrier_t *const start_barrier = calloc(1, sizeof(pthread_barrier_t));
    pthread_barrier_t *const end_barrier = calloc(1, sizeof(pthread_barrier_t));

    pthread_attr_t * worker_attrs = calloc(1, sizeof(pthread_attr_t));
    physics_t *const ex_allocated = calloc(1, sizeof(physics_t));

    size_t *const rows_start = calloc(n_threads, sizeof(size_t));
    size_t *const rows_end = calloc(n_threads, sizeof(size_t));
    int *const quit = calloc(1, sizeof(int));

    if (tids == NULL ||
        quit == NULL ||
        rows_start == NULL ||
        rows_end == NULL ||
        start_barrier == NULL ||
        end_barrier == NULL ||
        worker_attrs == NULL||
        ex_allocated == NULL) {

        free(start_barrier);
        free(end_barrier);
        free(worker_attrs);
        free(buffer);
        free(rows_start);
        free(rows_end);
        free(quit);

        return NULL;
    }
    
    memcpy(ex_allocated, &ex, sizeof(physics_t));

    buffer->n_threads = n_threads;
    buffer->tids = tids;
    
    buffer->start_barrier = start_barrier;
    buffer->end_barrier = end_barrier;
    
    buffer->worker_attrs = worker_attrs;
    buffer->ex = ex_allocated;
    buffer->quit = quit;

    buffer->rows_start = rows_start;
    buffer->rows_end = rows_end;

    fill_pool(buffer, grid);
    return buffer;
}

void await_workers(const managed_pool_t pool) {
    for (size_t i = 0; i < pool.n_threads; ++i) {
        void *retval;
        pthread_join(pool.tids[i], &retval);
        fprintf(stderr, "Joined %lu\n", i);
        // free the memory chunk
        // allocated in `spawn_workers` function
        free(retval);
    }
}

int destroy_pool(managed_pool_t *const pool) {
    if (pool == NULL) return EMPTY_PTR_ERROR;
    free(pool->tids);

    pthread_barrier_destroy(pool->start_barrier);
    pthread_barrier_destroy(pool->end_barrier);
    free(pool->start_barrier);
    free(pool->end_barrier);

    free(pool->ex);
    free(pool->quit);
    
    free(pool->rows_start);
    free(pool->rows_end);
    
    pthread_attr_destroy(pool->worker_attrs);
    free(pool->worker_attrs);
    free(pool);
    return OK;
}

int spawn_workers(const managed_pool_t pool, grid_t *const current_grid, grid_t *const prev_grid) {
    // check provided pointers
    if (current_grid == NULL) return EMPTY_PTR_ERROR;
    if (prev_grid == NULL) return EMPTY_PTR_ERROR;
    if (current_grid->grid == NULL) return EMPTY_PTR_ERROR;
    if (prev_grid->grid == NULL) return EMPTY_PTR_ERROR;

    for (size_t i = 0; i < pool.n_threads; ++i) {
        // create buffer with arguments and initialize const-qualified fields,
        // then copy the buffer into the allocated area
        // because arguments variable should not expire
        // through the thread's lifetime
        // the memory will be deallocated after the job is done
        // and workers join the managing thread
        worker_args_t args_template = {
            .current_ts = current_grid,
            .prev_ts = prev_grid,
            .start_barrier = pool.start_barrier,
            .end_barrier = pool.end_barrier,
            .quit = pool.quit,
            .setup = pool.ex,
            .row_start = pool.rows_start[i],
            .row_end = pool.rows_end[i]
        };

        worker_args_t *const args = malloc(sizeof(worker_args_t));
        if (args == NULL) {
            fprintf(stderr, "Failed to allocate memory for worker arguments\n");
            return EMPTY_PTR_ERROR;
        }

        memcpy(args, &args_template, sizeof(worker_args_t));

        if (pthread_create(pool.tids + i, pool.worker_attrs, worker_routine, (void*)args)) {
            fprintf(stderr, "Failed to create thread\n");
            return THREAD_CREATION_ERROR;
        }
    }
    return OK;
}

void sync(pthread_barrier_t *const barrier) {
    pthread_barrier_wait(barrier);
}

struct timeval elapsed_time(const struct timeval start, const struct timeval end) {
    const struct timeval dt = {
        .tv_sec = end.tv_sec - start.tv_sec,
        .tv_usec = end.tv_usec - start.tv_usec
    };
    return dt;
}

