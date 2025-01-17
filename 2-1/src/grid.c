#include <stdlib.h>
#include <time.h>
#include "../include/grid.h"

Grid* create_grid(int size) {
    Grid* grid = (Grid*)malloc(sizeof(Grid));
    grid->size = size;
    grid->padded_size = size + 2;  // Add border cells
    int total_size = grid->padded_size * grid->padded_size;
    grid->cells = (int*)calloc(total_size, sizeof(int));
    return grid;
}

void destroy_grid(Grid* grid) {
    free(grid->cells);
    free(grid);
}

void initialize_random_grid(Grid* grid) {
    static int seed_set = 0;
    if (!seed_set) {
        srand(time(NULL));
        seed_set = 1;
    }
    
    // Initialize only the inner cells, leaving border cells as 0
    for(int i = 1; i <= grid->size; i++) {
        for(int j = 1; j <= grid->size; j++) {
            grid->cells[i * grid->padded_size + j] = rand() % 2;
        }
    }
}

void copy_grid(Grid* dest, Grid* src) {
    for (int i = 1; i <= src->size; i++) {
        for (int j = 1; j <= src->size; j++) {
            dest->cells[i * dest->padded_size + j] = src->cells[i * src->padded_size + j];
        }
    }
}
