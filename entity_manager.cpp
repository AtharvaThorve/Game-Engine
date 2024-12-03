#include "entity_manager.hpp"

void EntityManager::addEntity(std::shared_ptr<Entity> entity) {
  std::lock_guard<std::mutex> lock(entityMutex);
  entities.insert(entity);
}

void EntityManager::removeEntity(std::shared_ptr<Entity> entity) {
  std::lock_guard<std::mutex> lock(entityMutex);
  entities.erase(entity);
}

void EntityManager::updateEntities(Timeline *timeline) {
  std::lock_guard<std::mutex> lock(entityMutex);
  for (auto &entity : entities) {
    Event move_event("move", timeline->getTime());
    move_event.parameters["entity"] = entity;
    EventManager &em = EventManager::getInstance();
    em.raise_event(move_event);
  }
}

void EntityManager::applyGravityOnEntities(PhysicsSystem &physicsSystem) {
  std::lock_guard<std::mutex> lock(entityMutex);
  for (auto &entity : entities) {
    if (entity->isAffectedByGravity) {
      physicsSystem.applyGravity(*entity);
    }
  }
}

void EntityManager::updateEntityDeltaTime() {
  std::lock_guard<std::mutex> lock(entityMutex);
  for (auto &entity : entities) {
    entity->updateDeltaTime();
  }
}

void EntityManager::drawEntities(float cameraX, float cameraY) {
  std::lock_guard<std::mutex> lock(entityMutex);
  for (auto &entity : entities) {
    entity->draw(cameraX, cameraY);
  }
  for (auto &deathZone : deathZones) {
    deathZone->draw(cameraX, cameraY);
  }
}

void EntityManager::updateMovementPatternEntities() {
  std::lock_guard<std::mutex> lock(entityMutex);
  for (auto &entity : entities) {
    if (entity->hasMovementPattern) {
      entity->movementPattern.update(*entity);
    }
  }
}

bool EntityManager::checkCollisions(EntityManager &otherEntityManager) {
  std::lock_guard<std::mutex> lock(entityMutex);
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

bool EntityManager::checkPlayerDeath(std::shared_ptr<Entity> player) {
  for (auto &deathZone : deathZones) {
    if (player->isColliding(*deathZone)) {
      return true;
    }
  }
  return false;
}

std::unordered_set<std::shared_ptr<Entity>> EntityManager::getEntities(void) {
  std::lock_guard<std::mutex> lock(entityMutex);
  return entities;
}
