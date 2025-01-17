import pandas as pd
import matplotlib.pyplot as plt
import os
import sys

# Constants
FIG_DIR = "test/fig"
RESULTS_FILE = "test/results/performance_results.txt"

# Create figure directory
os.makedirs(FIG_DIR, exist_ok=True)

# Check if results file exists
if not os.path.exists(RESULTS_FILE):
    print(f"Error: Could not find results file: {RESULTS_FILE}")
    sys.exit(1)

try:
    # Read data
    df = pd.read_csv(RESULTS_FILE, sep=' ', comment='#',
                     names=['size', 'threads', 'type', 'time'])

    # Plot settings - use a simpler style
    plt.style.use('default')
    plt.rcParams['figure.figsize'] = [10, 6]
    plt.rcParams['lines.markersize'] = 8
    plt.rcParams['lines.linewidth'] = 2
    plt.rcParams['font.size'] = 12
    plt.rcParams['axes.grid'] = True

    def plot_size_performance(size_data, size):
        plt.figure(figsize=(10, 6))
        
        # Get serial time for this size
        serial_time = size_data[size_data['type'] == 'serial']['time'].iloc[0]
        
        # Plot parallel performance
        parallel_data = size_data[size_data['type'] == 'parallel']
        
        # Speedup calculation
        speedup = serial_time / parallel_data['time']
        
        # Create subplot with two y-axes
        fig, ax1 = plt.subplots(figsize=(10, 6))
        ax2 = ax1.twinx()
        
        # Use better colors
        ln1 = ax1.plot(parallel_data['threads'], parallel_data['time'],
                      '-o', label='Execution Time', color='#1f77b4')
        ax1.axhline(y=serial_time, color='#d62728', linestyle='--',
                    label='Serial Time')
        ln2 = ax2.plot(parallel_data['threads'], speedup,
                      '-o', label='Speedup', color='#2ca02c')
        
        # Add labels and title
        ax1.set_xlabel('Number of Threads')
        ax1.set_ylabel('Execution Time (seconds)')
        ax2.set_ylabel('Speedup')
        plt.title(f'Performance Analysis - Grid Size {size}x{size}')
        
        # Fix legend creation
        lns = ln1 + ln2
        labs = [l.get_label() for l in lns]  # Fixed: 'l in lns' instead of 'lns'
        ax1.legend(lns, labs, loc='center right')
        
        plt.savefig(f'{FIG_DIR}/performance_{size}.png')
        plt.close()

    # Generate individual plots for each grid size
    for size in df['size'].unique():
        size_data = df[df['size'] == size]
        plot_size_performance(size_data, size)

    # Generate comparison plot
    plt.figure(figsize=(12, 8))
    for size in df['size'].unique():
        size_data = df[df['size'] == size]
        parallel_data = size_data[size_data['type'] == 'parallel']
        plt.plot(parallel_data['threads'], parallel_data['time'],
                 marker='o', label=f'{size}x{size}')

    plt.xlabel('Number of Threads')
    plt.ylabel('Execution Time (seconds)')
    plt.title('Performance Comparison - All Grid Sizes')
    plt.legend()
    plt.grid(True)
    plt.savefig(f'{FIG_DIR}/comparison_all_sizes.png')
    plt.close()

except Exception as e:
    print(f"Error processing results: {e}")
    sys.exit(1)
