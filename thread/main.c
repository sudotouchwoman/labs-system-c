#include <stdlib.h>
#include <sys/time.h>
#include <memory.h>

#include "manager.h"

#define N_THREADS 4
#define H (32)
#define W (32)

int main(int argc, char* argv[]) {
    size_t n_threads = N_THREADS;

    if (argc == 2) {
        n_threads = (size_t)atol(argv[1]);
        fprintf(stdout, "Provided number of threads: %lu\n", n_threads);
    } else {
        fprintf(stdout, "No user-provided number of threads, defaulting to %lu\n", n_threads);
    }

    if (!n_threads || H % n_threads) {
        fprintf(stderr, "Invalid number of threads specified: %lu, Row count: %d\n", n_threads, H);
        exit(EXIT_FAILURE);
    }

    // theis config should be exported somewhere outside
    // and read in runtime but I am too lazy to do that
    const size_t timesteps = 250;
    const physics_t ex = {
        .C = 1.0,
        .R = 2.5,
        .I = 15.0,
        .U = 20.0,
        .h = 0.2,
        .SOURCE = CURRENT_SOURCE
    };

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

    managed_pool_t *const pool = init_pool(ex, n_threads, *current_grid);
    if (pool == NULL) {
        destroy_grid(current_grid);
        destroy_grid(prev_grid);
        fprintf(stderr, "Thread pool creation error\n");
        return EXIT_FAILURE;
    }

    // open saparate process to pass intermediate results to gnuplot
    FILE* gnuplot_script = popen("gnuplot -persist", "w");
    if (gnuplot_script == NULL) {
        fprintf(stderr, "Failed to open output file with gnuplot script\n");
        destroy_grid(current_grid);
        destroy_grid(prev_grid);
        destroy_pool(pool);
        exit(EXIT_FAILURE);
    }

    setup_gnuplot(gnuplot_script);

    // measure the computation time
    struct timeval t_start, t_end;
    struct timezone tz;
    gettimeofday(&t_start, &tz);

    const size_t grid_size = sizeof(double) * current_grid->h * current_grid->w;
    // create threads and start integration
    spawn_workers(*pool, current_grid, prev_grid);
    sync(pool->start_barrier);

    for (size_t i = 1; i < timesteps; ++i) {
        sync(pool->end_barrier);
        fprintf(stderr, "\r[%3lu/%3lu] Processing...", i, timesteps);
        dump_timestep(gnuplot_script, prev_grid, i);
        memcpy(prev_grid->grid, current_grid->grid, grid_size);
        sync(pool->start_barrier);
    }

    mark_done(pool);
    fprintf(stderr, "Done\n");
    sync(pool->end_barrier);

    pclose(gnuplot_script);
    await_workers(*pool);

    gettimeofday(&t_end, &tz);
    fprintf(stderr, "Elapsed time: %.8lfs\n", elapsed_time(t_start, t_end));

    destroy_pool(pool);
    destroy_grid(current_grid);
    destroy_grid(prev_grid);

    return EXIT_SUCCESS;
}