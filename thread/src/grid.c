#include "grid.h"

#include <stdlib.h>
#include <memory.h>

size_t idx(const grid_t *const ref, const size_t x, const size_t y) {
    return 0;
}

grid_t * make_grid(const size_t h, const size_t w) {
    grid_t * grid = calloc(1, sizeof(grid_t));
    if (grid == NULL) return NULL;

    grid->w = w;
    grid->h = h;

    grid->grid = calloc(w * h, sizeof(double));
    if (grid->grid == NULL) {
        free(grid);
        return NULL;
    }
    return grid;
}

void set_grid(grid_t *const grid, const double value) {
    memset(grid->grid, value, sizeof(double) * grid->w * grid->h);
}

int destroy_grid(grid_t *const grid) {
    if (grid == NULL) return GRID_EMPTY_PTR_ERROR;
    free(grid->grid);
    free(grid);
    return GRID_OK;
}
