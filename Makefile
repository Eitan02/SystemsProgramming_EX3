# eitan.derdiger@gmail.com

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I. -Isrc -g

# Source files
EXCEPTIONS = $(wildcard exceptions/*.cpp)
GAME = $(wildcard game/*.cpp)
ROLES = $(wildcard roles/*.cpp)
SRCS = $(GAME) $(ROLES) $(EXCEPTIONS)
OBJS = $(SRCS:.cpp=.o)

# Main executable
MAIN_OBJ = main.o
TARGET = Main

# Unit tests
TEST_SOURCES = $(wildcard tests/test_*.cpp)
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)
TEST_TARGET = TestRunner

.PHONY: all clean test valgrind

# Default target
all: Main

# Build demo executable
Main: $(MAIN_OBJ) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(MAIN_OBJ) $(OBJS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

# Compile each test file
$(TEST_OBJECTS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build unit test runner
TestRunner: $(TEST_OBJECTS) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_OBJECTS) $(OBJS)

# Run unit tests
test: TestRunner
	./$(TEST_TARGET)

# Memory leak check
valgrind: Main
	valgrind --leak-check=full --track-origins=yes ./$(TARGET)

# Clean build artifacts
clean:
	rm -f $(MAIN_OBJ) $(TEST_OBJECTS) $(OBJS) $(TARGET) $(TEST_TARGET)
