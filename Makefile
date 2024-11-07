SRC_DIR = src
BUILD_DIR = build/debug
CC = g++
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_NAME = Chip8
INCLUDE_PATHS = $(shell sdl2-config --cflags) -Iinclude
LIBRARY_PATHS = $(shell sdl2-config --libs)
COMPILER_FLAGS = -Wall -std=c++11 -O0 -g

all: 
	$(CC) $(COMPILER_FLAGS) $(INCLUDE_PATHS) $(SRC_FILES) $(LIBRARY_PATHS) -o $(BUILD_DIR)/$(OBJ_NAME)
