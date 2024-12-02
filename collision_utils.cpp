#include "collision_utils.hpp"

namespace collision_utils {
std::string checkCollisionDirection(std::shared_ptr<Entity> entityA,
                                    std::shared_ptr<Entity> entityB) {
  // Use the isColliding function to check for a collision first
  if (entityA->isColliding(*entityB)) {
    RectangleShape *rectA =
        dynamic_cast<RectangleShape *>(entityA->shape.get());
    RectangleShape *rectB =
        dynamic_cast<RectangleShape *>(entityB->shape.get());

    if (rectA && rectB) {
      // Calculate the distances from the edges of rectA to rectB
      int deltaLeft = (rectB->rect.x + rectB->rect.w) -
                      rectA->rect.x; // A's left side vs B's right side
      int deltaRight = (rectA->rect.x + rectA->rect.w) -
                       rectB->rect.x; // A's right side vs B's left side
      int deltaTop = (rectB->rect.y + rectB->rect.h) -
                     rectA->rect.y; // A's top side vs B's bottom side
      int deltaBottom = (rectA->rect.y + rectA->rect.h) -
                        rectB->rect.y; // A's bottom side vs B's top side

      // Find the minimum overlap distance to determine collision direction
      int minDeltaX = std::min(deltaLeft, deltaRight);
      int minDeltaY = std::min(deltaTop, deltaBottom);

      // Collision direction determination based on the minimum overlap
      if (minDeltaX < minDeltaY) {
        // Collision is more horizontal
        return (deltaLeft < deltaRight) ? "left" : "right";
      } else {
        // Collision is more vertical
        return (deltaTop < deltaBottom) ? "up" : "down";
      }
    }
  }

  // Default case: no collision or unsupported shapes
  return "none";
}

void handlePlatformCollision(std::shared_ptr<Entity> entityA,
                             std::shared_ptr<Entity> entityB) {
  std::string direction = checkCollisionDirection(entityA, entityB);
  RectangleShape *rectA = dynamic_cast<RectangleShape *>(entityA->shape.get());
  RectangleShape *rectB = dynamic_cast<RectangleShape *>(entityB->shape.get());

  // std::cout << direction << std::endl;

  if (direction == "up") {
    entityA->position.y = entityB->position.y + rectB->rect.h;
    if (entityA->velocity.y < 0)
      entityA->velocity.y = 0;
  } else if (direction == "down") {
    entityA->position.y = entityB->position.y - rectA->rect.h + 1;
    if (!entityA->standingPlatform)
      entityA->velocity.x = 0;

    entityA->standingPlatform = entityB;
    if (entityA->velocity.y > 0)
      entityA->velocity.y = 0;
  } else if (direction == "left") {
    entityA->position.x = entityB->position.x + rectB->rect.w;
    if (entityA->velocity.x < 0)
      entityA->velocity.x = 0;
  } else if (direction == "right") {
    entityA->position.x = entityB->position.x - rectA->rect.w;
    if (entityA->velocity.x > 0)
      entityA->velocity.x = 0;
  }
}

void handleDeathZoneCollision(std::shared_ptr<Entity> entityA,
                              std::shared_ptr<Entity> entityB) {
  Event death_event("death", globalTimeline.getTime() + 1);
  death_event.parameters["player"] = entityA;
  EventManager &em = EventManager::getInstance();
  em.raise_event(death_event);
}

void handlePlayerBulletAlienCollision(std::shared_ptr<Entity> bullet,
                                      std::shared_ptr<Entity> alien) {
  alien->isHittable = false;
  bullet->isHittable = false;
}

void handleAlienBulletPlayerCollision(std::shared_ptr<Entity> bullet,
                                      std::shared_ptr<Entity> player) {
  bullet->isHittable = false;
  Event death_event("death", globalTimeline.getTime() + 1);
  death_event.parameters["player"] = player;
  EventManager &em = EventManager::getInstance();
  em.raise_event(death_event);
}

void handlePlayerWallCollision(std::shared_ptr<Entity> player, int worldWidth) {
  Vector2 playerPos = player->getPosition();
  if (playerPos.x < 0) {
    playerPos.x = 0;
  } else if (playerPos.x + player->dimensions.x >= worldWidth) {
    playerPos.x = worldWidth - player->dimensions.x;
  }
}

} // namespace collision_utils
