#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include "defs.hpp"
#include "structs.hpp"

extern App* app;
void setRenderScale(float scaleX, float scaleY);
void updateScaleFactor(float& scale);

extern bool allowScaling;