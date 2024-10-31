#pragma once
#include "entity.hpp"
#include "event_manager.hpp"
#include "timeline.hpp"

class InputHandler : public EventHandler {
public:
  InputHandler(EventManager *em, Timeline *timeline);
  void on_event(const Event &event) override;

private:
  EventManager *em;
  Timeline *timeline;

  const size_t input_event_hash = std::hash<std::string>{}("input");
  const size_t move_x = std::hash<std::string>{}("move_x");
  const size_t move_y = std::hash<std::string>{}("move_y");
  const size_t stop_x = std::hash<std::string>{}("stop_x");
  const size_t stop_y = std::hash<std::string>{}("stop_y");
  const size_t jump = std::hash<std::string>{}("jump");
  const size_t dash = std::hash<std::string>{}("dash");

  void handle_input(std::shared_ptr<Entity> player, size_t input_type,
                    float acceleration_rate = 5, float jump_force = -150,
                    Vector2 dash_vector = {50, 50});
};