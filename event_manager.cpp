#include "event_manager.hpp"

void EventManager::register_handler(const std::string &event_type,
                                    EventHandler *event_handler) {
  size_t event_type_hash = std::hash<std::string>{}(event_type);
  handlers[event_type_hash] = event_handler;
}

void EventManager::register_wildcard_handler(EventHandler *event_handler) {
  wildcard_handlers.insert(event_handler);
}

void EventManager::deregister_handler(const std::string &event_type,
                                      EventHandler *event_handler) {
  size_t event_type_hash = std::hash<std::string>{}(event_type);
  handlers.erase(event_type_hash);
}

void EventManager::raise_event(const Event &event) {
  event_queue.emplace(event);
  if (replay_only_mode &&
      event.parameters.find("is_replay_event") != event.parameters.end()) {
    replay_events_count++;
  }
}

void EventManager::set_replay_only_mode(bool mode) {
  replay_only_mode = mode;
  if (mode) {
    replay_events_count = 0;
  }
}

void EventManager::process_events(int64_t current_timestamp) {
  while (!event_queue.empty() &&
         event_queue.top().timestamp <= current_timestamp) {
    Event event = event_queue.top();
    event_queue.pop();

    if (replay_only_mode &&
        event.parameters.find("is_replay_event") == event.parameters.end())
      continue;

    auto it = handlers.find(event.type);
    if (it != handlers.end()) {
      it->second->on_event(event);
    }

    for (auto *handler : wildcard_handlers) {
      handler->on_event(event);
    }

    if (replay_only_mode &&
        event.parameters.find("is_replay_event") != event.parameters.end()) {
      replay_events_count--;

      if (replay_events_count == 0) {
        replay_only_mode = false;
      }
    }
  }
}
