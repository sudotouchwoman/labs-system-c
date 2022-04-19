#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <memory.h>

#include "manager.h"

#define N_THREADS 4
#define H 8
#define W 10

int main(int argc, char* argv[]) {
    const physics_t ex = {
        .C = 2,
        .R = 3,
        .I = 4,
        .U = 10,
        .h = 0.5,
        .SOURCE = VOLTAGE_SOUCRE
    };

    const size_t timesteps = 100;

    grid_t * prev = make_grid(H, W);
    if (prev == NULL) {
        fprintf(stderr, "Failed to create grid\n");
        return EXIT_FAILURE;
    }

    grid_t * current = make_grid(H, W);
    if (current == NULL) {
        fprintf(stderr, "Failed to create grid\n");
        return EXIT_FAILURE;
    }

    const size_t n_threads = N_THREADS;
    pthread_attr_t thread_attrs;

    if (set_thread_attrs(&thread_attrs) != OK) {
        fprintf(stderr, "Attributes not set\n");
        return EXIT_FAILURE;
    }

    managed_pool_t *const pool = init_pool(ex, n_threads);
    if (pool == NULL) {
        destroy_grid(current);
        destroy_grid(prev);
        fprintf(stderr, "Thread pool creation error\n");
        return EXIT_FAILURE;
    }

    pthread_barrier_t start_barrier, end_barrier;
    init_barrier(&start_barrier, n_threads + 1);
    init_barrier(&start_barrier, n_threads + 1);

    pool->start_barrier = &start_barrier;
    pool->end_barrier = &end_barrier;
    pool->worker_attrs = &thread_attrs;

    spawn_workers(*pool, current, prev);

    struct timeval t_start, t_end;
    struct timezone tz;

    gettimeofday(&t_start, &tz);
    const size_t grid_size = sizeof(double) * current->h * current->w;

    sync(&start_barrier);

    for (size_t i = 1; i < timesteps; ++i) {
        sync(&end_barrier);
        memcpy(prev->grid, current->grid, grid_size);
        memset(current->grid, 0, grid_size);
        fprintf(stderr, "[%lu]\tProcessing...\n", i);
        sync(&start_barrier);
    }

    // quit = 1;

    gettimeofday(&t_end, &tz);
    const struct timeval dt = elapsed_time(t_start, t_end);

    fprintf(stderr, "Elapsed time: %lds %ldms\n", dt.tv_sec, dt.tv_usec);
    return EXIT_SUCCESS;
}