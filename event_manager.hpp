#pragma once
#include "event.hpp"
#include "structs.hpp"
#include <iostream>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class EventHandler {
public:
  virtual void on_event(const Event &event) = 0;
};

class EventManager {
private:
  EventManager() {}
  
  std::unordered_map<size_t, EventHandler *> handlers;
  std::unordered_set<EventHandler *> wildcard_handlers;

  std::priority_queue<Event, std::vector<Event>, EventCompare> event_queue;
  bool replay_only_mode = 0;
  int64_t replay_events_count = 0;

  std::mutex event_mutex;

public:
  static EventManager &getInstance() {
    static EventManager instance;
    return instance;
  }

  EventManager(const EventManager &) = delete;
  EventManager &operator=(const EventManager &) = delete;

  void register_handler(const std::string &event_type,
                        EventHandler *event_handler);

  void register_wildcard_handler(EventHandler *event_handler);

  void deregister_handler(const std::string &event_type,
                          EventHandler *event_handler);

  void raise_event(const Event &event);

  void process_events(int64_t current_timestamp);

  void set_replay_only_mode(bool mode);
};