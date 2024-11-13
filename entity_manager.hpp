#pragma once
#include "entity.hpp"
#include "physics.hpp"
#include "event_manager.hpp"
#include "timeline.hpp"
#include <memory>
#include <unordered_set>

class EntityManager {
public:
  void addEntity(std::shared_ptr<Entity> entity);

  template <typename... Args> void addEntities(Args &&...args) {
    addEntitiesHelper(std::forward<Args>(args)...);
  }

  void removeEntity(std::shared_ptr<Entity> entity);

  void updateEntities(Timeline *timeline);

  void applyGravityOnEntities(PhysicsSystem &physicsSystem);

  void updateEntityDeltaTime();

  void drawEntities(float cameraX, float cameraY);

  void updateMovementPatternEntities();

  bool checkCollisions(EntityManager &otherEntityManager);

  std::unordered_set<std::shared_ptr<Entity>> getEntities(void);

  void addDeathZone(std::shared_ptr<Entity> deathZone);
  template <typename... Args> void addDeathZones(Args &&...args) {
    addDeathZonesHelper(std::forward<Args>(args)...);
  }

  void addSpawnPoint(std::shared_ptr<Entity> spawnPoint);
  template <typename... Args> void addSpawnPoints(Args &&...args) {
    addSpawnPointsHelper(std::forward<Args>(args)...);
  }

  bool checkPlayerDeath(std::shared_ptr<Entity> player);

  void respawn(std::shared_ptr<Entity> player);

private:
  std::unordered_set<std::shared_ptr<Entity>> entities;
  std::unordered_set<std::shared_ptr<Entity>> deathZones;
  std::unordered_set<std::shared_ptr<Entity>> spawnPoints;

  void addEntitiesHelper() {}

  template <typename T, typename... Args>
  void addEntitiesHelper(T &&firstEntity, Args &&...restEntities) {
    addEntity(std::forward<T>(firstEntity));
    addEntitiesHelper(std::forward<Args>(restEntities)...);
  }

  void addDeathZonesHelper() {}
  template <typename T, typename... Args>
  void addDeathZonesHelper(T &&firstZone, Args &&...restZones) {
    addDeathZone(std::forward<T>(firstZone));
    addDeathZonesHelper(std::forward<Args>(restZones)...);
  }

  void addSpawnPointsHelper() {}
  template <typename T, typename... Args>
  void addSpawnPointsHelper(T &&firstPoint, Args &&...restPoints) {
    addSpawnPoint(std::forward<T>(firstPoint));
    addSpawnPointsHelper(std::forward<Args>(restPoints)...);
  }
};