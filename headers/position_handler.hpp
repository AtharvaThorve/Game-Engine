#pragma once
#include "entity.hpp"
#include "event_manager.hpp"
#include "timeline.hpp"

class PositionHandler : public EventHandler {
public:
  PositionHandler(Timeline *timeline);
  void on_event(const Event &event) override;

private:
  Timeline *timeline;
  const size_t position_event_hash =
      std::hash<std::string>{}("update_position");
};