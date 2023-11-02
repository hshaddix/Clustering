# Specify the compiler                                                                                                                                                                                      
CXX=g++

# Specify the flags                                                                                                                                                                                         
CXXFLAGS=-std=c++11 -Wall -Wextra

# Target executable name                                                                                                                                                                                    
TARGET=MergeClusters

# Source files                                                                                                                                                                                              
SOURCES=MergeClusters.cpp

# Object files                                                                                                                                                                                              
OBJECTS=$(SOURCES:.cpp=.o)

# Rule for making the actual target                                                                                                                                                                         
$(TARGET): $(OBJECTS)
        $(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

# Rule for making the object files                                                                                                                                                                          
%.o: %.cpp
        $(CXX) $(CXXFLAGS) -c $< -o $@

# Rule for cleaning the project                                                                                                                                                                             
clean:
        rm -f $(OBJECTS) $(TARGET)

# Rule for running the program                                                                                                                                                                              
run: $(TARGET)
        ./$(TARGET)

# Dependency rules for object files                                                                                                                                                                         
MergeClusters.o: MergeClusters.cpp

