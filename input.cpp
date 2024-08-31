#include "input.hpp"
#include <SDL2/SDL.h>

// Assuming these are defined elsewhere, such as in a header or another source file
extern int rectX;
extern int rectY;
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
        if (rectY - MOVE_SPEED >= 0) // Check upper boundary
        {
            rectY -= MOVE_SPEED;
        }
    }

    // Check if the DOWN key is pressed
    if (state[SDL_SCANCODE_DOWN])
    {
        if (rectY + 400 + MOVE_SPEED <= SCREEN_HEIGHT) // Check lower boundary
        {
            rectY += MOVE_SPEED;
        }
    }

    // Check if the LEFT key is pressed
    if (state[SDL_SCANCODE_LEFT])
    {
        if (rectX - MOVE_SPEED >= 0) // Check left boundary
        {
            rectX -= MOVE_SPEED;
        }
    }

    // Check if the RIGHT key is pressed
    if (state[SDL_SCANCODE_RIGHT])
    {
        if (rectX + 200 + MOVE_SPEED <= SCREEN_WIDTH) // Check right boundary
        {
            rectX += MOVE_SPEED;
        }
    }
}
