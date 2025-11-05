#!/bin/bash

# Configuration
SIZES=(64 1024 4096)
GENERATIONS=1000
PROCESSES=(1 2 4 8 16)
OUTPUT_FILE="test/results/mpi_performance.txt"
MACHINES_FILE="../Game-Of-Life/machines"

# Ensure directories exist
mkdir -p test/results
mkdir -p test/fig

# Create executable
make

# Clear previous results
echo "# Format: size processes time" > "$OUTPUT_FILE"

# Function to run either locally or distributed
run_mpi() {
    local size=$1
    local procs=$2
    
    if [ -f "$MACHINES_FILE" ] && [ "$USE_CLUSTER" = "yes" ]; then
        # Distributed run
        mpiexec -f $MACHINES_FILE -n $procs ./bin/game_of_life $GENERATIONS $size
    else
        # Local run
        mpiexec -n $procs ./bin/game_of_life $GENERATIONS $size
    fi
}

# Ask user about execution mode
read -p "Run on cluster? (yes/no) " USE_CLUSTER

# Run tests for each configuration
for size in "${SIZES[@]}"; do
    for p in "${PROCESSES[@]}"; do
        echo "Testing ${size}x${size} grid with $p processes"
        time_output=$(run_mpi $size $p 2>&1)
        if [ $? -eq 0 ]; then
            exec_time=$(echo "$time_output" | grep "Time" | cut -d' ' -f3)
            echo "$size $p $exec_time" >> "$OUTPUT_FILE"
        else
            echo "Error running test with size=$size processes=$p"
        fi
    done
done

# Generate plots only if user confirms
read -p "Do you want to generate plots? (yes/no) " answer
if [[ $answer == "yes" ]]; then
    if command -v python3 &> /dev/null; then
        python3 plot.py
    else
        echo "Python3 not found, skipping plot generation"
    fi
else
    echo "Skipping plot generation"
fi

echo "Results saved to $OUTPUT_FILE"
