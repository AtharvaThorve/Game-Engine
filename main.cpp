#include "main.hpp"
#include <memory>

int rectX = (SCREEN_WIDTH / 2) - 100; // Initialize X position
int rectY = (SCREEN_HEIGHT / 2) - 200; // Initialize Y position

int main(int argc, char* argv[])
{
    initSDL();

    SDL_Color screenColor;
    screenColor.r = 0x00;
    screenColor.g = 0x00;
    screenColor.b = 0xFF;
    screenColor.a = 0x00;

    SDL_Color rectColor;
    rectColor.r = 0xFF;
    rectColor.g = 0x00;
    rectColor.b = 0x00;
    rectColor.a = 0x00;

    while (1)
    {
        prepareScene(screenColor);

        doInput();

        drawRect(rectX, rectY, 200, 400); // Use rectX and rectY

        colorRect(rectColor);

        presentScene();
    }

    return 0;
}