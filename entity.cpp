#include "entity.hpp"

// Initialize the static member variable.
int64_t Entity::nextID = 0; // Start the unique ID from 0 or any other value.

Entity::Entity(const Vector2 &position, const Vector2 &dimensions,
               const SDL_Color &color, Timeline *anchor, int64_t tic)
    : position(position), shape(nullptr), color(color), timeline(anchor, tic),
      lastUpdateTime(timeline.getTime()),
      lastGlobalTicSize(timeline.getAnchorTic()),
      id(nextID++) // Assign unique ID
{
  SDL_Rect rect = {static_cast<int>(position.x), static_cast<int>(position.y),
                   static_cast<int>(dimensions.x),
                   static_cast<int>(dimensions.y)};
  shape = std::make_unique<RectangleShape>(rect);
}

Entity::Entity(const Vector2 &position, const SDL_Point &center, int radius,
               const SDL_Color &color, Timeline *anchor, int64_t tic)
    : position(position), shape(nullptr), color(color), timeline(anchor, tic),
      lastUpdateTime(timeline.getTime()),
      lastGlobalTicSize(timeline.getAnchorTic()),
      id(nextID++) // Assign unique ID
{
  shape = std::make_unique<CircleShape>(radius, center);
}

void Entity::updateDeltaTime() {
  int64_t currentGlobalTicSize = timeline.getAnchorTic();
  if (currentGlobalTicSize != lastGlobalTicSize) {
    rescaleLastUpdateTime(lastGlobalTicSize, currentGlobalTicSize);
    lastGlobalTicSize = currentGlobalTicSize;
  }
  int64_t currentTime = timeline.getTime();
  deltaTime = (currentTime - lastUpdateTime) /
              NANOSECONDS_TO_SECONDS; // Nanoseconds to seconds
  lastUpdateTime = currentTime;
}

void Entity::updatePosition() {
  if (isMovable) {
    Vector2 finalAcceleration = acceleration;

    finalAcceleration.x += inputAcceleration.x;
    finalAcceleration.y += inputAcceleration.y;

    velocity.x += finalAcceleration.x * deltaTime;
    velocity.y += finalAcceleration.y * deltaTime;

    if (!isDashing) {
      velocity.x =
          std::max(std::min(velocity.x, maxVelocity.x), -maxVelocity.x);
      velocity.y =
          std::max(std::min(velocity.y, maxVelocity.y), -maxVelocity.y);
    }

    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;

    if (standingPlatform) {
      Vector2 platformDelta;
      platformDelta.x =
          standingPlatform->velocity.x * standingPlatform->deltaTime;
      platformDelta.y =
          standingPlatform->velocity.y * standingPlatform->deltaTime;

      // Move the entity with the platform
      position.x += platformDelta.x;
      position.y += platformDelta.y;
    }
  }
}

void Entity::draw(float cameraX, float cameraY) {
  if (shape) {
    updateSDLObject(cameraX, cameraY);
    shape->draw(color);
  }
}

bool Entity::isColliding(const Entity &other) const {
  if (shape && other.shape) {
    return shape->isColliding(*other.shape);
  }
  return false;
}

void Entity::rescaleLastUpdateTime(int64_t oldGlobalTicSize,
                                   int64_t newGlobalTicSize) {
  if (oldGlobalTicSize != newGlobalTicSize) {
    double scaleFactor =
        static_cast<double>(oldGlobalTicSize) / newGlobalTicSize;
    lastUpdateTime =
        lastUpdateTime * timeline.getTic() + timeline.getStartTime();
    lastUpdateTime = static_cast<int64_t>(lastUpdateTime * scaleFactor);
    lastUpdateTime -= timeline.getStartTime();
    lastUpdateTime /= timeline.getTic();
  }
}

// Add this getter to retrieve the unique ID.
int64_t Entity::getID() const { return id; }

void Entity::updateSDLObject(float cameraX, float cameraY) {
  if (shape->type == ShapeType::RECTANGLE) {
    RectangleShape *rectShape = dynamic_cast<RectangleShape *>(shape.get());
    if (rectShape) {
      rectShape->rect.x = static_cast<int>(position.x - cameraX);
      rectShape->rect.y = static_cast<int>(position.y - cameraY);
    }
  } else if (shape->type == ShapeType::CIRCLE) {
    CircleShape *circleShape = dynamic_cast<CircleShape *>(shape.get());
    if (circleShape) {
      circleShape->center.x = static_cast<int>(position.x - cameraX);
      circleShape->center.y = static_cast<int>(position.y - cameraY);
    }
  }
}

void Entity::clearPlatformReference() { standingPlatform = nullptr; }