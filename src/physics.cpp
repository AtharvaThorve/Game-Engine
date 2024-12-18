#include "physics.hpp"

PhysicsSystem::PhysicsSystem(float gravityX, float gravityY) :
	gravity{ gravityX, gravityY } {}

void PhysicsSystem::applyGravity(Entity& entity) {
	if (entity.isAffectedByGravity) {
		entity.acceleration.x = gravity.x;
		entity.acceleration.y = gravity.y;
	}
}