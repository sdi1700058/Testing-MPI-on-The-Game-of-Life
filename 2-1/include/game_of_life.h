#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

#include "grid.h"

void evolve_serial(Grid* current, Grid* next);
void evolve_parallel(Grid* current, Grid* next, int num_threads);
int count_neighbors(Grid* grid, int i, int j);  // Updated signature

#endif
