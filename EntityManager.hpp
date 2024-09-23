#pragma once
#include "entity.hpp"
#include "physics.hpp"
#include "main.hpp"
#include <unordered_set>
#include <memory>

class EntityManager {
public:

	void addEntity(std::shared_ptr<Entity> entity);
	void removeEntity(std::shared_ptr<Entity> entity);
	void updateEntities();
	void applyGravityOnEntities(PhysicsSystem& physicsSystem);
	void updateEntityDeltaTime();
	void drawEntities(void);
	void updateMovementPatternEntities();
	std::unordered_set<std::shared_ptr<Entity>> getEntities(void);

private:
	std::unordered_set<std::shared_ptr<Entity>> entities;
};