CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
TARGET = Cluster

all: $(TARGET)

$(TARGET): $(TARGET).o
    $(CXX) $(CXXFLAGS) $(TARGET).o -o $(TARGET)

$(TARGET).o: $(TARGET).cpp
    $(CXX) $(CXXFLAGS) -c $(TARGET).cpp

run_test1:
    @echo "Running test 1:"
    @echo "0000101100110100 0000101100110110" | ./$(TARGET)

run_test2:
    @echo "Running test 2:"
    @echo "0000101101111011 0000101110000010" | ./$(TARGET)

clean:
    rm -f $(TARGET) $(TARGET).o

.PHONY: all clean run_test1 run_test2
