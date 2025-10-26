# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -std=c++11

# Target executable
TARGET = A4

# Source files
SRCS = main.cpp TeamBuilder.cpp Utilities.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Rule for building the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule for building object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule for cleaning up build files
clean:
	rm -f $(TARGET) $(OBJS)

# Rule for running the program
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
