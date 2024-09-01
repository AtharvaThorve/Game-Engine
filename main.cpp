#include "main.hpp"
#include <memory>
#include <iostream>

int obj2_X = 100;
int obj2_Y = 100;

int main(int argc, char* argv[]) 
{
    initSDL();

	initPhysics(0.0f, 9.8f);


	Uint32 lastTime = SDL_GetTicks();

	while (1)
	{
		doInput();

		// Uint32 currentTime = SDL_GetTicks();
		// float deltaTime = (currentTime - lastTime) / 1000.0f;
		// lastTime = currentTime;

		// entityManager.updateEntities(deltaTime);

		prepareScene(SDL_Color{0, 0, 255, 0});
		// entityManager.renderEntities();
		presentScene();

		SDL_RenderPresent(app->renderer);
	}

	clean_up_physics_system();
	clean_up_sdl();
	
	return 0;
}