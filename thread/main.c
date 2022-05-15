#include <stdlib.h>
#include <sys/time.h>
#include <memory.h>

#include "manager.h"

#define N_THREADS 4
#define H (32)
#define W (32)
#define TIMESTEPS (250)

int main(int argc, char* argv[]) {
    size_t n_threads = N_THREADS;

    if (argc >= 2) {
        n_threads = (size_t)atol(argv[1]);
        fprintf(stdout, "Provided number of threads: %lu\n", n_threads);
    } else {
        fprintf(stdout, "No user-provided number of threads, defaulting to %lu\n", n_threads);
    }

    size_t width = W, height = H, timesteps = TIMESTEPS;

    if (argc >= 3) {
        width = (size_t)atol(argv[2]);
        height = (size_t)atol(argv[2]);
    }

    if (argc >= 4) {
        height = (size_t)atol(argv[3]);
    }

    if (argc == 5) {
        timesteps = (size_t)atol(argv[4]);
    }

    if (!n_threads || height % n_threads) {
        fprintf(stderr, "Invalid number of threads specified: %lu, Row count: %lu\n", n_threads, height);
        exit(EXIT_FAILURE);
    }

    fprintf(
        stdout,
        "Running with the following specs: grid [%lu;%lu], timesteps: %lu\n",
        height, width, timesteps
    );

    // this config should be exported somewhere outside
    // and read in runtime but I am too lazy to do that
    const physics_t ex = {
        .C = 1.0,
        .R = 2.5,
        .I = 15.0,
        .U = 20.0,
        .h = 0.2,
        .SOURCE = CURRENT_SOURCE
    };

    grid_t * prev_grid = make_grid(height, width);
    if (prev_grid == NULL) {
        fprintf(stderr, "Failed to create grid\n");
        return EXIT_FAILURE;
    }

    grid_t * current_grid = make_grid(height, width);
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
    FILE* gnuplot_script = popen("gnuplot -persist 2> /dev/null", "w");
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

    // create threads and start integration
    spawn_workers(*pool, current_grid, prev_grid);
    sync(pool->start_barrier);

    for (size_t i = 1; i < timesteps; ++i) {
        sync(pool->end_barrier);
        fprintf(stderr, "\r[%3lu/%3lu] Processing...", i, timesteps);
        dump_timestep(gnuplot_script, prev_grid, i);

        double * tmp = prev_grid->grid;
        prev_grid->grid = current_grid->grid;
        current_grid->grid = tmp;

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