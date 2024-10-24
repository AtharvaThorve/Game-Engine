#pragma once
#include "collision_utils.hpp"
#include "entity.hpp"
#include "event_manager.hpp"
#include "timeline.hpp"

class CollisionHandler : public EventHandler {
public:
  CollisionHandler(EventManager *em, Timeline *timeline);
  void on_event(const Event &event) override;

private:
  EventManager *em;
  Timeline *timeline;
  const size_t collision_event_hash = std::hash<std::string>{}("collision");
  const size_t platform_collision_hash = std::hash<std::string>{}("platform");
  const size_t death_zone_collision_hash =
      std::hash<std::string>{}("death_zone");
  void handle_collision(std::shared_ptr<Entity> entity1,
                        std::shared_ptr<Entity> entity2, size_t collision_type);
};