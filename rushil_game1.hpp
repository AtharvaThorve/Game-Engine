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
#include "MovementPattern.hpp"

// SDL render and window context
extern App* app;

int rushil_game1();