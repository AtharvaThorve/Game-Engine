#include "entity_manager.hpp"

void EntityManager::addEntity(std::shared_ptr<Entity> entity) {
  entities.insert(entity);
}

void EntityManager::removeEntity(std::shared_ptr<Entity> entity) {
  entities.erase(entity);
}

void EntityManager::updateEntities() {
  for (auto &entity : entities) {
    entity->updatePosition();
  }
}

void EntityManager::applyGravityOnEntities(PhysicsSystem &physicsSystem) {
  for (auto &entity : entities) {
    if (entity->isAffectedByGravity) {
      physicsSystem.applyGravity(*entity);
    }
  }
}

void EntityManager::updateEntityDeltaTime() {
  for (auto &entity : entities) {
    entity->updateDeltaTime();
  }
}

void EntityManager::drawEntities(float cameraX, float cameraY) {
  for (auto &entity : entities) {
    entity->draw(cameraX, cameraY);
  }
  for (auto &deathZone : deathZones) {
    deathZone->draw(cameraX, cameraY);
  }
}

void EntityManager::updateMovementPatternEntities() {
  for (auto &entity : entities) {
    if (entity->hasMovementPattern) {
      entity->movementPattern.update(*entity);
    }
  }
}

bool EntityManager::checkCollisions(EntityManager &otherEntityManager) {
  for (auto &entity1 : entities) {
    for (auto &entity2 : otherEntityManager.getEntities()) {
      if (entity1 != entity2 && entity1->isColliding(*entity2)) {
        return true;
      }
    }
  }
  return false;
}
void EntityManager::addDeathZone(std::shared_ptr<Entity> deathZone) {
  deathZones.insert(deathZone);
}

void EntityManager::addSpawnPoint(std::shared_ptr<Entity> spawnPoint) {
  spawnPoints.insert(spawnPoint);
}

bool EntityManager::checkPlayerDeath(std::shared_ptr<Entity> player) {
  for (auto &deathZone : deathZones) {
    if (player->isColliding(*deathZone)) {
      return true;
    }
  }
  return false;
}

void EntityManager::respawn(std::shared_ptr<Entity> player) {
  float playerX = player->position.x;
  float playerY = player->position.y;
  std::shared_ptr<Entity> closestSpawn = nullptr;
  float minDistance = std::numeric_limits<float>::max();

  for (auto &spawnPoint : spawnPoints) {
    if (spawnPoint->position.x < playerX) {
      float distance = abs(playerX - spawnPoint->position.x) +
                       abs(playerY - spawnPoint->position.y);
      if (distance < minDistance) {
        minDistance = distance;
        closestSpawn = spawnPoint;
      }
    }
  }

  if (closestSpawn) {
    player->position = closestSpawn->position;
    player->velocity = {0, 0};
  }
}
std::unordered_set<std::shared_ptr<Entity>> EntityManager::getEntities(void) {
  return entities;
}
