#include "Physics.hpp"

PhysicsSystem::PhysicsSystem(float gravityX, float gravityY) :
	gravity{ gravityX, gravityY } {}

void PhysicsSystem::applyGravity(Entity& entity, float deltaTime) {
	if (entity.isAffectedByGravity) {
		entity.velocity.x += gravity.x * deltaTime;
		entity.velocity.y += gravity.y * deltaTime;
	}
}