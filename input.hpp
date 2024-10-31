#pragma once
#include "defs.hpp"
#include "entity.hpp"
#include "event_manager.hpp"
#include "scaling.hpp"
#include "structs.hpp"
#include <SDL2/SDL.h>
#include <optional>

void doInput(std::shared_ptr<Entity> entity, Timeline *globalTimeline,
             EventManager *em, float accelerationRate = 5,
             float dash_speed = 50, float decelerationRate = 5);