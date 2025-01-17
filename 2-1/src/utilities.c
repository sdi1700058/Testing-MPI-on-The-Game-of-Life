#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/utilities.h"  // Fixed include path

Parameters parse_arguments(int argc, char* argv[]) {
    Parameters params;
    if (argc != 5) {
        printf("Usage: %s <generations> <grid_size> <serial|parallel> <num_threads>\n", argv[0]);
        exit(1);
    }
    
    params.generations = atoi(argv[1]);
    params.grid_size = atoi(argv[2]);
    params.is_parallel = strcmp(argv[3], "parallel") == 0;
    params.num_threads = atoi(argv[4]);
    
    validate_parameters(&params);
    return params;
}

void validate_parameters(Parameters* params) {
    if (params->generations <= 0 || params->grid_size <= 0 || params->num_threads <= 0) {
        printf("Error: Invalid parameters\n");
        exit(1);
    }
}

void print_grid(Grid* grid) {
    for (int i = 1; i <= grid->size; i++) {
        for (int j = 1; j <= grid->size; j++) {
            printf("%c ", grid->cells[i * grid->padded_size + j] ? '#' : '.');
        }
        printf("\n");
    }
    printf("\n");
}

int count_population(Grid* grid) {
    int count = 0;
    for (int i = 1; i <= grid->size; i++) {
        for (int j = 1; j <= grid->size; j++) {
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
