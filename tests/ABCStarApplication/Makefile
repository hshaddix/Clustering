CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
TARGET = Cluster

all: $(TARGET)

$(TARGET): Cluster.o
	$(CXX) $(CXXFLAGS) Cluster.o -o $(TARGET)

Cluster.o: Cluster.cpp
	$(CXX) $(CXXFLAGS) -c Cluster.cpp

run_test1:
	@echo "Running test 1 (merging on same strip):"
	@./$(TARGET) <<< "0000001100110010 0000001100110100"

run_test2:
	@echo "Running test 2 (merging across strip boundaries):"
	@./$(TARGET) <<< "0000011111110001 0001000000000001"

clean:
	rm -f $(TARGET) Cluster.o

.PHONY: all clean run_test1 run_test2
