#ifndef _MANAGER_H
#define _MANAGER_H

#include "worker.h"
#include <stdio.h>

enum MANAGER {
    OK,
    EMPTY_PTR_ERROR,
    THREAD_CREATION_ERROR,
    WORKING = 0,
    DONE
};

typedef struct {
    pthread_t * tids;
    size_t * rows_start;
    size_t * rows_end;
    pthread_barrier_t* start_barrier;
    pthread_barrier_t* end_barrier;
    pthread_attr_t * worker_attrs;
    int * quit;
    physics_t * ex;
    size_t n_threads;
} managed_pool_t;

managed_pool_t* init_pool(const physics_t ex, const size_t n_threads, const grid_t grid);
void await_workers(const managed_pool_t pool);
int destroy_pool(managed_pool_t *const pool);

int spawn_workers(
    managed_pool_t const pool,
    grid_t *const current_grid,
    grid_t *const prev_grid);

void sync(pthread_barrier_t *const barrier);
void mark_done(managed_pool_t *const pool);
// int set_thread_attrs(pthread_attr_t *const attrs);
// int init_barrier(pthread_barrier_t *const barrier, const size_t n_threads);
double elapsed_time(const struct timeval start, const struct timeval end);

void setup_gnuplot(FILE *const fd);
void dump_timestep(FILE *const fd, const grid_t *const grid);

#endif