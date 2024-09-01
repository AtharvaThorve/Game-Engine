#include "input.hpp"
#include <SDL2/SDL.h>

void doInput(Entity* entity, float move_speed = 5)
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
        /*entity->velocity.x = 0;
        entity->velocity.y = 0;*/
        if (state[SDL_SCANCODE_UP]) {
            entity->velocity.y = -move_speed; // Move up
        }
        if (state[SDL_SCANCODE_DOWN]) {
            entity->velocity.y = move_speed;  // Move down
        }
        if (state[SDL_SCANCODE_LEFT]) {
            entity->velocity.x = -move_speed; // Move left
        }
        if (state[SDL_SCANCODE_RIGHT]) {
            entity->velocity.x = move_speed;  // Move right
        }
    }
}
