#pragma once
#include <SDL2/SDL.h>
#include <string>

// Define struct that contains pointers to the renderer and window
typedef struct {
  SDL_Renderer *renderer;
  SDL_Window *window;
} App;

// Vector of 2 variables
typedef struct {
  float x;
  float y;
} Vector2;
