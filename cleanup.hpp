#pragma once
#include "physics.hpp"
#include "structs.hpp"
#include "entity.hpp"

class EntityManager;

extern App* app;
extern PhysicsSystem* physicsSystem;

void clean_up_sdl(void);
void clean_up_physics_system(void);
//void clean_up_entities(EntityManager* entityManager);