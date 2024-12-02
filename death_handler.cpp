#include "death_handler.hpp"

DeathHandler::DeathHandler(Timeline *timeline) : timeline(timeline) {}

void DeathHandler::on_event(const Event &event) {
  if (event.type == death_event_hash) {
    std::shared_ptr<Entity> player =
        std::get<std::shared_ptr<Entity>>(event.parameters.at("player"));
    handle_death(player);
  }
}

void DeathHandler::handle_death(std::shared_ptr<Entity> player) {
  // Can do other stuff regarding death here, but currently we only want to
  // spawn the character when it dies
  player->isMovable = false;

  terminateThreads.store(true);
}
