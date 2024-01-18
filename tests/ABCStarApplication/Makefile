CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
TARGET = Cluster

all: $(TARGET)

$(TARGET): $(TARGET).o
	$(CXX) $(CXXFLAGS) $(TARGET).o -o $(TARGET)

$(TARGET).o: $(TARGET).cpp
	$(CXX) $(CXXFLAGS) -c $(TARGET).cpp

run_case1:
	@echo "Running case 1:"
	@./$(TARGET) <<< "0000001100110010 0000001100110100"

run_case2:
	@echo "Running case 2:"
	@./$(TARGET) <<< "0000011111110001 0001000000000001"

clean:
	rm -f $(TARGET) $(TARGET).o

.PHONY: all clean run_case1 run_case2
