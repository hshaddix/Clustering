#!/bin/bash

# Compile the C++ program
g++ -std=c++11 -o MergeClusters MergeClusters.cpp

# Run the program with the provided arguments
./MergeClusters "$@"
