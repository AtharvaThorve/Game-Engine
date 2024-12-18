#pragma once
#include "entity.hpp"
#include "event_manager.hpp"
#include "timeline.hpp"

class DisconnectHandler : public EventHandler {
public:
  DisconnectHandler(Timeline *timeline);
  void on_event(const Event &event) override;

private:
  Timeline *timeline;
  const size_t disconnect_event_hash = std::hash<std::string>{}("disconnect");
};