#include "main.hpp"
#include <memory>
#include <iostream>

int main(int argc, char* argv[])
{
    initSDL();

    // game using game engine
    int game = rushil_game1();

    clean_up_sdl();

    return game;
}
