#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "../include/grid.h"
#include "../include/game_of_life.h"
#include "../include/utilities.h"

int main(int argc, char* argv[]) {
    Parameters params = parse_arguments(argc, argv);
    
    Grid* current = create_grid(params.grid_size);
    Grid* next = create_grid(params.grid_size);
    
    initialize_random_grid(current);
    
    double start_time = omp_get_wtime();
    
    for (int gen = 0; gen < params.generations; gen++) {
        if (params.is_parallel) {
            evolve_parallel(current, next, params.num_threads);
        } else {
            evolve_serial(current, next);
        }
        Grid* temp = current;
        current = next;
        next = temp;
    }
    
    double end_time = omp_get_wtime();
    print_stats(current, params.generations, end_time - start_time);
    
    destroy_grid(current);
    destroy_grid(next);
    
    return 0;
}
