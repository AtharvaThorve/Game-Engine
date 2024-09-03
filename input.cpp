#include "input.hpp"
#include <SDL2/SDL.h>

void doInput(std::shared_ptr<Entity> entity, float move_speedX, float move_speedY)
{
    // Update the state of the keyboard
    const Uint8* state = SDL_GetKeyboardState(NULL);

    // Check if the SDL_QUIT event has been triggered
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            exit(0);
        }
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

        if (state[SDL_SCANCODE_LSHIFT]) {
            allowScaling = false;
        }
        else if (state[SDL_SCANCODE_RSHIFT]) {
            allowScaling = true;
        }
    }
}
