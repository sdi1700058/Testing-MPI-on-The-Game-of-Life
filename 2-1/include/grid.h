#ifndef GRID_H
#define GRID_H

// Grid structure representing the Game of Life board
typedef struct {
    int* cells;         // 1D array for better cache performance
    int size;          // Actual grid size (N)
    int padded_size;   // Size with padding for border cells
} Grid;

// Grid management functions
Grid* create_grid(int size);
void destroy_grid(Grid* grid);
void initialize_random_grid(Grid* grid);
void copy_grid(Grid* dest, Grid* src);

#endif
