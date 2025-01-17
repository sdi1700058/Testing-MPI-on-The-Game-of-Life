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

void update_ghost_rows(Grid* local_current, int rank, int size) {
    int prev_rank = (rank - 1 + size) % size;
    int next_rank = (rank + 1) % size;
    
    MPI_Request requests[4];
    MPI_Status statuses[4];
    
    // Send/receive ghost rows
    MPI_Isend(&local_current->cells[local_current->size], local_current->padded_size, 
              MPI_INT, next_rank, 0, MPI_COMM_WORLD, &requests[0]);
    MPI_Isend(&local_current->cells[1 * local_current->padded_size], local_current->padded_size, 
              MPI_INT, prev_rank, 1, MPI_COMM_WORLD, &requests[1]);
    MPI_Irecv(&local_current->cells[0], local_current->padded_size, 
              MPI_INT, prev_rank, 0, MPI_COMM_WORLD, &requests[2]);
    MPI_Irecv(&local_current->cells[(local_current->size + 1) * local_current->padded_size], 
              local_current->padded_size, MPI_INT, next_rank, 1, MPI_COMM_WORLD, &requests[3]);
    
    MPI_Waitall(4, requests, statuses);
}

void evolve_mpi(Grid* local_current, Grid* local_next, int rank, int size) {
    update_ghost_rows(local_current, rank, size);
    
    for (int i = 1; i <= local_current->size; i++) {
        for (int j = 1; j <= local_current->size; j++) {
            int neighbors = count_neighbors(local_current, i, j);
            int index = i * local_current->padded_size + j;
            if (local_current->cells[index]) {
                local_next->cells[index] = (neighbors == 2 || neighbors == 3);
            } else {
                local_next->cells[index] = (neighbors == 3);
            }
        }
    }
}
