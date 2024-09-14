#pragma once
#include <SDL2/SDL.h>
#include "structs.hpp"
#include "defs.hpp"
#include "entity.hpp"
#include "scaling.hpp"

void doInput(std::shared_ptr<Entity> entity, Timeline* globalTimeline, float move_speedX=5, float move_speedY=5);