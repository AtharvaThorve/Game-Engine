#pragma once
#include "event.hpp"
#include "structs.hpp"
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>

class EventHandler {
public:
  virtual void on_event(const Event &event) = 0;
};

class EventManager {
private:
  std::unordered_map<size_t, EventHandler *> handlers;

  std::priority_queue<Event, std::vector<Event>, EventCompare> event_queue;

public:
  void register_handler(const std::string &event_type,
                        EventHandler *event_handler);

  void deregister_handler(const std::string &event_type,
                          EventHandler *event_handler);

  void raise_event(const Event &event);

  void process_events(int64_t current_timestamp);
};