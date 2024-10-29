#include "input.hpp"

static bool wasRShiftPressed = false;
static bool wasEscPressed = false;

static bool wasPlusPressed = false;
static bool wasMinusPressed = false;

static bool wasSpacePressed = false;

void doInput(std::shared_ptr<Entity> entity, Timeline *globalTimeline,
             EventManager *em, float accelerationRate, float decelerationRate) {
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      exit(0);
    }
  }

  // Toggle pause/unpause when Escape key is pressed
  bool isEscPressed = state[SDL_SCANCODE_ESCAPE];
  if (isEscPressed != wasEscPressed) {
    if (isEscPressed) {
      if (globalTimeline->isPaused()) {
        globalTimeline->unpause();
      } else {
        globalTimeline->pause();
      }
    }
    wasEscPressed = isEscPressed;
  }

  bool game_speed_up = state[SDL_SCANCODE_P];

  bool game_speed_down = state[SDL_SCANCODE_M];

  if (game_speed_up != wasPlusPressed) {
    if (game_speed_up && globalTimeline->getTic() > 1) {
      globalTimeline->changeTic(globalTimeline->getTic() * 0.5);
    }
    wasPlusPressed = game_speed_up;
  }

  if (game_speed_down != wasMinusPressed) {
    if (game_speed_down && globalTimeline->getTic() < 8) {
      globalTimeline->changeTic(globalTimeline->getTic() * 2.0);
    }
    wasMinusPressed = game_speed_down;
  }

  if (entity->isMovable) {
    if (state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W]) {

      Event move_up_event("input", globalTimeline->getTime() + 10);
      move_up_event.parameters["player"] = entity;
      move_up_event.parameters["input_type"] =
          std::hash<std::string>{}("move_y");
      move_up_event.parameters["acceleration_rate"] = -accelerationRate;
      em->raise_event(move_up_event);

    } else if (state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S]) {

      Event move_down_event("input", globalTimeline->getTime() + 10);
      move_down_event.parameters["player"] = entity;
      move_down_event.parameters["input_type"] =
          std::hash<std::string>{}("move_y");
      move_down_event.parameters["acceleration_rate"] = accelerationRate;
      em->raise_event(move_down_event);

    } else {

      Event stop_up_event("input", globalTimeline->getTime() + 10);
      stop_up_event.parameters["player"] = entity;
      stop_up_event.parameters["input_type"] =
          std::hash<std::string>{}("stop_y");
      em->raise_event(stop_up_event);
    }

    if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A]) {

      Event move_left_event("input", globalTimeline->getTime() + 10);
      move_left_event.parameters["player"] = entity;
      move_left_event.parameters["input_type"] =
          std::hash<std::string>{}("move_x");
      move_left_event.parameters["acceleration_rate"] = -accelerationRate;
      em->raise_event(move_left_event);

    } else if (state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D]) {

      Event move_right_event("input", globalTimeline->getTime() + 10);
      move_right_event.parameters["player"] = entity;
      move_right_event.parameters["input_type"] =
          std::hash<std::string>{}("move_x");
      move_right_event.parameters["acceleration_rate"] = accelerationRate;
      em->raise_event(move_right_event);

    } else {

      Event stop_up_event("input", globalTimeline->getTime() + 10);
      stop_up_event.parameters["player"] = entity;
      stop_up_event.parameters["input_type"] =
          std::hash<std::string>{}("stop_x");
      em->raise_event(stop_up_event);
    }

    bool isSpacePressed = state[SDL_SCANCODE_SPACE];
    if (isSpacePressed && !wasSpacePressed && entity->standingPlatform) {
      // Todo: Update the jumpForce so that user provides it.

      Event jump_event("input", globalTimeline->getTime() + 10);
      jump_event.parameters["player"] = entity;
      jump_event.parameters["input_type"] = std::hash<std::string>{}("jump");
      jump_event.parameters["jump_force"] = -150.0f;
      em->raise_event(jump_event);
    }
  }

  bool isRShiftPressed = state[SDL_SCANCODE_RSHIFT];

  if (isRShiftPressed != wasRShiftPressed) {
    if (!isRShiftPressed) {
      allowScaling = !allowScaling;
    }
    wasRShiftPressed = isRShiftPressed;
  }
}
