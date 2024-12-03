#include "entity.hpp"

// Initialize the static member variable.
int64_t Entity::nextID = 0; // Start the unique ID from 0 or any other value.

Entity::Entity(const Vector2 &position, const Vector2 &dimensions,
               const SDL_Color &color, Timeline *anchor, int64_t tic)
    : position(position), dimensions(dimensions), shape(nullptr), color(color),
      timeline(anchor, tic), lastUpdateTime(timeline.getTime()),
      lastGlobalTicSize(timeline.getAnchorTic()),
      id(nextID++) // Assign unique ID
{
  SDL_Rect rect = {static_cast<int>(position.x), static_cast<int>(position.y),
                   static_cast<int>(dimensions.x),
                   static_cast<int>(dimensions.y)};
  shape = std::make_unique<RectangleShape>(rect);
  isDrawable = true;
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

Vector2 Entity::getPosition() {
  std::unique_lock<std::mutex> lock(positionMutex);
  return position;
}

void Entity::setPosition(const Vector2 &ps) {
  std::unique_lock<std::mutex> lock(positionMutex);
  position = ps;
}

std::unordered_map<std::string, std::string> Entity::serialize() const {
  std::unordered_map<std::string, std::string> state;

  state["position_x"] = std::to_string(position.x);
  state["position_y"] = std::to_string(position.y);
  state["dimensions_x"] = std::to_string(dimensions.x);
  state["dimensions_y"] = std::to_string(dimensions.y);

  state["velocity_x"] = std::to_string(velocity.x);
  state["velocity_y"] = std::to_string(velocity.y);
  state["maxVelocity_x"] = std::to_string(maxVelocity.x);
  state["maxVelocity_y"] = std::to_string(maxVelocity.y);

  state["acceleration_x"] = std::to_string(acceleration.x);
  state["acceleration_y"] = std::to_string(acceleration.y);
  state["inputAcceleration_x"] = std::to_string(inputAcceleration.x);
  state["inputAcceleration_y"] = std::to_string(inputAcceleration.y);

  state["isAffectedByGravity"] = std::to_string(isAffectedByGravity);
  state["isMovable"] = std::to_string(isMovable);
  state["isHittable"] = std::to_string(isHittable);
  state["isDashing"] = std::to_string(isDashing);
  state["isDrawable"] = std::to_string(isDrawable);

  state["id"] = std::to_string(id);

  state["color_r"] = std::to_string(color.r);
  state["color_g"] = std::to_string(color.g);
  state["color_b"] = std::to_string(color.b);
  state["color_a"] = std::to_string(color.a);

  return state;
}

void Entity::deserialize(const std::unordered_map<std::string, std::string> &state) {
  position.x = std::stof(state.at("position_x"));
  position.y = std::stof(state.at("position_y"));
  dimensions.x = std::stof(state.at("dimensions_x"));
  dimensions.y = std::stof(state.at("dimensions_y"));

  velocity.x = std::stof(state.at("velocity_x"));
  velocity.y = std::stof(state.at("velocity_y"));
  maxVelocity.x = std::stof(state.at("maxVelocity_x"));
  maxVelocity.y = std::stof(state.at("maxVelocity_y"));

  acceleration.x = std::stof(state.at("acceleration_x"));
  acceleration.y = std::stof(state.at("acceleration_y"));
  inputAcceleration.x = std::stof(state.at("inputAcceleration_x"));
  inputAcceleration.y = std::stof(state.at("inputAcceleration_y"));

  isAffectedByGravity =
      static_cast<bool>(std::stoi(state.at("isAffectedByGravity")));
  isMovable = static_cast<bool>(std::stoi(state.at("isMovable")));
  isHittable = static_cast<bool>(std::stoi(state.at("isHittable")));
  isDashing = static_cast<bool>(std::stoi(state.at("isDashing")));
  isDrawable = static_cast<bool>(std::stoi(state.at("isDrawable")));

  id = std::stoll(state.at("id"));

  color.r = static_cast<Uint8>(std::stoi(state.at("color_r")));
  color.g = static_cast<Uint8>(std::stoi(state.at("color_g")));
  color.b = static_cast<Uint8>(std::stoi(state.at("color_b")));
  color.a = static_cast<Uint8>(std::stoi(state.at("color_a")));
}