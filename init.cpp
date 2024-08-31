#include "init.hpp"

void initSDL(void) {
	
	if (SDL_Init(SDL_INIT_VIDEO)) {
		// Error Initializing
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not Init SDL: %s", SDL_GetError());
		exit(3);
	}

	app->window = SDL_CreateWindow("Hello SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);

	if (!app->window) {
		// Error Creating Window
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create SDL Window: %s", SDL_GetError());
		exit(3);
	}

	app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (!app->renderer) {
		// Error Creating Renderer
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create SDL Renderer: %s", SDL_GetError());
		exit(3);
	}
}

void initPhysics(float gravityX, float gravityY) 
{
	init_physics_system(gravityX, gravityY);
}