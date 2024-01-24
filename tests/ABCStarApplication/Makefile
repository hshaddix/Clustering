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
	@echo "0000000110010010 0000000110100010" | ./$(TARGET)

run_case2:
	@echo "Running case 2:"
	@echo "0000001111100010 0000100000000010" | ./$(TARGET)


run_case3:
	@echo "Running case 3:"
	@echo "0000101111100100 0000100110110010" | ./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).o

.PHONY: all clean run_case1 run_case2
