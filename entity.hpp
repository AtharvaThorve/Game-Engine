#pragma once
#include "defs.hpp"
#include "movement_pattern.hpp"
#include "shape.hpp"
#include "structs.hpp"
#include "timeline.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include <mutex>

extern App *app;

class Entity {
public:
  Vector2 position = {0, 0};
  Vector2 dimensions = {0, 0};

  Vector2 velocity = {0, 0};
  Vector2 maxVelocity = {50, 50};

  Vector2 acceleration = {0, 0};
  Vector2 inputAcceleration = {0, 0};

  bool isAffectedByGravity = false;
  bool isMovable = false;
  bool isHittable = false;
  bool isDashing = false;

  std::unique_ptr<Shape> shape;
  SDL_Color color;

  bool hasMovementPattern = false;
  MovementPattern movementPattern;

  Timeline timeline;
  int64_t lastUpdateTime;
  int64_t lastGlobalTicSize;
  float deltaTime;

  // Static member variable to track the next unique ID
  static int64_t nextID;

  // Member variable to store this entity's unique ID
  int64_t id;

  // Platform value if entity is on top of a platform
  std::shared_ptr<Entity> standingPlatform;

  // Constructor
  Entity(const Vector2 &position, const Vector2 &dimensions,
         const SDL_Color &color, Timeline *anchor, int64_t tic = 1);
  Entity(const Vector2 &position, const SDL_Point &center, int radius,
         const SDL_Color &color, Timeline *anchor, int64_t tic = 1);

  // Destructor
  ~Entity() = default;

  // Move constructor and move assignment
  Entity(Entity &&) noexcept = default;
  Entity &operator=(Entity &&) noexcept = default;

  // Deleted copy constructor and assignment operator
  Entity(const Entity &) = delete;
  Entity &operator=(const Entity &) = delete;

  void updateDeltaTime();

  // Method to draw the entity
  void draw(float cameraX, float cameraY);

  // Method to check for collisions with another entity
  bool isColliding(const Entity &other) const;

  // Rescale the last update time when the global tic size changes
  void rescaleLastUpdateTime(int64_t oldGlobalTicSize,
                             int64_t newGlobalTicSize);

  // Getter for the entity's unique ID
  int64_t getID() const;

  // Clearing platform's reference
  void clearPlatformReference();

  Vector2 getPosition();
  void setPosition(const Vector2 &ps);

private:
  void updateSDLObject(float cameraX, float cameraY);

  std::mutex positionMutex;
};
