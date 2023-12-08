CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
TARGET = Cluster

all: $(TARGET)

$(TARGET): $(TARGET).o
    $(CXX) $(CXXFLAGS) $(TARGET).o -o $(TARGET)

$(TARGET).o: $(TARGET).cpp
    $(CXX) $(CXXFLAGS) -c $(TARGET).cpp

run_simple_case:
    @echo "Running simple merging case:"
    @echo "0000100001110100 0000100010000101" | ./$(TARGET)

run_empty_strip_case:
    @echo "Running empty strip and merged clusters case:"
    @echo "0000000000000000 0000100010010111 0000100010100110" | ./$(TARGET)

clean:
    rm -f $(TARGET) $(TARGET).o

.PHONY: all clean run_simple_case run_empty_strip_case
