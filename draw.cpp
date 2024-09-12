#include "draw.hpp"

SDL_Rect rect;

void prepareScene(SDL_Color color)
{
	SDL_SetRenderDrawColor(app->renderer, color.r, color.g, color.b, color.a);

	SDL_RenderClear(app->renderer);
}

void presentScene(void)
{
	SDL_RenderPresent(app->renderer);
}
