#pragma once
#include "Entity.hpp"
#include "structs.hpp"

class Camera
{
public:
	Camera(int w, int h);
	void update(const Entity& entity, int worldWidth, int worldHeight);
	Vector2 position;
	int width;
	int height;
	void clampToWorld(int worldWidth, int worldHeight);
};
