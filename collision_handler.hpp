#pragma once
#include "collision_utils.hpp"
#include "entity.hpp"
#include "event_manager.hpp"
#include "timeline.hpp"
#include <functional>
#include <unordered_map>

using CollisionFunction =
    std::function<void(std::shared_ptr<Entity>, std::shared_ptr<Entity>)>;

class CollisionHandler : public EventHandler {
public:
  CollisionHandler(Timeline *timeline);
  void on_event(const Event &event) override;
  void register_collision_handler(const std::string &collision_type,
                                  CollisionFunction collision_function);

private:
  Timeline *timeline;

  const size_t collision_event_hash = std::hash<std::string>{}("collision");

  std::unordered_map<std::string, CollisionFunction> collision_handlers;

  void handle_collision(std::shared_ptr<Entity> entity1,
                        std::shared_ptr<Entity> entity2,
                        std::string collision_type);
};