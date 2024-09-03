#include "EntityManager.hpp"
#include "BoundingBox.hpp"

void EntityManager::addEntity(std::shared_ptr<Entity> entity) {
	entities.insert(entity);
}

void EntityManager::removeEntity(std::shared_ptr<Entity> entity) {
	entities.erase(entity);
}

void EntityManager::updateEntities(float deltaTime, BoundingBox& boundingBox) {
	for (const auto& entity : entities) {
		if (entity->hasMovementPattern) {
			entity->movementPattern.update(deltaTime, *entity);
		}
		entity->updatePosition(deltaTime);
		boundingBox.enforceBoundaries(entity);
	}
}

void EntityManager::updateEntities(float deltaTime, PhysicsSystem& physicsSystem, BoundingBox& boundingBox) {
	for (const auto& entity : entities) {
		if (entity->isAffectedByGravity) {
			physicsSystem.applyGravity(*entity, deltaTime);
		}
	}
	updateEntities(deltaTime, boundingBox);
}

void EntityManager::drawEntities(void) {
	for (auto& entity : entities) {
		entity->draw();
	}
}