CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
TARGET = bitCluster

all: $(TARGET)

$(TARGET): $(TARGET).o
	$(CXX) $(CXXFLAGS) $(TARGET).o -o $(TARGET)

$(TARGET).o: $(TARGET).cpp
	$(CXX) $(CXXFLAGS) -c $(TARGET).cpp

run_case1:
	@echo "Running case 1 (3 4 f9 2 1 2):"
	@echo "00000011000001000 11111001000000101 00000001000000101" | ./$(TARGET)

run_case2:
	@echo "Running case 2 (Empty first strip, f9 2 1 2):"
	@echo "00000000000000001 11111001000000101 00000001000000101" | ./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).o

.PHONY: all clean run_case1 run_case2
