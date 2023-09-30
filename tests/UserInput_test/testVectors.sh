#!/bin/bash

# Compile the C++ code
g++ UserInput_cluster.cpp -o clustering

# Define test cases
declare -a test_cases=(
    "1 5 8 12 18 22"
    "11 12 13 14"
    "0 1 2 3 4"
    "97 99 100"
)

# Run the program with each test case
for test_case in "${test_cases[@]}"
do
    echo "Running test case: $test_case"
    echo $test_case | ./clustering
    echo "----------------------------------------"
done

# Clean up
rm clustering
