#pragma once
#include "entity.hpp"
#include "event_manager.hpp"
#include "main.hpp"
#include "timeline.hpp"

class BulletHandler : public EventHandler {
public:
  BulletHandler(Timeline *timeline);
  void on_event(const Event &event) override;

private:
  Timeline *timeline;
  const size_t bullet_event_hash = std::hash<std::string>{}("bullet");
  void handle_bullet_creation(std::shared_ptr<Entity> entity);
};