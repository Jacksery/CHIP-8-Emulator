SRC_DIR = src
BUILD_DIR = build/debug
CC = g++
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_NAME = Chip8
INCLUDE_PATHS = -I /opt/homebrew/Cellar/sdl2/2.30.9/include/ -I include
LIBRARY_PATHS = -L /opt/homebrew/Cellar/sdl2/2.30.9/lib/
COMPILER_FLAGS = -Wall -std=c++11 -O0 -g
LINKER_FLAGS = -lsdl2

all: 
	$(CC) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(SRC_FILES) -o $(BUILD_DIR)/$(OBJ_NAME)