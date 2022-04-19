#include "worker.h"

#include <pthread.h>
#include <stdio.h>

enum {
    NO = 0,
    YES
};

static int is_corner(const size_t x, const size_t y, const grid_t * const grid) {
    const size_t width = grid->w;
    const size_t height = grid->h;

    if (x == 0 && y == 0) return YES;
    if (x == 0 && y == height) return YES;
    if (x == width && y == 0) return YES;
    if (x == width && y == height) return YES;
    return NO;
}

static double voltage_at_adjacent(
    const grid_t *const prev,
    const grid_t *const current,
    const size_t x,
    const size_t y) {
        // no pointer checks as entering the body
        // of this private function implies passing earlier checks

        // finds the total voltage with respect to the
        // current node to be later used in numeric ODE solution
        // as the problem describes a matrix, we should check the boundary
        // cases: upper, lower, leftmost and rightmost nodes (these 
        // lack one neighbour)
        // inner nodes have 4 neighbours, and corner nodes
        // are updated according to the boundary conditions
        const size_t height = prev->h;
        const size_t width = prev->w;
        const double phi_node =  current->grid[idx(prev, x, y)];

    if (x == 0) {
        // leftmost border
        const double phi_adj =
            prev->grid[idx(prev, x, y + 1)] +
            prev->grid[idx(prev, x, y - 1)] +
            prev->grid[idx(prev, x + 1, y)];
        return phi_adj - 3* phi_node;
    }
    if (x == width) {
        // rightmost border
        const double phi_adj =
            prev->grid[idx(prev, x, y + 1)] +
            prev->grid[idx(prev, x, y - 1)] +
            prev->grid[idx(prev, x - 1, y)];
        return phi_adj - 3* phi_node;
    }
    if (y == 0) {
        // upper border
        const double phi_adj =
            prev->grid[idx(prev, x, y + 1)] +
            prev->grid[idx(prev, x + 1, y)] +
            prev->grid[idx(prev, x - 1, y)];
        return phi_adj - 3* phi_node;
    }
    if (y == height) {
        // lower border
        const double phi_adj =
            prev->grid[idx(prev, x, y - 1)] +
            prev->grid[idx(prev, x + 1, y)] +
            prev->grid[idx(prev, x - 1, y)];
        return phi_adj - 3* phi_node;
    }
    const double phi_adj =
            prev->grid[idx(prev, x, y - 1)] +
            prev->grid[idx(prev, x, y + 1)] +
            prev->grid[idx(prev, x + 1, y)] +
            prev->grid[idx(prev, x - 1, y)];
    return phi_adj - 4 * phi_node;
}

void* worker_routine(void * args) {
    if (args == NULL) return args;

    fprintf(stderr, "Thread started\n");
    worker_args_t *const routine_args = (worker_args_t*)args;
    grid_t *const current_ts = routine_args->current_ts;
    grid_t *const prev_ts = routine_args->prev_ts;

    // create aliases for routine arguments
    const size_t row_start = routine_args->row_start;
    const size_t row_end = routine_args->row_end;
    const size_t width = routine_args->current_ts->w;
    // ex stands for experiment setup, "ex" is just shorter
    // that "setup"
    const physics_t ex = *routine_args->setup;
    const int source_type = ex.SOURCE;

    // temporary variable to hold the total
    // voltage at adjacent nodes
    double v_adj = 0.0;

    // continue computations as long as required
    while (!*routine_args->quit) {
        // await the manager thread to swap timestep buffers
        pthread_barrier_wait(routine_args->start_barrier);
        // iterate over the specified grid area
        // in this problem, we assume that the load is equally balanced
        // among the threads, i.e. row count is divisible by thread count
        for (size_t y = row_start; y < row_end; ++y) {
            for (size_t x = 0; x < width; ++x) {
                const size_t index = idx(current_ts, x, y);

                if (is_corner(x, y, current_ts) == NO) {
                    v_adj = voltage_at_adjacent(prev_ts, current_ts, x, y);
                } else {
                // for corner points, the boundary conditions
                // are defined as constant voltage (formed either by current 
                // or voltage sources).
                    if (source_type == VOLTAGE_SOUCRE) {
                        current_ts->grid[index] = ex.U;
                        continue;
                    }
                    if (source_type == CURRENT_SOURCE)
                        v_adj = ex.I * ex.R;
                }
                // perform the forward Euler step:
                // V^{t+1} = V^t + \frac{dV^t}{dt} * h
                const double tau = ex.R * ex.C;
                current_ts->grid[index] = prev_ts->grid[index] + ex.h * v_adj / tau;
            }
        }
        // await other threads to finish current iteration
        pthread_barrier_wait(routine_args->end_barrier);
    }
    fprintf(stderr, "Thread Finished\n");
    pthread_exit(args);
}

