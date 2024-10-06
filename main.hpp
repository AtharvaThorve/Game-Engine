#pragma once
#include "structs.hpp"
#include "init.hpp"
#include "draw.hpp"
#include "input.hpp"
#include "physics.hpp"
#include "entity.hpp"
#include "cleanup.hpp"
#include <memory>
#include <SDL2/SDL.h>
#include "scaling.hpp"
#include "EntityManager.hpp"
#include "rushil_game1.hpp"

// SDL render and window context
extern App* app;

int main(int argc, char* argv[]);

void setRenderScale(float scaleX, float scaleY);