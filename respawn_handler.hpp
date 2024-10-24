#pragma once
#include "entity.hpp"
#include "event_manager.hpp"
#include "timeline.hpp"
#include <memory>
#include <unordered_set>

class RespawnHandler : public EventHandler {
public:
  RespawnHandler(EventManager *em, Timeline *timeline);
  void on_event(const Event &event) override;

  void add_spawn_point(std::shared_ptr<Entity> spawnPoint);
  template <typename... Args> void add_spawn_points(Args &&...args) {
    add_spawn_points_helper(std::forward<Args>(args)...);
  }

private:
  EventManager *em;
  Timeline *timeline;
  const size_t respawn_event_hash = std::hash<std::string>{}("respawn");
  std::unordered_set<std::shared_ptr<Entity>> spawn_points;

  void add_spawn_points_helper() {}
  template <typename T, typename... Args>
  void add_spawn_points_helper(T &&first_point, Args &&...rest_points) {
    add_spawn_point(std::forward<T>(first_point));
    add_spawn_points_helper(std::forward<Args>(rest_points)...);
  }

  void handle_respawn(std::shared_ptr<Entity> player);
};