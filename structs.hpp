#pragma once
#include <SDL2/SDL.h>
#include <string>

//A struct to contain the pointers to the SDL Renderer and SDL window.
typedef struct {
	SDL_Renderer* renderer;
	SDL_Window* window;
} App;

// Vector of 2 variables
typedef struct {
	float x;
	float y;
} Vector2;


typedef struct {
	Vector2 gravity;
} PhysicsSystem;