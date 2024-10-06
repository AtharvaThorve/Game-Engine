# Compiler
CXX = g++

# Compiler Flags
CXXFLAGS = -Wall -std=c++17 -I./include $(shell sdl2-config --cflags) -fPIC

# Linker Flags (for SDL2)
LDFLAGS = $(shell sdl2-config --libs) -lSDL2main -lSDL2 -lzmq -lpthread

# SUBSYSTEM = ./src/subsystem
# UTILS = ./src/utils

# Source files
SRC = main.cpp \
      cleanup.cpp \
      EntityManager.cpp \
      draw.cpp \
      init.cpp \
      physics.cpp \
      scaling.cpp \
      entity.cpp \
      input.cpp \

# Object files
OBJ = $(SRC:.cpp=.o)

# Executable name
EXEC = main

# Default target
all: $(EXEC)

# Linking the executable
$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $(EXEC) $(LDFLAGS)

# Compiling each .cpp file into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up object files and executable
clean:
	rm -f $(OBJ) $(EXEC)

# Phony targets
.PHONY: all clean