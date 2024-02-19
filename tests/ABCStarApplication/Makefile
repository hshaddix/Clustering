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
    @echo "000000110010010" | ./$(TARGET)

run_case2:
    @echo "Running case 2:"
    @echo "000000110010101 000000110110100" | ./$(TARGET)

run_case3:
    @echo "Running case 3:"
    @echo "0000001111010111 0000100000000110" | ./$(TARGET) 

clean:
    rm -f $(TARGET) $(TARGET).o

.PHONY: all clean run_case1 run_case2 run_case3
