#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

#include "grid.h"

void evolve_mpi(Grid* local_current, Grid* local_next, int rank, int size);
int count_neighbors(Grid* grid, int i, int j);

#endif
