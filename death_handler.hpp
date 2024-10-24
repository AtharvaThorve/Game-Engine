#pragma once
#include "entity.hpp"
#include "event_manager.hpp"
#include "timeline.hpp"

class DeathHandler : public EventHandler {
public:
  DeathHandler(EventManager *em, Timeline *timeline);
  void on_event(const Event &event) override;

private:
  EventManager *em;
  Timeline *timeline;
  const size_t death_event_hash = std::hash<std::string>{}("death");
  void handle_death(std::shared_ptr<Entity> player);
};