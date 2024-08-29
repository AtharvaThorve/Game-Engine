#pragma once
#include "structs.hpp"
#include <SDL2/SDL.h>

extern App* app;

void prepareScene(SDL_Color);
void presentScene(void);
void drawRect(int x, int y, int h, int w);
void colorRect(SDL_Color);