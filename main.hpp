#pragma once
#include "structs.hpp"
#include "init.hpp"
#include "draw.hpp"
#include "input.hpp"
#include "Physics.hpp"
#include "Entity.hpp"
#include "cleanup.hpp"
#include <memory>
#include "SDL2/SDL.h"

// SDL render and window context
extern App* app;

int main(int argc, char* argv[]);

void setRenderScale(float scaleX, float scaleY);