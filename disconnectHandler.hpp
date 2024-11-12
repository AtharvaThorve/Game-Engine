#pragma once
#include "entity.hpp"
#include "event_manager.hpp"
#include "timeline.hpp"

class DisconnectHandler : public EventHandler {
public:
  DisconnectHandler(EventManager *em, Timeline *timeline);
  void on_event(const Event &event) override;

private:
  EventManager *em;
  Timeline *timeline;
  const size_t disconnect_event_hash = std::hash<std::string>{}("disconnect");
};