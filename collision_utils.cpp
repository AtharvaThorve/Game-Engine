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

void handleBallPaddleCollision(std::shared_ptr<Entity> ball,
                               std::shared_ptr<Entity> paddle) {
  ball->velocity.y = -std::abs(ball->velocity.y);
  float deltaX = ball->position.x - paddle->position.x;
  ball->velocity.x += deltaX * 5 * (ball->velocity.x >= 0 ? 1 : -1);
}

void handleBallBrickCollision(std::shared_ptr<Entity> ball,
                              std::shared_ptr<Entity> brick) {
  std::string dir = checkCollisionDirection(ball, brick);
  if (dir == "up" || dir == "down")
    ball->velocity.y = -ball->velocity.y;
  else if (dir == "left" || dir == "right")
    ball->velocity.x = -ball->velocity.x;
    
  brick->isHittable = false;
  brick->isDrawable = false;
}

void handleBallWallCollision(std::shared_ptr<Entity> ball, int screenWidth,
                             int screenHeight) {
  if (ball->position.x <= 0 ||
      ball->position.x + ball->dimensions.x >= screenWidth) {
    ball->velocity.x = -ball->velocity.x;
  }
  if (ball->position.y <= 0) {
    ball->velocity.y = -ball->velocity.y;
  }
}

} // namespace collision_utils
