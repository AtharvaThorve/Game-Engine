#include "input.hpp"
#include <SDL2/SDL.h>

void doInput(std::shared_ptr<Entity> entity, float move_speed = 5)
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
        if (state[SDL_SCANCODE_UP]) {
            entity->inputVelocity.y = -move_speed;
        }
        if (state[SDL_SCANCODE_DOWN]) {
            entity->inputVelocity.y = move_speed;
        }
        if (state[SDL_SCANCODE_LEFT]) {
            entity->inputVelocity.x = -move_speed;
        }
        if (state[SDL_SCANCODE_RIGHT]) {
            entity->inputVelocity.x = move_speed;
        }
        if (state[SDL_SCANCODE_RSHIFT]) {
            allowScaling = !allowScaling;
        }
    }
}
