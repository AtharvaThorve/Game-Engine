#pragma once
#include "entity.hpp"
#include "event_manager.hpp"
#include "main.hpp"
#include "timeline.hpp"

class DeathHandler : public EventHandler {
public:
  DeathHandler(Timeline *timeline);
  void on_event(const Event &event) override;

private:
  Timeline *timeline;
  const size_t death_event_hash = std::hash<std::string>{}("death");
  void handle_death(std::shared_ptr<Entity> player);
};