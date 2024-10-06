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

void EntityManager::drawEntities(float cameraX, float cameraY) {
	for (auto& entity : entities) {
		entity->draw(cameraX, cameraY);
	}
}

void EntityManager::updateMovementPatternEntities() {
	for (auto& entity : entities) {
		if (entity->hasMovementPattern) {
			entity->movementPattern.update(*entity);
		}
	}
}

bool EntityManager::checkCollisions(EntityManager& otherEntityManager) {
	for (auto& entity1 : entities) {
		for (auto& entity2 : otherEntityManager.getEntities()) {
			if (entity1 != entity2 && entity1->isColliding(*entity2)) {
				return true;
			}
		}
	}
	return false;
}


std::unordered_set<std::shared_ptr<Entity>> EntityManager::getEntities(void) {
	return entities;
}
