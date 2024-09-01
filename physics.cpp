#include "physics.hpp"
#include <iostream>

PhysicsSystem* physicsSystem = new PhysicsSystem();

void init_physics_system(float gravityX, float gravityY)
{
	physicsSystem->gravity.x = gravityX;
	physicsSystem->gravity.y = gravityY;
}

//void apply_gravity(Entity* entity, float deltaTime)
//{
//	if (entity->isAffectedByGravity) {
//		entity->velocity.x += physicsSystem->gravity.x * deltaTime;
//		entity->velocity.y += physicsSystem->gravity.y * deltaTime;
//	}
//}

//void update_entity(Entity* entity, float deltaTime)
//{
//	entity->position.x += entity->velocity.x * deltaTime;
//	entity->position.y += entity->velocity.y * deltaTime;
//}