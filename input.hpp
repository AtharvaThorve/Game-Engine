#pragma once

#include "client.hpp"
#include "defs.hpp"
#include "entity.hpp"
#include "event_manager.hpp"
#include "scaling.hpp"
#include "structs.hpp"
#include <SDL2/SDL.h>
#include <list>
#include <optional>
#include <unordered_set>

// Helper function for raising movement events
void raiseMovementEvent(const std::string &input_type,
                        std::shared_ptr<Entity> entity, float rate,
                        Timeline *timeline);

// Function to check if the current direction combination is valid
bool isValidDirectionCombo(const std::unordered_set<size_t> &directions);

// Function to process dash logic with validated directions
void processDashInput(std::shared_ptr<Entity> entity, Timeline *timeline,
                      float dash_speed, float dash_duration);

// Function to handle player input
void doInput(std::shared_ptr<Entity> entity, Timeline *globalTimeline,
             EntityManager &entityManager,
             std::shared_ptr<std::unordered_set<std::shared_ptr<Entity>>> playerBullets,
             float accelerationRate = 5, float dash_speed = 50,
             float dash_duration = 3000000000);
