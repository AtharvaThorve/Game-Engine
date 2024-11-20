#include "input.hpp"

static int64_t dash_start_time = 0;
static std::optional<size_t> dash_direction_1;
static std::optional<size_t> dash_direction_2;

static bool wasLShiftPressed = false;
static bool wasEscPressed = false;
static bool wasPlusPressed = false;
static bool wasMinusPressed = false;
static bool wasSpacePressed = false;
static bool wasEnterPressed = false;
static bool isRecording = false;
static bool wasRShiftPressed = false;
static bool wasKPressed = false;
static std::unordered_set<size_t> pressed_directions;

// Helper function for raising movement events
void raiseMovementEvent(const std::string &input_type,
                        std::shared_ptr<Entity> entity, float rate,
                        Timeline *timeline) {
  Event event("input", timeline->getTime());
  event.parameters["player"] = entity;
  event.parameters["input_type"] = std::hash<std::string>{}(input_type);
  event.parameters["acceleration_rate"] = rate;
  EventManager &em = EventManager::getInstance();
  em.raise_event(event);
}

// Checks if the current direction combination is valid
bool isValidDirectionCombo(const std::unordered_set<size_t> &directions) {
  return (directions.count(std::hash<std::string>{}("up")) +
              directions.count(std::hash<std::string>{}("down")) <=
          1) &&
         (directions.count(std::hash<std::string>{}("left")) +
              directions.count(std::hash<std::string>{}("right")) <=
          1) &&
         (directions.size() > 0);
}

// Process dash logic with validated directions
void processDashInput(std::shared_ptr<Entity> entity, Timeline *timeline,
                      float dash_speed, float dash_duration) {
  float dash_x = 0.0f, dash_y = 0.0f;
  if (dash_direction_1) {
    if (*dash_direction_1 == std::hash<std::string>{}("up"))
      dash_y = -dash_speed;
    else if (*dash_direction_1 == std::hash<std::string>{}("down"))
      dash_y = dash_speed;
    else if (*dash_direction_1 == std::hash<std::string>{}("left"))
      dash_x = -dash_speed;
    else if (*dash_direction_1 == std::hash<std::string>{}("right"))
      dash_x = dash_speed;
  }
  if (dash_direction_2) {
    if (*dash_direction_2 == std::hash<std::string>{}("up"))
      dash_y = -dash_speed;
    else if (*dash_direction_2 == std::hash<std::string>{}("down"))
      dash_y = dash_speed;
    else if (*dash_direction_2 == std::hash<std::string>{}("left"))
      dash_x = -dash_speed;
    else if (*dash_direction_2 == std::hash<std::string>{}("right"))
      dash_x = dash_speed;
  }

  Event dash_event("input", timeline->getTime());
  dash_event.parameters["player"] = entity;
  dash_event.parameters["input_type"] = std::hash<std::string>{}("dash");
  dash_event.parameters["dash_vector"] = Vector2{dash_x, dash_y};
  EventManager &em = EventManager::getInstance();
  em.raise_event(dash_event);
  Event stop_dash_event("input", timeline->getTime() + dash_duration);
  stop_dash_event.parameters["player"] = entity;
  stop_dash_event.parameters["input_type"] =
      std::hash<std::string>{}("stop_dash");
  em.raise_event(stop_dash_event);
}

