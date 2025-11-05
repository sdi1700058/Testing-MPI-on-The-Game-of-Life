#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <signal.h>
#include "../include/grid.h"
#include "../include/game_of_life.h"
#include "../include/utilities.h"

void cleanup_mpi() {
    int finalized;
    MPI_Finalized(&finalized);
    if (!finalized) {
        MPI_Finalize();
    }
}

void cleanup_and_exit(Grid* full_grid, Grid* local_current, Grid* local_next, int status) {
    static int cleaning = 0;
    if (cleaning) return;  // Prevent recursive cleanup
    cleaning = 1;

    // First cleanup grids
    if (local_next) {
        destroy_grid(local_next);
        local_next = NULL;
    }
    if (local_current) {
        destroy_grid(local_current);
        local_current = NULL;
    }
    if (full_grid) {
        destroy_grid(full_grid);
        full_grid = NULL;
    }

    // Then cleanup MPI
    cleanup_mpi();
    
    exit(status);
}

void signal_handler(int signum) {
    printf("\nReceived signal %d, cleaning up...\n", signum);
    MPI_Abort(MPI_COMM_WORLD, 1);
    exit(1);
}

// Add signal handler setup function
void setup_signal_handlers() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGSEGV, &sa, NULL);
}

// Add atexit handler for cleanup
void setup_cleanup() {
    atexit(cleanup_mpi);
}

int main(int argc, char* argv[]) {
    int rank, size;
    
    // Add cleanup handler
    setup_cleanup();
    
    // Use new signal handler setup
    setup_signal_handlers();

    // Initialize MPI with error checking
    if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
        fprintf(stderr, "Failed to initialize MPI\n");
        return 1;
    }
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3) {
        if (rank == 0) {
            fprintf(stderr, "Usage: %s <generations> <grid_size>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    Parameters params = parse_arguments(argc, argv);
    
    // Calculate local grid dimensions
    int rows_per_proc = params.grid_size / size;
    if (rank == size - 1) {
        rows_per_proc += params.grid_size % size;
    }

    Grid* local_current = NULL;
    Grid* local_next = NULL;
    Grid* full_grid = NULL;
    double start_time, end_time;

    // Allocate and initialize grids
    local_current = create_grid_local(params.grid_size, rows_per_proc);
    local_next = create_grid_local(params.grid_size, rows_per_proc);

    // Add error checking for grid creation
    if (!local_current || !local_next || 
        !local_current->cells || !local_next->cells) {
        fprintf(stderr, "Failed to allocate grid memory\n");
        cleanup_and_exit(full_grid, local_current, local_next, 1);
    }

    if (rank == 0) {
        full_grid = create_grid(params.grid_size);
        if (!full_grid) cleanup_and_exit(full_grid, local_current, local_next, 1);
        initialize_random_grid(full_grid);
        distribute_grid(full_grid, size);
        copy_grid_portion(local_current, full_grid, 0, rows_per_proc);
    } else {
        receive_grid_portion(local_current, rank, rows_per_proc, params.grid_size);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    // Main simulation loop with error checking and progress reporting
    int mpi_err;
    for (int gen = 0; gen < params.generations; gen++) {
        mpi_err = exchange_borders(local_current, rank, size);
        if (mpi_err != MPI_SUCCESS) {
            if (rank == 0) {
                fprintf(stderr, "Error in exchange_borders: %d\n", mpi_err);
            }
            cleanup_and_exit(full_grid, local_current, local_next, 1);
        }
        
        evolve_mpi(local_current, local_next, rows_per_proc);
        SWAP_GRIDS(local_current, local_next);
        
        // Improved progress reporting
        if (rank == 0 && (gen % 10 == 0 || gen == params.generations - 1)) {
            printf("\rProgress: %d/%d generations (%.1f%%)", 
                   gen + 1, params.generations, 
                   ((float)(gen + 1) / params.generations) * 100);
            fflush(stdout);
        }
    }
    
    // Clear progress line before final output
    if (rank == 0) {
        printf("\n");
    }

    MPI_Barrier(MPI_COMM_WORLD);  // Synchronize before timing
    end_time = MPI_Wtime();

    // Gather and print results
    if (rank == 0) {
        if (!full_grid) full_grid = create_grid(params.grid_size);
        if (full_grid) {
            gather_grid(full_grid, local_current, size);
            print_stats(full_grid, params.generations, end_time - start_time);
        }
    } else {
        send_grid_portion(local_current, rank, rows_per_proc, params.grid_size);
    }

    cleanup_and_exit(full_grid, local_current, local_next, 0);
    return 0;  // This line will never be reached
}
