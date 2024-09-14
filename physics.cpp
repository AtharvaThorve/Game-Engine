#include "Physics.hpp"

PhysicsSystem::PhysicsSystem(float gravityX, float gravityY) :
	gravity{ gravityX, gravityY } {}

void PhysicsSystem::applyGravity(Entity& entity, float deltaTime) {
	//int64_t currentTime = physicsTimeline.getTime();
	//deltaTime = (currentTime - lastUpdateTime) / 1000000000.0f; // Nanoseconds to seconds
	//lastUpdateTime = currentTime;

	std::cout << deltaTime << std::endl;

	if (entity.isAffectedByGravity) {
		entity.velocity.x += gravity.x * deltaTime;
		entity.velocity.y += gravity.y * deltaTime;
	}
}