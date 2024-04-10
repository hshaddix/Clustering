CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
TARGET = simulation
SRC = testIncluded.cpp

all: $(TARGET)

$(TARGET): $(SRC)
    $(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
    ./$(TARGET)

clean:
    rm -f $(TARGET)

.PHONY: all run clean
