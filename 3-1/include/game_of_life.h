#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

#include "grid.h"

void evolve_serial(Grid* current, Grid* next);
void evolve_mpi(Grid* current, Grid* next, int local_rows);
int exchange_borders(Grid* grid, int rank, int size);  // Updated to return int
int count_neighbors(Grid* grid, int i, int j);

#endif
