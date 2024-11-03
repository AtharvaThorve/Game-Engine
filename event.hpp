#pragma once
#include "structs.hpp"
#include <memory>
#include <unordered_set>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

class Entity;

using VariantType = std::variant<
    int, float, size_t, std::string, Vector2, std::shared_ptr<Entity>,
    std::shared_ptr<std::unordered_map<
        std::string, std::unordered_map<int, std::pair<float, float>>>>,
    std::shared_ptr<std::unordered_set<std::string>>>;

class Event {
public:
  // Defines the type of the event
  size_t type;
  // Maps the event type to the parameters it requires.
  std::unordered_map<std::string, VariantType> parameters;

  int64_t timestamp;

  Event(const std::string &event_type, int64_t event_timestamp = 0)
      : type(std::hash<std::string>{}(event_type)), timestamp(event_timestamp) {
  }
};

typedef struct {
  bool operator()(const Event &lhs, const Event &rhs) const {
    return lhs.timestamp > rhs.timestamp;
  }
} EventCompare;