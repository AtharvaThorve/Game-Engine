#include "EntityManager.hpp"

void EntityManager::addEntity(std::shared_ptr<Entity> entity) {
	entities.insert(entity);
}

void EntityManager::removeEntity(std::shared_ptr<Entity> entity) {
	entities.erase(entity);
}

void EntityManager::updateEntities(float deltaTime) {
	for (auto& entity : entities) {
		entity->updateDeltaTime();
		if (entity->hasMovementPattern) {
			entity->movementPattern.update(*entity);
		}
		entity->updatePosition();
	}
}

void EntityManager::updateEntities(float deltaTime, PhysicsSystem& physicsSystem) {
	for (auto& entity : entities) {
		if (entity->isAffectedByGravity) {
			physicsSystem.applyGravity(*entity, deltaTime);
		}
	}
	updateEntities(deltaTime);
}

void EntityManager::drawEntities(void) {
	for (auto& entity : entities) {
		entity->draw();
	}
}

std::unordered_set<std::shared_ptr<Entity>> EntityManager::getEntities(void) {
	return entities;
}