// Main input handler
void doInput(std::shared_ptr<Entity> entity, Timeline *globalTimeline,
             ScriptManager *sm, float accelerationRate, float dash_speed,
             float dash_duration) {
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  EventManager &em = EventManager::getInstance();

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      Client::disconnectRequested.store(true);
      // exit(0);
    }
  }

  // Toggle pause
  bool isEscPressed = state[SDL_SCANCODE_ESCAPE];
  if (isEscPressed && !wasEscPressed) {
    globalTimeline->isPaused() ? globalTimeline->unpause()
                               : globalTimeline->pause();
    wasEscPressed = isEscPressed;
  }

  // Handle speed control with P and M
  if (state[SDL_SCANCODE_P] && !wasPlusPressed &&
      globalTimeline->getTic() > 1) {
    globalTimeline->changeTic(globalTimeline->getTic() * 0.5);
    wasPlusPressed = true;
  } else if (state[SDL_SCANCODE_M] && !wasMinusPressed &&
             globalTimeline->getTic() < 8) {
    globalTimeline->changeTic(globalTimeline->getTic() * 2.0);
    wasMinusPressed = true;
  }

  // Handle Replay Events
  bool enterPressed = state[SDL_SCANCODE_RETURN];
  if (enterPressed && !wasEnterPressed) {
    wasEnterPressed = true;
  } else if (!enterPressed && wasEnterPressed) {
    isRecording = !isRecording;
    if (isRecording) {
      std::cout << "Started Recording" << std::endl;
      Event start_recording_event("start_recording", globalTimeline->getTime());

      em.raise_event(start_recording_event);
    } else {
      std::cout << "Stopped Recording" << std::endl;
      Event stop_recording_event("stop_recording", globalTimeline->getTime());
      em.raise_event(stop_recording_event);
    }
    wasEnterPressed = false;
  }

  bool rShiftPressed = state[SDL_SCANCODE_RSHIFT];
  if (rShiftPressed != wasRShiftPressed) {
    if (rShiftPressed) {
      std::cout << "Playing Recording" << std::endl;
      Event play_recording_event("play_recording", globalTimeline->getTime());
      em.raise_event(play_recording_event);
    }
    wasRShiftPressed = rShiftPressed;
  }

  // Handle script inputs
  bool h = state[SDL_SCANCODE_H];
  if(h) {
    sm->runOne("hello_world", false, "default");
  }
  bool k = state[SDL_SCANCODE_K];
  if(k != wasKPressed) {
    if(k)
      sm->runOne("player", false, "default");
    wasKPressed = k;
  }

  // Handle player movement
  if (entity->isMovable) {
    bool l_shift_pressed = state[SDL_SCANCODE_LSHIFT];

    if (state[SDL_SCANCODE_W]) {
      pressed_directions.insert(std::hash<std::string>{}("up"));
      raiseMovementEvent("move_y", entity, -accelerationRate, globalTimeline);
    } else if (state[SDL_SCANCODE_S]) {
      pressed_directions.insert(std::hash<std::string>{}("down"));
      raiseMovementEvent("move_y", entity, accelerationRate, globalTimeline);
    } else {
      pressed_directions.erase(std::hash<std::string>{}("up"));
      pressed_directions.erase(std::hash<std::string>{}("down"));
      if (entity->inputAcceleration.y != 0)
        raiseMovementEvent("stop_y", entity, 0, globalTimeline);
    }

    if (state[SDL_SCANCODE_A]) {
      pressed_directions.insert(std::hash<std::string>{}("left"));
      raiseMovementEvent("move_x", entity, -accelerationRate, globalTimeline);
    } else if (state[SDL_SCANCODE_D]) {
      pressed_directions.insert(std::hash<std::string>{}("right"));
      raiseMovementEvent("move_x", entity, accelerationRate, globalTimeline);
    } else {
      pressed_directions.erase(std::hash<std::string>{}("left"));
      pressed_directions.erase(std::hash<std::string>{}("right"));
      if (entity->inputAcceleration.x != 0)
        raiseMovementEvent("stop_x", entity, 0, globalTimeline);
    }

    if ((pressed_directions.size() > 1 &&
         !isValidDirectionCombo(pressed_directions)) ||
        !l_shift_pressed)
      pressed_directions.clear();

    if (l_shift_pressed != wasLShiftPressed) {
      if (l_shift_pressed) {
        allowScaling = !allowScaling;
      }
      wasLShiftPressed = l_shift_pressed;
    }

    if (l_shift_pressed && !entity->isDashing &&
        isValidDirectionCombo(pressed_directions)) {
      dash_direction_1 = *pressed_directions.begin();
      dash_direction_2 =
          (pressed_directions.size() > 1)
              ? std::optional<size_t>(*std::next(pressed_directions.begin()))
              : std::nullopt;
      dash_start_time = globalTimeline->getTime();
      processDashInput(entity, globalTimeline, dash_speed, dash_duration);
    }

    bool isSpacePressed = state[SDL_SCANCODE_SPACE];
    if (isSpacePressed && !wasSpacePressed && entity->standingPlatform) {
      // TODO: Update the jumpForce so that user provides it.

      Event jump_event("input", globalTimeline->getTime() + 10);
      jump_event.parameters["player"] = entity;
      jump_event.parameters["input_type"] = std::hash<std::string>{}("jump");
      jump_event.parameters["jump_force"] = -150.0f;
      em.raise_event(jump_event);
    }
    wasSpacePressed = isSpacePressed;
  }
}
