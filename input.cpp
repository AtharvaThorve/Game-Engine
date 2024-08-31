#include "input.hpp"
#include <SDL2/SDL.h>

// Assuming these are defined elsewhere, such as in a header or another source file
extern int rectX;
extern int rectY;
const int MOVE_SPEED = 5;

void doInput(void)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            exit(0);
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_UP:
                if (rectY - MOVE_SPEED >= 0) // Check upper boundary
                {
                    rectY -= MOVE_SPEED;
                }
                break;
            case SDLK_DOWN:
                if (rectY + 400 + MOVE_SPEED <= SCREEN_HEIGHT) // Check lower boundary
                {
                    rectY += MOVE_SPEED;
                }
                break;
            case SDLK_LEFT:
                if (rectX - MOVE_SPEED >= 0) // Check left boundary
                {
                    rectX -= MOVE_SPEED;
                }
                break;
            case SDLK_RIGHT:
                if (rectX + 200 + MOVE_SPEED <= SCREEN_WIDTH) // Check right boundary
                {
                    rectX += MOVE_SPEED;
                }
                break;
            }
            break;
        default:
            break;
        }
    }
}