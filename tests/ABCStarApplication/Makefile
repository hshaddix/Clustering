CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
TARGET = simulation

all: $(TARGET)

$(TARGET): $(TARGET).cpp
    $(CXX) $(CXXFLAGS) -o $(TARGET) $(TARGET).cpp

run: $(TARGET)
    ./$(TARGET)

clean:
    rm -f $(TARGET)

.PHONY: all run clean
