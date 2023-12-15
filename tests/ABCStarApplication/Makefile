CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
TARGET = Cluster

all: $(TARGET)

$(TARGET): $(TARGET).o
    $(CXX) $(CXXFLAGS) $(TARGET).o -o $(TARGET)

$(TARGET).o: $(TARGET).cpp
    $(CXX) $(CXXFLAGS) -c $(TARGET).cpp

run_test1:
    @echo "Running test 1 (merging on same strip):"
    @echo "0001001100100011 0001001101100001" | ./$(TARGET)

run_test2:
    @echo "Running test 2 (merging across strip boundaries):"
    @echo "0001011110110010 0010000000000001" | ./$(TARGET)

clean:
    rm -f $(TARGET) $(TARGET).o

.PHONY: all clean run_test1 run_test2
