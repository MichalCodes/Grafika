CXX = g++
LIBS = -lglfw -lGL -lGLEW
FLAGS = -Wall -pedantic -I/usr/include -I/usr/local/include
TARGET = main

SRC_DIR = src
OBJ_DIR = out

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# tinyobjloader single compilation unit
TINY_SRC = tinyobjloader/tiny_obj_loader.cc
TINY_OBJ = $(OBJ_DIR)/tiny_obj_loader.o

.PHONY: clean all

all: $(TARGET)

# Vytvoření cíle (binárky)
$(TARGET): $(OBJS) $(TINY_OBJ)
	$(CXX) -o $@ $^ $(LIBS) -fsanitize=address

# Kompilace .cpp -> .o do složky out/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(FLAGS) -c -o $@ $<

# Compile tinyobjloader implementation
$(TINY_OBJ): $(TINY_SRC) | $(OBJ_DIR)
	$(CXX) $(FLAGS) -c -o $@ $<

# Vytvoření složky out/ pokud neexistuje
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
