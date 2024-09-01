#include "input.hpp"
#include <SDL2/SDL.h>

// Assuming these are defined elsewhere, such as in a header or another source file
extern int obj2_X;
extern int obj2_Y;
const int MOVE_SPEED = 5;

void doInput(void)
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
        if (obj2_Y - MOVE_SPEED >= 0) // Check upper boundary
        {
            obj2_Y -= MOVE_SPEED;
        }
    }

    // Check if the DOWN key is pressed
    if (state[SDL_SCANCODE_DOWN])
    {
        if (obj2_Y + 400 + MOVE_SPEED <= SCREEN_HEIGHT) // Check lower boundary
        {
            obj2_Y += MOVE_SPEED;
        }
    }

    // Check if the LEFT key is pressed
    if (state[SDL_SCANCODE_LEFT])
    {
        if (obj2_X - MOVE_SPEED >= 0) // Check left boundary
        {
            obj2_X -= MOVE_SPEED;
        }
    }

    // Check if the RIGHT key is pressed
    if (state[SDL_SCANCODE_RIGHT])
    {
        if (obj2_X + 200 + MOVE_SPEED <= SCREEN_WIDTH) // Check right boundary
        {
            obj2_X += MOVE_SPEED;
        }
    }
}