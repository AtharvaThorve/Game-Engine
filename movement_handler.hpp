#pragma once
#include "entity.hpp"
#include "event_manager.hpp"
#include "timeline.hpp"

class MovementHandler : public EventHandler {
public:
  MovementHandler(EventManager *em, Timeline *timeline);
  void on_event(const Event &event) override;

private:
    EventManager *em;
    Timeline *timeline;
    const size_t move_event_hash = std::hash<std::string>{}("move");
    void handle_movement(std::shared_ptr<Entity> entity);
};