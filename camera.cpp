#include "camera.hpp"

Camera::Camera(int w, int h) : position({0, 0}), width(w), height(h) {}

void Camera::clampToWorld(int worldWidth, int worldHeight) {
  if (position.x < 0)
    position.x = 0;
  if (position.y < 0)
    position.y = 0;
  if (position.x + width > worldWidth)
    position.x = worldWidth - width;
  if (position.y + height > worldHeight)
    position.y = worldHeight - height;
}

void Camera::update(const Entity &entity, int worldWidth, int worldHeight) {
  // Todo: Change value 50 to the entities dimensions.
  position.x = entity.position.x + 50 / 2 - width / 2;
  position.y = entity.position.y + 50 / 2 - height / 2;
  clampToWorld(worldWidth, worldHeight);
}
