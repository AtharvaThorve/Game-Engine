#include <SDL2/SDL.h>
#include<iostream>

#include "Rectangle.hpp"

// constructor
Rectangle::Rectangle(SDL_Renderer*& renderer) {
	std::cout << "rectangle object created\n";
}

Rectangle::~Rectangle(){

}

void Rectangle::Draw(int x, int y, int w, int h) {
    // Create a rectangle
    rectangle.x = x;
    rectangle.y = y;
    rectangle.w = w;
    rectangle.h = h;
}

//SDL_bool Rectangle::IsColliding(Rectangle& obj) {
//    const SDL_Rect temp = obj.GetRectangle();
//    SDL_bool collision = SDL_HasIntersection(&rectangle, &temp);
//    if (collision) {
//        std::cout << "Rectangles collide\n";
//    }
//    else {
//        std::cout << "Rectangles don't collide\n";
//    }
//    return collision;
//}

void Rectangle::Render(SDL_Renderer*& renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); // default white color
}