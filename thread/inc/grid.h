#ifndef _GRID_H
#define _GRID_H

#include <sys/types.h>

enum {
    CURRENT_SOURCE,
    VOLTAGE_SOUCRE
};

enum {
    GRID_EMPTY_PTR_ERROR = -1,
    GRID_OK
};

typedef struct {
    double * grid;  // h by w matrix
    size_t h;       // height
    size_t w;       // width
} grid_t;

typedef struct {
    double U;  // voltage source
    double I;  // DC at current source
    double R;  // resistance of R element
    double C;  // capacity of capacitor
    double h;  // time integration step
    // source type to support the boundary condition
    int SOURCE;
} physics_t;

size_t idx(const grid_t *const ref, const size_t x, const size_t y);
double at(const grid_t *const ref, const size_t x, const size_t y);

grid_t * make_grid(const size_t h, const size_t w);
void set_grid(grid_t * const grid, const double value);
int destroy_grid(grid_t *const grid);

void pprint_grid(const grid_t* const grid);

#endif