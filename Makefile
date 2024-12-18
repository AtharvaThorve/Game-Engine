# Compiler
CXX = g++

# Compiler Flags
CXXFLAGS = -Wall -std=c++17 -O2 $(shell sdl2-config --cflags) -I/usr/include/v8 -Iheaders

# Linker Flags (for SDL2)
LDFLAGS = $(shell sdl2-config --libs) -lSDL2main -lSDL2 -lzmq -lpthread -lv8

# Source files
SRC = src/main.cpp \
      src/cleanup.cpp \
      src/entity_manager.cpp \
      src/draw.cpp \
      src/init.cpp \
      src/physics.cpp \
      src/scaling.cpp \
      src/entity.cpp \
      src/input.cpp \
      src/movement_pattern.cpp \
      src/shape.cpp \
      src/structs.cpp \
      src/client.cpp \
      src/server.cpp \
      src/timeline.cpp \
      src/camera.cpp \
      src/collision_utils.cpp \
      src/event_manager.cpp \
      src/collision_handler.cpp \
      src/death_handler.cpp \
      src/respawn_handler.cpp \
      src/input_handler.cpp \
      src/disconnectHandler.cpp \
      src/replay_recorder.cpp \
      src/movement_handler.cpp \
      src/position_handler.cpp \
      src/v8helpers.cpp \
      src/script_manager.cpp \
      src/event_manager_bindings.cpp \
      src/entity_bindings.cpp \
# Object directory
OBJDIR = obj

# Object files
OBJ = $(SRC:src/%.cpp=$(OBJDIR)/%.o)

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
$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up object files and executable but keep the obj directory
clean:
	@if [ -d $(OBJDIR) ]; then rm -f $(OBJDIR)/*.o; fi
	rm -f $(EXEC)

# Phony targets
.PHONY: all clean
