#include <mpi.h>
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

void evolve_mpi(Grid* current, Grid* next, int local_rows) {
    for (int i = 1; i <= local_rows; i++) {
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

// Update return type to return error status
int exchange_borders(Grid* grid, int rank, int size) {
    MPI_Request requests[4];
    MPI_Status statuses[4];
    int req_count = 0;
    int row_size = grid->padded_size;
    int err = MPI_SUCCESS;
    
    // First post all receives, then all sends to avoid deadlock
    if (rank > 0) {
        err = MPI_Irecv(&grid->cells[0], row_size, MPI_INT,
                      rank - 1, 0, MPI_COMM_WORLD,
                      &requests[req_count++]);
        if (err != MPI_SUCCESS) return err;
    }
    
    if (rank < size - 1) {
        err = MPI_Irecv(&grid->cells[(grid->local_rows + 1) * row_size],
                      row_size, MPI_INT, rank + 1, 1, MPI_COMM_WORLD,
                      &requests[req_count++]);
        if (err != MPI_SUCCESS) return err;
    }
    
    if (rank < size - 1) {
        err = MPI_Isend(&grid->cells[grid->local_rows * row_size], 
                      row_size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD,
                      &requests[req_count++]);
        if (err != MPI_SUCCESS) return err;
    }
    
    if (rank > 0) {
        err = MPI_Isend(&grid->cells[row_size], row_size, 
                      MPI_INT, rank - 1, 1, MPI_COMM_WORLD,
                      &requests[req_count++]);
        if (err != MPI_SUCCESS) return err;
    }
    
    if (req_count > 0) {
        err = MPI_Waitall(req_count, requests, statuses);
    }
    
    return err;
}
