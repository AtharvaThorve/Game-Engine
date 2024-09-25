#pragma once
#include "entity.hpp"
#include "physics.hpp"
#include <unordered_set>
#include <memory>

class EntityManager {
public:

	void addEntity(std::shared_ptr<Entity> entity);
	template<typename... Args>
	void addEntities(Args&&... args) {
		addEntitiesHelper(std::forward<Args>(args)...);
	}
	void removeEntity(std::shared_ptr<Entity> entity);
	void updateEntities();
	void applyGravityOnEntities(PhysicsSystem& physicsSystem);
	void updateEntityDeltaTime();
	void drawEntities(void);
	void updateMovementPatternEntities();
	std::unordered_set<std::shared_ptr<Entity>> getEntities(void);

private:
	std::unordered_set<std::shared_ptr<Entity>> entities;

	void addEntitiesHelper() {}

	template<typename T, typename... Args>
	void addEntitiesHelper(T&& firstEntity, Args&&... restEntities) {
		addEntity(std::forward<T>(firstEntity));
		addEntitiesHelper(std::forward<Args>(restEntities)...);
	}

};