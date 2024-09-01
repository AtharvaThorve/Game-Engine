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

    virtual void draw(const SDL_Color& color) = 0;

    // Virtual method for collision detection
    virtual bool isColliding(const Shape& other) const = 0;
};

class RectangleShape : public Shape {
public:
    SDL_Rect rect;

    RectangleShape(const SDL_Rect& rectangle);

    void draw(const SDL_Color& color) override;

    SDL_bool IsColliding(const SDL_Rect& rectangle);

    // Override for collision detection
    bool isColliding(const Shape& other) const override;

};


class CircleShape : public Shape {
public:
    int radius;
    SDL_Point center;

    CircleShape(int radius, const SDL_Point& center);
    void draw(const SDL_Color& color) override;

    void drawCircle(int x, int y, int radius);
    // Override for collision detection
    bool isColliding(const Shape& other) const override;
    SDL_Rect circleToRect() const;
};