CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
TARGET = Cluster

all: $(TARGET)

$(TARGET): $(TARGET).o
	$(CXX) $(CXXFLAGS) $(TARGET).o -o $(TARGET)

$(TARGET).o: $(TARGET).cpp
	$(CXX) $(CXXFLAGS) -c $(TARGET).cpp

run_case1:
	@echo "Running case 1 (clusters at positions 50 and 52 on the same strip):"
	@./$(TARGET) <<< "0000001100110010 0000001100110100"

run_case2:
	@echo "Running case 2 (cluster at position 124 on one strip and at position 0 on the next strip):"
	@./$(TARGET) <<< "0000011111110001 0001000000000001"

clean:
	rm -f $(TARGET) $(TARGET).o

.PHONY: all clean run_case1 run_case2
