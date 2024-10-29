# Compiler
CXX = g++

# Compiler Flags
CXXFLAGS = -Wall -std=c++17 -O2 $(shell sdl2-config --cflags)

# Linker Flags (for SDL2)
LDFLAGS = $(shell sdl2-config --libs) -lSDL2main -lSDL2 -lzmq -lpthread

# Source files
SRC = main.cpp \
      cleanup.cpp \
      entity_manager.cpp \
      draw.cpp \
      init.cpp \
      physics.cpp \
      scaling.cpp \
      entity.cpp \
      input.cpp \
      movement_pattern.cpp \
      shape.cpp \
      structs.cpp \
      client.cpp \
      server.cpp \
      timeline.cpp \
      camera.cpp \
      collision_utils.cpp \
      event_manager.cpp \
      collision_handler.cpp \
      death_handler.cpp \
      respawn_handler.cpp \
      input_handler.cpp

# Object directory
OBJDIR = obj

# Object files
OBJ = $(SRC:%.cpp=$(OBJDIR)/%.o)

# Executable name
EXEC = main

# Default target
all: $(EXEC)

# Ensure obj directory exists
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Linking the executable
$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $(EXEC) $(LDFLAGS)

# Compiling each .cpp file into .o files and place them in obj directory
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up object files and executable but keep the obj directory
clean:
	@if [ -d $(OBJDIR) ]; then rm -f $(OBJDIR)/*.o; fi
	rm -f $(EXEC)

# Phony targets
.PHONY: all clean
