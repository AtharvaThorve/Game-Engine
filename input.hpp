#pragma once
#include "defs.hpp"
#include "entity.hpp"
#include "scaling.hpp"
#include "structs.hpp"
#include <SDL2/SDL.h>

void doInput(std::shared_ptr<Entity> entity, Timeline *globalTimeline,
             float accelerationRate = 5, float decelerationRate = 5);