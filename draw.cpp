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

void drawRect(int x, int y, int h, int w)
{
	rect.x = x;
	rect.y = y;
	rect.h = h;
	rect.w = w;
}

void colorRect(SDL_Color color)
{
	SDL_SetRenderDrawColor(app->renderer, color.r, color.g, color.b, color.a);

	SDL_RenderFillRect(app->renderer, &rect);
}
