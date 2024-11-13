#include "position_handler.hpp"

PositionHandler::PositionHandler(Timeline *timeline)
    : timeline(timeline) {}

void PositionHandler::on_event(const Event &event) {
  if (event.type == position_event_hash) {
    std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<Entity>>>
        dict = std::get<std::shared_ptr<
            std::unordered_map<std::string, std::shared_ptr<Entity>>>>(
            event.parameters.at("dict"));
    std::string identifier =
        std::get<std::string>(event.parameters.at("identifier"));
    Vector2 newPosition = std::get<Vector2>(event.parameters.at("newPosition"));
    (*dict)[identifier]->position = newPosition;
  }
}