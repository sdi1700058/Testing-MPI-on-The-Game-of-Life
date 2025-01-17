#ifndef UTILITIES_H
#define UTILITIES_H

#include "grid.h"

typedef struct {
    int generations;
    int grid_size;
} Parameters;

Parameters parse_arguments(int argc, char* argv[]);
void validate_parameters(Parameters* params);
void print_grid(Grid* grid);
void print_stats(Grid* grid, int generation, double time);
int count_population(Grid* grid);

#endif
