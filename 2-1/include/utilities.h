#ifndef UTILITIES_H
#define UTILITIES_H

#include "grid.h"

typedef struct {
    int generations;
    int grid_size;
    int is_parallel;
    int num_threads;
} Parameters;

Parameters parse_arguments(int argc, char* argv[]);
void print_grid(Grid* grid);
int count_population(Grid* grid);
void print_stats(Grid* grid, int generation, double time);
void validate_parameters(Parameters* params);

#endif
