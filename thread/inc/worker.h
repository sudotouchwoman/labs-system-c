#ifndef _WORKER_H
#define _WORKER_H

#include "grid.h"

typedef struct {
    grid_t *const current_ts;   // system state at current time step
    grid_t *const prev_ts;      // system state at previous time step
    int *const quit;            // end of processing flag
    pthread_barrier_t *const start_barrier;
    pthread_barrier_t *const end_barrier;
    const size_t* row_start;           // integration region borders
    const size_t* row_end;
    const physics_t* setup;            // model properties
} worker_args_t;

void *worker_routine(void * args);

#endif