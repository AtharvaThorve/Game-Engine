#pragma once
#include <SDL2/SDL.h>
#include "structs.hpp"

extern App* app;

class Rectangle {
public:
	//constructor
	Rectangle(SDL_Renderer*& renderer);

	~Rectangle();

	void Draw(int x, int y, int w, int h);

	SDL_bool IsColliding(Rectangle& obj);

	void Render(SDL_Renderer*& renderer);

private:
	inline SDL_Rect GetRectangle() const { return rectangle; }
	SDL_Rect rectangle;
};