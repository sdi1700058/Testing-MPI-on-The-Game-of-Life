#include <stdlib.h>
#include <time.h>
#include <mpi.h>
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

Grid* create_local_grid(int local_size, int total_size) {
    Grid* grid = (Grid*)malloc(sizeof(Grid));
    grid->size = local_size;
    grid->padded_size = total_size + 2;
    int total_cells = (local_size + 2) * grid->padded_size;
    grid->cells = (int*)calloc(total_cells, sizeof(int));
    return grid;
}

void distribute_grid(Grid* grid, Grid* local_grid, int rank, int size) {
    int local_size = grid->size / size;
    MPI_Scatter(grid->cells + grid->padded_size, 
                local_size * grid->padded_size, MPI_INT,
                local_grid->cells + local_grid->padded_size, 
                local_size * grid->padded_size, MPI_INT,
                0, MPI_COMM_WORLD);
}

void gather_grid(Grid* local_grid, Grid* grid, int rank, int size) {
    int local_size = grid->size / size;
    MPI_Gather(local_grid->cells + local_grid->padded_size,
               local_size * grid->padded_size, MPI_INT,
               grid->cells + grid->padded_size,
               local_size * grid->padded_size, MPI_INT,
               0, MPI_COMM_WORLD);
}

void update_ghost_rows(Grid* local_grid, int rank, int size) {
    int prev = (rank - 1 + size) % size;
    int next = (rank + 1) % size;
    
    MPI_Sendrecv(local_grid->cells + local_grid->size * local_grid->padded_size,
                 local_grid->padded_size, MPI_INT, next, 0,
                 local_grid->cells, local_grid->padded_size, MPI_INT, prev, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                 
    MPI_Sendrecv(local_grid->cells + local_grid->padded_size,
                 local_grid->padded_size, MPI_INT, prev, 1,
                 local_grid->cells + (local_grid->size + 1) * local_grid->padded_size,
                 local_grid->padded_size, MPI_INT, next, 1,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
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
