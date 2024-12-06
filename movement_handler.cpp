#include "movement_handler.hpp"

MovementHandler::MovementHandler(Timeline *timeline) : timeline(timeline) {}

void MovementHandler::on_event(const Event &event) {
  if (event.type == move_event_hash) {
    std::shared_ptr<Entity> entity =
        std::get<std::shared_ptr<Entity>>(event.parameters.at("entity"));
    handle_movement(entity);
  }
}

void MovementHandler::handle_movement(std::shared_ptr<Entity> entity) {
  if (entity->isMovable) {
    Vector2 finalAcceleration = entity->acceleration;

    finalAcceleration.x += entity->inputAcceleration.x;
    finalAcceleration.y += entity->inputAcceleration.y;

    entity->velocity.x += finalAcceleration.x * entity->deltaTime;
    entity->velocity.y += finalAcceleration.y * entity->deltaTime;

    if (!entity->isDashing) {
      entity->velocity.x =
          std::max(std::min(entity->velocity.x, entity->maxVelocity.x),
                   -entity->maxVelocity.x);
      entity->velocity.y =
          std::max(std::min(entity->velocity.y, entity->maxVelocity.y),
                   -entity->maxVelocity.y);
    }

    Vector2 position = entity->getPosition();
    position.x += entity->velocity.x * entity->deltaTime;
    position.y += entity->velocity.y * entity->deltaTime;

    entity->setPosition(position);

    if (entity->standingPlatform) {
      Vector2 platformDelta;
      platformDelta.x = entity->standingPlatform->velocity.x *
                        entity->standingPlatform->deltaTime;
      platformDelta.y = entity->standingPlatform->velocity.y *
                        entity->standingPlatform->deltaTime;

      Vector2 position = entity->getPosition();
      // Move the entity with the platform
      position.x += platformDelta.x;
      position.y += platformDelta.y;

      entity->setPosition(position);
    }
  }
}
