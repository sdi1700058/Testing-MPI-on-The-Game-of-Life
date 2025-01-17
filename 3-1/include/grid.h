#ifndef GRID_H
#define GRID_H

typedef struct {
    int* cells;
    int size;
    int padded_size;
} Grid;

Grid* create_grid(int size);
Grid* create_local_grid(int local_size, int total_size);
void destroy_grid(Grid* grid);
void initialize_random_grid(Grid* grid);
void distribute_grid(Grid* grid, Grid* local_grid, int rank, int size);
void gather_grid(Grid* local_grid, Grid* grid, int rank, int size);
void update_ghost_rows(Grid* local_grid, int rank, int size);

#endif
