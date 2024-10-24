#include "collision_handler.hpp"

CollisionHandler::CollisionHandler(EventManager *em, Timeline *timeline)
    : em(em), timeline(timeline) {}

void CollisionHandler::on_event(const Event &event) {
  if (event.type == collision_event_hash) {
    std::shared_ptr<Entity> entity1 =
        std::get<std::shared_ptr<Entity>>(event.parameters.at("entity1"));
    std::shared_ptr<Entity> entity2 =
        std::get<std::shared_ptr<Entity>>(event.parameters.at("entity2"));
    size_t collision_type =
        std::get<size_t>(event.parameters.at("collision_type"));

    handle_collision(entity1, entity2, collision_type);
  }
}

void CollisionHandler::handle_collision(std::shared_ptr<Entity> entity1,
                                        std::shared_ptr<Entity> entity2,
                                        size_t collision_type) {
  if (collision_type == platform_collision_hash) {
    collision_utils::handlePlatformCollision(entity1, entity2);
  } else if (collision_type == death_zone_collision_hash) {
    Event death_event("death", timeline->getTime() + 1);
    death_event.parameters["player"] = entity1;
    em->raise_event(death_event);
  }
}
