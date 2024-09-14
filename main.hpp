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
#include "scaling.hpp"
#include "EntityManager.hpp"
#include <vector>
#include <random>

// SDL render and window context
extern App* app;

int main(int argc, char* argv[]);

void setRenderScale(float scaleX, float scaleY);