#pragma once
#include "structs.hpp"
#include <SDL2/SDL.h>

extern App* app;

enum class ShapeType {
    RECTANGLE,
    CIRCLE,
    // add more
};

class Shape {
public:
    ShapeType type;

    Shape() = default;
    virtual ~Shape() = default; // Virtual destructor

    // Optionally, add virtual methods for drawing, updating, etc.
    virtual void draw(const SDL_Color& color) = 0;
};

class RectangleShape : public Shape {
public:
    SDL_Rect rect;

    RectangleShape(const SDL_Rect& rectangle);

    void draw(const SDL_Color& color) override;

    // Additional methods specific to RectangleShape
};


class CircleShape : public Shape {
public:
    int radius;
    SDL_Point center;

    CircleShape(int radius, const SDL_Point& center);
    void draw(const SDL_Color& color) override;

private:
    void drawCircle(int x, int y, int radius);
};