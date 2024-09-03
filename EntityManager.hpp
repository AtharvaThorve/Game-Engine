#pragma once

#include "entity.hpp"
#include "physics.hpp"
#include "main.hpp"
#include <unordered_set>
#include <memory>
#include "BoundingBox.hpp"

class EntityManager {
public:

    void addEntity(std::shared_ptr<Entity> entity);
    void removeEntity(std::shared_ptr<Entity> entity);
    void updateEntities(float deltaTime, BoundingBox& boundingBox);
    void updateEntities(float deltaTime, PhysicsSystem& physicsSystem, BoundingBox& boundingBox);
    void drawEntities(void);

private:
    std::unordered_set<std::shared_ptr<Entity>> entities;
};