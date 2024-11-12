#include "event_manager.hpp"

void EventManager::register_handler(const std::string &event_type,
                                    EventHandler *event_handler) {
  size_t event_type_hash = std::hash<std::string>{}(event_type);
  handlers[event_type_hash] = event_handler;
}

void EventManager::deregister_handler(const std::string &event_type,
                                      EventHandler *event_handler) {
  size_t event_type_hash = std::hash<std::string>{}(event_type);
  handlers.erase(event_type_hash);
}

void EventManager::raise_event(const Event &event) {
  event_queue.emplace(event);
}

void EventManager::process_events(int64_t current_timestamp) {
  while (!event_queue.empty() &&
         event_queue.top().timestamp <= current_timestamp) {
    Event event = event_queue.top();
    event_queue.pop();

    auto it = handlers.find(event.type);
    if (it != handlers.end()) {
      it->second->on_event(event);
    }
  }
}
