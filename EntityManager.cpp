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
	    for (auto& deathZone : deathZones) {
        deathZone->draw(cameraX, cameraY);
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
std::vector<std::shared_ptr<Entity>> spawnPoints;

void EntityManager::addDeathZone(std::shared_ptr<Entity> deathZone) {
    deathZones.insert(deathZone);
}

void EntityManager::addSpawnPoint(std::shared_ptr<Entity> spawnPoint) {
    spawnPoints.push_back(spawnPoint);
}

std::vector<std::shared_ptr<Entity>> EntityManager::getSpawnPoints() const {
    return spawnPoints;
}
bool EntityManager::checkPlayerDeathAndRespawn(std::shared_ptr<Entity> player) {
    for (auto& deathZone : deathZones) {
        if (player->isColliding(*deathZone)) {
            float playerX = player->position.x;
            std::shared_ptr<Entity> closestSpawn = nullptr;
            float minDistance = std::numeric_limits<float>::max();

            for (auto& spawnPoint : spawnPoints) {
                if (spawnPoint->position.x < playerX) {
                    float distance = playerX - spawnPoint->position.x;
                    if (distance < minDistance) {
                        minDistance = distance;
                        closestSpawn = spawnPoint;
                    }
                }
            }

            // If we found a valid spawn point, respawn the player
            if (closestSpawn) {
                player->position = closestSpawn->position;
                player->velocity = {0, 0}; // Reset velocity upon respawn
                return true; // Player died and was respawned
            }
        }
    }

    return false; // Player did not die
}
std::unordered_set<std::shared_ptr<Entity>> EntityManager::getEntities(void) {
	return entities;
}
