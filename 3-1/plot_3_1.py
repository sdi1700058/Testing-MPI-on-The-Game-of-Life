import pandas as pd
import matplotlib.pyplot as plt
import os
import sys

# Constants
FIG_DIR = "test/fig"
RESULTS_FILE = "test/results/mpi_performance.txt"

# Create figure directory
os.makedirs(FIG_DIR, exist_ok=True)

def plot_performance():
    # Read data
    df = pd.read_csv(RESULTS_FILE, sep=' ', comment='#',
                     names=['size', 'processes', 'time'])
    
    # Plot for each grid size
    for size in df['size'].unique():
        size_data = df[df['size'] == size]
        
        # Calculate speedup
        base_time = size_data[size_data['processes'] == 1]['time'].iloc[0]
        speedup = base_time / size_data['time']
        
        # Create subplot with two y-axes
        fig, ax1 = plt.subplots(figsize=(10, 6))
        ax2 = ax1.twinx()
        
        # Plot execution time and speedup
        ln1 = ax1.plot(size_data['processes'], size_data['time'],
                      'b-o', label='Execution Time')
        ln2 = ax2.plot(size_data['processes'], speedup,
                      'r-o', label='Speedup')
        
        # Labels and title
        ax1.set_xlabel('Number of Processes')
        ax1.set_ylabel('Execution Time (seconds)')
        ax2.set_ylabel('Speedup')
        plt.title(f'MPI Performance - Grid Size {size}x{size}')
        
        # Legend
        lns = ln1 + ln2
        labs = [l.get_label() for l in lns]
        ax1.legend(lns, labs, loc='center right')
        
        plt.savefig(f'{FIG_DIR}/mpi_performance_{size}.png')
        plt.close()

if __name__ == "__main__":
    if not os.path.exists(RESULTS_FILE):
        print(f"Error: Could not find results file: {RESULTS_FILE}")
        sys.exit(1)
    try:
        plot_performance()
    except Exception as e:
        print(f"Error processing results: {e}")
        sys.exit(1)
