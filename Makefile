# eitan.derdiger@gmail.com

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I. -Isrc -g

# מקור המשחק
EXCEPTIONS = $(wildcard exceptions/*.cpp)
GAME = $(wildcard game/*.cpp)
ROLES = $(wildcard roles/*.cpp)
SRCS = $(GAME) $(ROLES) $(EXCEPTIONS)
OBJS = $(SRCS:.cpp=.o)

# ראשי
MAIN_OBJ = main.o
TARGET = Main

# טסטים
TEST_SOURCES = $(wildcard tests/test_*.cpp)
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)
TEST_TARGET = TestRunner

.PHONY: all clean test valgrind

# ברירת מחדל
all: Main

# הדגמה
Main: $(MAIN_OBJ) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(MAIN_OBJ) $(OBJS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

# קומפילציית קבצי בדיקה
$(TEST_OBJECTS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# קובץ הרצה של הבדיקות
TestRunner: $(TEST_OBJECTS) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_OBJECTS) $(OBJS)

# הרצת הבדיקות
test: TestRunner
	./$(TEST_TARGET)

# בדיקת זיכרון
valgrind: Main
	valgrind --leak-check=full --track-origins=yes ./$(TARGET)

# ניקוי
clean:
	rm -f $(MAIN_OBJ) $(TEST_OBJECTS) $(OBJS) $(TARGET) $(TEST_TARGET)
