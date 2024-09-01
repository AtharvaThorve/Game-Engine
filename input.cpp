#include "input.hpp"
#include <SDL2/SDL.h>


void doInput(Entity* entity, int move_speed = 5)
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

    // Check if the UP key is pressed
    if (state[SDL_SCANCODE_UP])
    {
        if (entity->position.y - move_speed >= 0) // Check upper boundary
        {
            entity->position.y -= move_speed;
        }
    }

    // Check if the DOWN key is pressed
    if (state[SDL_SCANCODE_DOWN])
    {
        if (entity->position.y + 400 + move_speed <= SCREEN_HEIGHT) // Check lower boundary
        {
            entity->position.y += move_speed;
        }
    }

    // Check if the LEFT key is pressed
    if (state[SDL_SCANCODE_LEFT])
    {
        if (entity->position.x - move_speed >= 0) // Check left boundary
        {
            entity->position.x -= move_speed;
        }
    }

    // Check if the RIGHT key is pressed
    if (state[SDL_SCANCODE_RIGHT])
    {
        if (entity->position.x + 200 + move_speed <= SCREEN_WIDTH) // Check right boundary
        {
            entity->position.x += move_speed;
        }
    }
}