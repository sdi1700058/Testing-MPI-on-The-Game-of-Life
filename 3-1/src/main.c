#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "../include/grid.h"
#include "../include/game_of_life.h"
#include "../include/utilities.h"

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    Parameters params = parse_arguments(argc, argv);
    
    Grid* current = NULL;
    Grid* next = NULL;
    Grid* local_current = NULL;
    Grid* local_next = NULL;
    
    int rows_per_proc = params.grid_size / size;
    if (rank == 0) {
        current = create_grid(params.grid_size);
        next = create_grid(params.grid_size);
        initialize_random_grid(current);
    }

    local_current = create_grid_section(rows_per_proc, params.grid_size);
    local_next = create_grid_section(rows_per_proc, params.grid_size);

    double start_time = MPI_Wtime();
    
    for (int gen = 0; gen < params.generations; gen++) {
        distribute_grid(current, local_current, rank, size, params.grid_size);
        evolve_mpi(local_current, local_next, rank, size);
        gather_grid(local_next, next, rank, size, params.grid_size);
        
        if (rank == 0) {
            Grid* temp = current;
            current = next;
            next = temp;
        }
    }
    
    double end_time = MPI_Wtime();
    
    if (rank == 0) {
        print_stats(current, params.generations, end_time - start_time);
        destroy_grid(current);
        destroy_grid(next);
    }
    
    destroy_grid(local_current);
    destroy_grid(local_next);
    
    MPI_Finalize();
    return 0;
}
