#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#include "../include/grid.h"

Grid* create_grid(int size) {
    Grid* grid = (Grid*)malloc(sizeof(Grid));
    if (!grid) return NULL;
    
    grid->size = size;
    grid->padded_size = size + 2;
    int total_size = grid->padded_size * grid->padded_size;
    grid->cells = (int*)calloc(total_size, sizeof(int));
    
    if (!grid->cells) {
        free(grid);
        return NULL;
    }
    return grid;
}

Grid* create_grid_local(int size, int local_rows) {
    Grid* grid = (Grid*)malloc(sizeof(Grid));
    if (!grid) return NULL;
    
    grid->size = size;
    grid->local_rows = local_rows;
    grid->padded_size = size + 2;
    int total_size = grid->padded_size * (local_rows + 2); // +2 for ghost rows
    grid->cells = (int*)calloc(total_size, sizeof(int));
    
    if (!grid->cells) {
        free(grid);
        return NULL;
    }
    return grid;
}

void distribute_grid(Grid* full_grid, int num_procs) {
    if (!verify_grid_integrity(full_grid)) return;
    
    int base_rows = full_grid->size / num_procs;
    int remainder = full_grid->size % num_procs;
    int offset = full_grid->padded_size; // Skip first row
    int mpi_err;
    
    for (int i = 1; i < num_procs; i++) {
        int rows = base_rows + (i < remainder ? 1 : 0);
        mpi_err = MPI_Send(&rows, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        if (mpi_err != MPI_SUCCESS) {
            fprintf(stderr, "Error sending row count to process %d\n", i);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        
        mpi_err = MPI_Send(&full_grid->cells[offset], 
                          rows * full_grid->padded_size, 
                          MPI_INT, i, 1, MPI_COMM_WORLD);
        if (mpi_err != MPI_SUCCESS) {
            fprintf(stderr, "Error sending grid data to process %d\n", i);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        offset += rows * full_grid->padded_size;
    }
}

void gather_grid(Grid* full_grid, Grid* local_grid, int num_procs) {
    if (!verify_grid_integrity(full_grid) || !verify_grid_integrity(local_grid)) return;
    
    int offset = full_grid->padded_size; // Skip first row
    MPI_Status status;
    
    // Copy local part first
    memcpy(&full_grid->cells[offset], 
           &local_grid->cells[full_grid->padded_size],
           local_grid->local_rows * full_grid->padded_size * sizeof(int));
    
    // Gather from other processes
    for (int i = 1; i < num_procs; i++) {
        int rows;
        MPI_Recv(&rows, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&full_grid->cells[offset], 
                 rows * full_grid->padded_size,
                 MPI_INT, i, 3, MPI_COMM_WORLD, &status);
        offset += rows * full_grid->padded_size;
    }
}

void destroy_grid(Grid* grid) {
    if (!grid) return;

    if (grid->cells) {
        // Zero out memory before freeing
        size_t size = grid->padded_size * 
                     (grid->local_rows ? (grid->local_rows + 2) : grid->padded_size);
        memset(grid->cells, 0, size * sizeof(int));
        free(grid->cells);
        grid->cells = NULL;
    }

    // Zero out grid structure before freeing
    memset(grid, 0, sizeof(Grid));
    free(grid);
}

// Add verification function
int verify_grid_integrity(Grid* grid) {
    if (!grid) return 0;
    if (!grid->cells) return 0;
    if (grid->size <= 0) return 0;
    if (grid->padded_size <= grid->size) return 0;
    return 1;
}

void initialize_random_grid(Grid* grid) {
    static int seed_set = 0;
    if (!seed_set) {
        srand(time(NULL));
        seed_set = 1;
    }
    
    // Fixed loop condition: j instead of i in second loop
    for(int i = 1; i <= grid->size; i++) {
        for(int j = 1; j <= grid->size; j++) {
            grid->cells[i * grid->padded_size + j] = rand() % 2;
        }
    }
}

void copy_grid(Grid* dest, Grid* src) {
    // Fixed loop condition
    for (int i = 1; i <= src->size; i++) {
        for (int j = 1; j <= src->size; j++) {  // Changed from i to j
            dest->cells[i * dest->padded_size + j] = src->cells[i * src->padded_size + j];
        }
    }
}

void copy_grid_portion(Grid* dest, Grid* src, int start_row, int num_rows) {
    memcpy(&dest->cells[dest->padded_size], 
           &src->cells[(start_row + 1) * src->padded_size], 
           num_rows * src->padded_size * sizeof(int));
    dest->local_rows = num_rows;
}

void receive_grid_portion(Grid* grid, int rank, int rows, int total_size) {
    MPI_Status status;
    
    // Receive number of rows first
    MPI_Recv(&grid->local_rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    
    // Receive grid data
    MPI_Recv(&grid->cells[grid->padded_size], 
             grid->local_rows * grid->padded_size, 
             MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
}

void send_grid_portion(Grid* grid, int rank, int rows, int total_size) {
    // Send number of rows first
    MPI_Send(&grid->local_rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    
    // Send grid data
    MPI_Send(&grid->cells[grid->padded_size], 
             grid->local_rows * grid->padded_size, 
             MPI_INT, 0, 3, MPI_COMM_WORLD);
}

// Remove exchange_borders function from here since it's already in game_of_life.c
// Delete or comment out the entire exchange_borders function
