#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

class Entity;

class Event {
public:
  // Defines the type of the event
  size_t type;
  // Maps the event type to the parameters it requires.
  std::unordered_map<std::string, std::variant<int, float, size_t, std::string,
                                               std::shared_ptr<Entity>>>
      parameters;

  int64_t timestamp;

  Event(const std::string &event_type, int64_t event_timestamp = 0)
      : type(std::hash<std::string>{}(event_type)), timestamp(event_timestamp) {
  }
};