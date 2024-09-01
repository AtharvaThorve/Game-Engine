#pragma once
#include "structs.hpp"
#include "init.hpp"
#include "draw.hpp"
#include "input.hpp"
#include "physics.hpp"
#include "entity.hpp"
#include "cleanup.hpp"
#include "Rectangle.hpp"
#include <memory>
#include "SDL2/SDL.h"

// SDL render and window context
extern App* app;

int main(int argc, char* argv[]);