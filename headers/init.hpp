#pragma once
#include "structs.hpp"
#include "defs.hpp"
#include "physics.hpp"
#include <SDL2/SDL.h>
#include <iostream>

extern App* app;

// Initialize SDL rendering window
void initSDL(void);

// void initPhysics(float gravityX, float gravityY);