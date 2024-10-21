#include "event_manager.hpp"

void EventManager::register_handler(const std::string &event_type,
                                    EventHandler *event_handler) {
  handlers[event_type] = event_handler;
}

void EventManager::deregister_handler(const std::string &event_type,
                                      EventHandler *event_handler) {
  handlers.erase(event_type);
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
