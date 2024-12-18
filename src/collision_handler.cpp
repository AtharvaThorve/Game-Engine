#include "collision_handler.hpp"

CollisionHandler::CollisionHandler(Timeline *timeline) : timeline(timeline) {}

void CollisionHandler::register_collision_handler(
    const std::string &collision_type, CollisionFunction collision_function) {
  collision_handlers[collision_type] = collision_function;
}

void CollisionHandler::on_event(const Event &event) {
  if (event.type == collision_event_hash) {
    std::shared_ptr<Entity> entity1 =
        std::get<std::shared_ptr<Entity>>(event.parameters.at("entity1"));
    std::shared_ptr<Entity> entity2 =
        std::get<std::shared_ptr<Entity>>(event.parameters.at("entity2"));
    std::string collision_type =
        std::get<std::string>(event.parameters.at("collision_type"));

    handle_collision(entity1, entity2, collision_type);
  }
}

void CollisionHandler::handle_collision(std::shared_ptr<Entity> entity1,
                                        std::shared_ptr<Entity> entity2,
                                        std::string collision_type) {
  auto it = collision_handlers.find(collision_type);
  if (it != collision_handlers.end()) {
    it->second(entity1, entity2);
  } else {
    std::cerr << "No collision handler for collision type: " << collision_type
              << std::endl;
  }
}
