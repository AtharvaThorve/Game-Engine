#include "input.hpp"
#include <SDL2/SDL.h>

static bool wasRShiftPressed = false;
static bool wasEscPressed = false;

static bool wasPlusPressed = false;
static bool wasMinusPressed = false;

void doInput(std::shared_ptr<Entity> entity, Timeline *globalTimeline, float move_speedX, float move_speedY)
{

    const Uint8* state = SDL_GetKeyboardState(NULL);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            exit(0);
        }
    }

    // Toggle pause/unpause when Escape key is pressed
    bool isEscPressed = state[SDL_SCANCODE_ESCAPE];
    if (isEscPressed != wasEscPressed) {
        if (isEscPressed) {
            if (globalTimeline->isPaused()) {
                globalTimeline->unpause();
            }
            else {
                globalTimeline->pause();
            }
        }
        wasEscPressed = isEscPressed;
    }

    bool game_speed_up = state[SDL_SCANCODE_P];

    bool game_speed_down = state[SDL_SCANCODE_M];

    if (game_speed_up != wasPlusPressed) {
        if (game_speed_up && globalTimeline->getTic()>1) {
            globalTimeline->changeTic(globalTimeline->getTic() * 0.5);
        }
        wasPlusPressed = game_speed_up;
    }

    if (game_speed_down != wasMinusPressed) {
        if (game_speed_down && globalTimeline->getTic()<8)
        {
            globalTimeline->changeTic(globalTimeline->getTic() * 2.0);
        }
        wasMinusPressed = game_speed_down;
    }

    // Set velocity based on input
    if (entity->isMovable) {
        entity->inputVelocity.x = 0;
        entity->inputVelocity.y = 0;
        if (state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W]) {
            entity->inputVelocity.y = -move_speedY;
        }
        if (state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S]) {
            entity->inputVelocity.y = move_speedY;
        }
        if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A]) {
            entity->inputVelocity.x = -move_speedX;
        }
        if (state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D]) {
            entity->inputVelocity.x = move_speedX;
        }

        bool isRShiftPressed = state[SDL_SCANCODE_RSHIFT];

        if (isRShiftPressed != wasRShiftPressed) {
            if (!isRShiftPressed) {
                allowScaling = !allowScaling;
            }
            wasRShiftPressed = isRShiftPressed;
        }
    }
}
