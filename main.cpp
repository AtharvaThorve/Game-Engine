#include "main.hpp"
#include <memory>

int main(int argc, char* argv[]) 
{
	initSDL();

	SDL_Color myColor;
	myColor.r = 0x00;
	myColor.g = 0x00;
	myColor.b = 0xFF;
	myColor.a = 0x00;

	while (1)
	{
		prepareScene(myColor);

		presentScene();
	}

	return 0;
}