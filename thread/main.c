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

    grid_t * prev_grid = make_grid(H, W);
    if (prev_grid == NULL) {
        fprintf(stderr, "Failed to create grid\n");
        return EXIT_FAILURE;
    }

    grid_t * current_grid = make_grid(H, W);
    if (current_grid == NULL) {
        fprintf(stderr, "Failed to create grid\n");
        return EXIT_FAILURE;
    }

    const size_t n_threads = N_THREADS;

    managed_pool_t *const pool = init_pool(ex, n_threads, *current_grid);
    if (pool == NULL) {
        destroy_grid(current_grid);
        destroy_grid(prev_grid);
        fprintf(stderr, "Thread pool creation error\n");
        return EXIT_FAILURE;
    }

    spawn_workers(*pool, current_grid, prev_grid);

    struct timeval t_start, t_end;
    struct timezone tz;

    gettimeofday(&t_start, &tz);
    const size_t grid_size = sizeof(double) * current_grid->h * current_grid->w;

    sync(pool->start_barrier);

    for (size_t i = 1; i < timesteps; ++i) {
        sync(pool->end_barrier);
        memcpy(prev_grid->grid, current_grid->grid, grid_size);
        memset(current_grid->grid, 0, grid_size);
        fprintf(stderr, "[%lu]\tProcessing...\n", i);
        sync(pool->start_barrier);
    }

    *pool->quit = 1;
    sync(pool->end_barrier);

    gettimeofday(&t_end, &tz);
    const struct timeval dt = elapsed_time(t_start, t_end);

    await_workers(*pool);
    destroy_pool(pool);

    destroy_grid(current_grid);
    destroy_grid(prev_grid);

    fprintf(stderr, "Elapsed time: %lds %ldms\n", dt.tv_sec, dt.tv_usec);
    return EXIT_SUCCESS;
}