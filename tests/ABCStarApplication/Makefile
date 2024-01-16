CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
TARGET = Cluster

all: $(TARGET)

$(TARGET): Cluster.o
\t$(CXX) $(CXXFLAGS) Cluster.o -o $(TARGET)

Cluster.o: Cluster.cpp
\t$(CXX) $(CXXFLAGS) -c Cluster.cpp

run_test1:
\t@echo "Running test 1 (merging on same strip):"
\t@./$(TARGET) <<< "0000000110010001 0000000110100100"

run_test2:
\t@echo "Running test 2 (merging across strip boundaries):"
\t@./$(TARGET) <<< "0000011111100001 0001000000000001"

clean:
\trm -f $(TARGET) Cluster.o

.PHONY: all clean run_test1 run_test2
