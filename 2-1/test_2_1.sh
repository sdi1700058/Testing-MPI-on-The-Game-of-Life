#!/bin/bash

# Configuration
SIZES=(64 1024 4096)
GENERATIONS=1000
THREADS=(1 2 4 8 16)
OUTPUT_FILE="test/results/performance_results.txt"

# Ensure directories exist
mkdir -p test/results
mkdir -p test/fig

# Create executable
make

# Clear previous results
echo "# Format: size threads type time" > "$OUTPUT_FILE"

# Run tests for each configuration
for size in "${SIZES[@]}"; do
    echo "Testing ${size}x${size} grid - Serial"
    time_output=$(./bin/game_of_life $GENERATIONS $size serial 1 2>&1)
    if [ $? -eq 0 ]; then
        serial_time=$(echo "$time_output" | grep "Time" | cut -d' ' -f3)
        echo "$size 1 serial $serial_time" >> "$OUTPUT_FILE"
    fi
    
    for t in "${THREADS[@]}"; do
        echo "Testing ${size}x${size} grid - $t threads"
        time_output=$(./bin/game_of_life $GENERATIONS $size parallel $t 2>&1)
        if [ $? -eq 0 ]; then
            parallel_time=$(echo "$time_output" | grep "Time" | cut -d' ' -f3)
            echo "$size $t parallel $parallel_time" >> "$OUTPUT_FILE"
        fi
    done
done

# Generate plots if python script exists
if [ -f "plot_2_1.py" ]; then
    python3 plot_2_1.py
fi

echo "Results saved to $OUTPUT_FILE"