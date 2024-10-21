#pragma once
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

class Entity;

class Event {
public:
  // Defines the type of the event
  std::string type;
  // Maps the event type to the parameters it requires.
  std::unordered_map<std::string,
                     std::variant<int, float, std::string, Entity *>>
      parameters;

  int64_t timestamp;

  Event(const std::string &event_type, int event_timestamp = 0)
      : type(event_type), timestamp(event_timestamp) {}
};