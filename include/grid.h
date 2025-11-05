#ifndef GRID_H
#define GRID_H

#define SWAP_GRIDS(a, b) do { Grid* temp = a; a = b; b = temp; } while(0)

// Grid structure representing the Game of Life board
typedef struct {
    int* cells;         // 1D array for better cache performance
    int size;          // Actual grid size (N)
    int padded_size;   // Size with padding for border cells
    int local_rows;    // Number of rows in this process
} Grid;

// Grid management functions
Grid* create_grid(int size);
Grid* create_grid_local(int size, int local_rows);
void destroy_grid(Grid* grid);
void initialize_random_grid(Grid* grid);
void distribute_grid(Grid* grid, int num_procs);
void gather_grid(Grid* full_grid, Grid* local_grid, int num_procs);
void copy_grid_portion(Grid* dest, Grid* src, int start_row, int num_rows);
void receive_grid_portion(Grid* grid, int rank, int rows, int total_size);
void send_grid_portion(Grid* grid, int rank, int rows, int total_size);
void copy_grid(Grid* dest, Grid* src);
int verify_grid_integrity(Grid* grid);
void cleanup_mpi(void);

#endif
