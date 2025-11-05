#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/utilities.h"

Parameters parse_arguments(int argc, char* argv[]) {
    Parameters params = {0};  // Initialize to zero

    // Check if help is requested
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        printf("Game of Life MPI Implementation\n");
        printf("Usage: %s <generations> <grid_size>\n\n", argv[0]);
        printf("Arguments:\n");
        printf("  generations  : Number of generations to simulate (positive integer)\n");
        printf("  grid_size   : Size of the grid (positive integer)\n\n");
        printf("Example:\n");
        printf("  %s 1000 64    # Simulate 1000 generations on a 64x64 grid\n", argv[0]);
        exit(0);
    }

    if (argc != 3) {
        fprintf(stderr, "Error: Wrong number of arguments\n");
        fprintf(stderr, "Usage: %s <generations> <grid_size>\n", argv[0]);
        fprintf(stderr, "Try '%s --help' for more information\n", argv[0]);
        exit(1);
    }
    
    // Parse generations with error checking
    char* endptr;
    params.generations = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || params.generations <= 0) {
        fprintf(stderr, "Error: generations must be a positive integer\n");
        exit(1);
    }
    
    // Parse grid_size with error checking
    params.grid_size = strtol(argv[2], &endptr, 10);
    if (*endptr != '\0' || params.grid_size <= 0) {
        fprintf(stderr, "Error: grid_size must be a positive integer\n");
        exit(1);
    }

    // Additional validation
    validate_parameters(&params);
    return params;
}

void validate_parameters(Parameters* params) {
    if (params->generations <= 0) {
        fprintf(stderr, "Error: Number of generations must be positive (got %d)\n", 
                params->generations);
        exit(1);
    }
    
    if (params->grid_size <= 0) {
        fprintf(stderr, "Error: Grid size must be positive (got %d)\n", 
                params->grid_size);
        exit(1);
    }

    // Add reasonable upper limits to prevent memory issues
    if (params->grid_size > 10000) {
        fprintf(stderr, "Error: Grid size too large (maximum: 10000, got %d)\n", 
                params->grid_size);
        exit(1);
    }
}

void print_grid(Grid* grid) {
    for (int i = 1; i <= grid->size; i++) {
        for (int j = 1; j <= grid->size; j++) {  // Changed from i to j
            printf("%c ", grid->cells[i * grid->padded_size + j] ? '#' : '.');
        }
        printf("\n");
    }
    printf("\n");
}

int count_population(Grid* grid) {
    int count = 0;
    for (int i = 1; i <= grid->size; i++) {
        for (int j = 1; j <= grid->size; j++) {  // Changed from i to j
            count += grid->cells[i * grid->padded_size + j];
        }
    }
    return count;
}

void print_stats(Grid* grid, int generation, double time) {
    printf("Generation: %d\n", generation);
    printf("Population: %d\n", count_population(grid));
    printf("Time taken: %.4f seconds\n", time);
}
