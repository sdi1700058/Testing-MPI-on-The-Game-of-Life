#include <omp.h>
#include "../include/game_of_life.h"

// Static helper function for internal use only
static inline int count_neighbors_internal(Grid* grid, int i, int j) {
    int index = i * grid->padded_size + j;
    int ps = grid->padded_size;
    return grid->cells[index - ps - 1] + 
           grid->cells[index - ps] + 
           grid->cells[index - ps + 1] +
           grid->cells[index - 1] + 
           grid->cells[index + 1] +
           grid->cells[index + ps - 1] + 
           grid->cells[index + ps] + 
           grid->cells[index + ps + 1];
}

// Public interface using row,col coordinates
int count_neighbors(Grid* grid, int i, int j) {
    return count_neighbors_internal(grid, i, j);
}

void evolve_serial(Grid* current, Grid* next) {
    for (int i = 1; i <= current->size; i++) {
        for (int j = 1; j <= current->size; j++) {
            int neighbors = count_neighbors_internal(current, i, j);
            int index = i * current->padded_size + j;
            if (current->cells[index]) {
                next->cells[index] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
            } else {
                next->cells[index] = (neighbors == 3) ? 1 : 0;
            }
        }
    }
}

void evolve_parallel(Grid* current, Grid* next, int num_threads) {
    #pragma omp parallel for collapse(2) schedule(static) num_threads(num_threads)
    for(int i = 1; i <= current->size; i++) {
        for(int j = 1; j <= current->size; j++) {
            int neighbors = count_neighbors_internal(current, i, j);
            int index = i * current->padded_size + j;
            
            // Apply Conway's Game of Life rules
            if(current->cells[index]) {
                // Cell is alive
                next->cells[index] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
            } else {
                // Cell is dead
                next->cells[index] = (neighbors == 3) ? 1 : 0;
            }
        }
    }
}
