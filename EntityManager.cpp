#include "EntityManager.hpp"

void EntityManager::addEntity(std::shared_ptr<Entity> entity) {
	entities.insert(entity);
}

void EntityManager::removeEntity(std::shared_ptr<Entity> entity) {
	entities.erase(entity);
}

void EntityManager::updateEntities() {
	for (auto& entity : entities) {
		entity->updatePosition();
	}
}

void EntityManager::applyGravityOnEntities(PhysicsSystem& physicsSystem) {
	for (auto& entity : entities) {
		if (entity->isAffectedByGravity) {
			physicsSystem.applyGravity(*entity);
		}
	}
}

void EntityManager::updateEntityDeltaTime() {
	for (auto& entity : entities) {
		entity->updateDeltaTime();
	}
}

void EntityManager::drawEntities(void) {
	for (auto& entity : entities) {
		entity->draw();
	}
}

void EntityManager::updateMovementPatternEntities() {
	for (auto& entity : entities) {
		if (entity->hasMovementPattern) {
			entity->movementPattern.update(*entity);
		}
	}
}

std::unordered_set<std::shared_ptr<Entity>> EntityManager::getEntities(void) {
	return entities;
}
