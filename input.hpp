#pragma once
#include <SDL2/SDL.h>
#include "structs.hpp"
#include "defs.hpp"
#include "entity.hpp"
#include "scaling.hpp"

void doInput(std::shared_ptr<Entity> entity, float moveSpeed);