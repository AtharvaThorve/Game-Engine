#include "input_handler.hpp"

InputHandler::InputHandler(EventManager *em, Timeline *timeline)
    : em(em), timeline(timeline) {}

void InputHandler::on_event(const Event &event) {
  if (event.type == input_event_hash) {

    std::shared_ptr<Entity> player =
        std::get<std::shared_ptr<Entity>>(event.parameters.at("player"));
    size_t input_type = std::get<size_t>(event.parameters.at("input_type"));

    float acceleration_rate = 0;
    if (event.parameters.find("acceleration_rate") != event.parameters.end()) {
      acceleration_rate =
          std::get<float>(event.parameters.at("acceleration_rate"));
    }

    float jump_force = 0;
    if (event.parameters.find("jump_force") != event.parameters.end()) {
      jump_force = std::get<float>(event.parameters.at("jump_force"));
    }

    Vector2 dash_vector;
    if (event.parameters.find("dash_vector") != event.parameters.end()) {
      dash_vector = std::get<Vector2>(event.parameters.at("dash_vector"));
    }

    handle_input(player, input_type, acceleration_rate, jump_force,
                 dash_vector);
  }
}

void InputHandler::handle_input(std::shared_ptr<Entity> player,
                                size_t input_type, float acceleration_rate,
                                float jump_force, Vector2 dash_vector) {
  if (input_type == move_x) {
    player->inputAcceleration.x = acceleration_rate;
  } else if (input_type == stop_x) {
    player->inputAcceleration.x = 0;
  }

  if (input_type == move_y) {
    player->inputAcceleration.y = acceleration_rate;
  } else if (input_type == stop_y) {
    player->inputAcceleration.y = 0;
  }

  if (input_type == jump) {
    player->velocity.y = jump_force;
  }

  if (input_type == dash) {
    player->velocity.x += dash_vector.x;
    player->velocity.y += dash_vector.y;
  }
}