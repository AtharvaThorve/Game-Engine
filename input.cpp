#include "input.hpp"

static Uint64 dash_start_time = 0;
static std::optional<size_t> dash_direction_1;
static std::optional<size_t> dash_direction_2;
static bool was_l_shift_pressed = false;
static bool is_dashing = false;
static float dash_duration = 3000000000;

static bool wasRShiftPressed = false;
static bool wasEscPressed = false;

static bool wasPlusPressed = false;
static bool wasMinusPressed = false;

static bool wasSpacePressed = false;

void doInput(std::shared_ptr<Entity> entity, Timeline *globalTimeline,
             EventManager *em, float accelerationRate, float dash_speed,
             float decelerationRate) {
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
    float dash_x = 0.0f, dash_y = 0.0f;

    bool l_shift_pressed = state[SDL_SCANCODE_LSHIFT];

    if (is_dashing) {
      if (globalTimeline->getTime() - dash_start_time >= dash_duration) {
        is_dashing = false;
        entity->velocity.x = 0;
        entity->velocity.y = 0;
      } else {
        return;
      }
    }

    if (l_shift_pressed && !was_l_shift_pressed) {
        if (!is_dashing) {  // Trigger dash only if not currently dashing
            dash_start_time = globalTimeline->getTime();
            dash_direction_1 = std::nullopt;
            dash_direction_2 = std::nullopt;
            is_dashing = true; // Start dashing
        }
    }

    was_l_shift_pressed = l_shift_pressed;

    std::optional<size_t> current_dir;

    if (state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W]) {

      current_dir = std::hash<std::string>{}("up");
      Event move_up_event("input", globalTimeline->getTime() + 10);
      move_up_event.parameters["player"] = entity;
      move_up_event.parameters["input_type"] =
          std::hash<std::string>{}("move_y");
      move_up_event.parameters["acceleration_rate"] = -accelerationRate;
      em->raise_event(move_up_event);

    } else if (state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S]) {

      current_dir = std::hash<std::string>{}("down");
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

      current_dir = std::hash<std::string>{}("left");
      Event move_left_event("input", globalTimeline->getTime() + 10);
      move_left_event.parameters["player"] = entity;
      move_left_event.parameters["input_type"] =
          std::hash<std::string>{}("move_x");
      move_left_event.parameters["acceleration_rate"] = -accelerationRate;
      em->raise_event(move_left_event);

    } else if (state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D]) {

      current_dir = std::hash<std::string>{}("right");
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

    const Uint64 dash_delay = 1000000;
    if (l_shift_pressed && current_dir) {
      if (!dash_direction_1) {
        dash_direction_1 = current_dir;
        dash_start_time = globalTimeline->getTime();
      } else if (*current_dir != dash_direction_1) {
        dash_direction_2 = current_dir;
      }
    }

    if (l_shift_pressed && dash_direction_1 &&
        (globalTimeline->getTime() - dash_start_time >= dash_delay ||
         dash_direction_2)) {
      if (*dash_direction_1 == std::hash<std::string>{}("up"))
        dash_y = -dash_speed;
      if (*dash_direction_1 == std::hash<std::string>{}("down"))
        dash_y = dash_speed;
      if (*dash_direction_1 == std::hash<std::string>{}("left"))
        dash_x = -dash_speed;
      if (*dash_direction_1 == std::hash<std::string>{}("right"))
        dash_x = dash_speed;

      if (dash_direction_2) {
        if (*dash_direction_2 == std::hash<std::string>{}("up"))
          dash_y = -dash_speed;
        if (*dash_direction_2 == std::hash<std::string>{}("down"))
          dash_y = dash_speed;
        if (*dash_direction_2 == std::hash<std::string>{}("left"))
          dash_x = -dash_speed;
        if (*dash_direction_2 == std::hash<std::string>{}("right"))
          dash_x = dash_speed;
      }
      Event dash_event("input", globalTimeline->getTime());
      dash_event.parameters["player"] = entity;
      dash_event.parameters["input_type"] = std::hash<std::string>{}("dash");
      Vector2 dash_vector{dash_x, dash_y};
      dash_event.parameters["dash_vector"] = dash_vector;
      em->raise_event(dash_event);

      std::cout << "Dashing" << std::endl;

      dash_direction_1.reset();
      dash_direction_2.reset();
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
