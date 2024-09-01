#pragma once
#include <vector>
#include <memory>
#include "structs.hpp"
#include "physics.hpp"
#include "Rectangle.hpp"

extern App* app;

Entity* createRectEntity(Vector2 position, Vector2 velocity, SDL_Color color, float mass = 1.0f, int height = 50, int width = 50, bool isMovable = false, bool isAffectedByGravity = true, bool isHittable = true);

class EntityManager {
public:
	std::vector<Entity*> entities;

	void addEntity(Entity* entity);
	void updateEntities(float deltaTime);
	void renderEntities(void);
	void cleanUp(void);
};