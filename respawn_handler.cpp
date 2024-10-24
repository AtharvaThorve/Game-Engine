#include "respawn_handler.hpp"

RespawnHandler::RespawnHandler(EventManager *em, Timeline *timeline)
    : em(em), timeline(timeline) {}

void RespawnHandler::add_spawn_point(std::shared_ptr<Entity> spawnPoint) {
  spawn_points.insert(spawnPoint);
}

void RespawnHandler::on_event(const Event &event) {
  if (event.type == respawn_event_hash) {
    std::shared_ptr<Entity> player =
        std::get<std::shared_ptr<Entity>>(event.parameters.at("player"));
    handle_respawn(player);
  }
}

void RespawnHandler::handle_respawn(std::shared_ptr<Entity> player) {
  float playerX = player->position.x;
  float playerY = player->position.y;
  std::shared_ptr<Entity> closestSpawn = nullptr;
  float minDistance = std::numeric_limits<float>::max();

  for (auto &spawnPoint : spawn_points) {
    if (spawnPoint->position.x < playerX) {
      float distance = abs(playerX - spawnPoint->position.x) +
                       abs(playerY - spawnPoint->position.y);
      if (distance < minDistance) {
        minDistance = distance;
        closestSpawn = spawnPoint;
      }
    }
  }

  if (closestSpawn) {
    player->position = closestSpawn->position;
    player->velocity = {0, 0};
  }
}