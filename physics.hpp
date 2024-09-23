#pragma once
#include <SDL2/SDL.h>
#include "structs.hpp"
#include "Entity.hpp"

class PhysicsSystem {
public:
	Vector2 gravity;

	PhysicsSystem(float gravityX, float gravityY);

	void applyGravity(Entity& entity);
};