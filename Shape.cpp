#include "Shape.hpp"
#include <iostream>

RectangleShape::RectangleShape(const SDL_Rect& rectangle) : rect(rectangle) {
    type = ShapeType::RECTANGLE;
}

void RectangleShape::draw(const SDL_Color& color) {
    SDL_SetRenderDrawColor(app->renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(app->renderer, &rect);
}

SDL_bool RectangleShape::IsColliding(const SDL_Rect& rectangle) {
    return SDL_HasIntersection(&rect, &rectangle);
}

bool RectangleShape::isColliding(const Shape& other) const {
    switch (other.type) {
    case ShapeType::RECTANGLE:
        return SDL_HasIntersection(&rect, &static_cast<const RectangleShape&>(other).rect);
    case ShapeType::CIRCLE:
        return false;
        //return IsColliding(static_cast<const CircleShape&>(other).circleToRect());
    default:
        return false;
    }
}

CircleShape::CircleShape(int radius, const SDL_Point& center) : radius(radius), center(center)
{
    type = ShapeType::CIRCLE;
}

void CircleShape::draw(const SDL_Color& color) {
    SDL_SetRenderDrawColor(app->renderer, color.r, color.g, color.b, color.a);
    drawCircle(center.x, center.y, radius);
}

void CircleShape::drawCircle(int x, int y, int radius) {
    int offsetX = 0;
    int offsetY = radius;
    int d = radius - 1;

    while (offsetY >= offsetX) {
        SDL_RenderDrawPoint(app->renderer, x + offsetX, y + offsetY);
        SDL_RenderDrawPoint(app->renderer, x + offsetY, y + offsetX);
        SDL_RenderDrawPoint(app->renderer, x - offsetX, y + offsetY);
        SDL_RenderDrawPoint(app->renderer, x - offsetY, y + offsetX);
        SDL_RenderDrawPoint(app->renderer, x + offsetX, y - offsetY);
        SDL_RenderDrawPoint(app->renderer, x + offsetY, y - offsetX);
        SDL_RenderDrawPoint(app->renderer, x - offsetX, y - offsetY);
        SDL_RenderDrawPoint(app->renderer, x - offsetY, y - offsetX);

        if (d >= 2 * offsetX) {
            d -= 2 * offsetX + 1;
            offsetX += 1;
        }
        else if (d < 2 * (radius - offsetY)) {
            d += 2 * offsetY - 1;
            offsetY -= 1;
        }
        else {
            d += 2 * (offsetY - offsetX - 1);
            offsetY -= 1;
            offsetX += 1;
        }
    }
}

bool CircleShape::isColliding(const Shape& other) const {
    switch (other.type) {
    case ShapeType::RECTANGLE:
        return other.isColliding(*this);
    case ShapeType::CIRCLE: {
        const CircleShape& otherCircle = static_cast<const CircleShape&>(other);
        int dx = center.x - otherCircle.center.x;
        int dy = center.y - otherCircle.center.y;
        int distanceSquared = dx * dx + dy * dy;
        int radiusSum = radius + otherCircle.radius;
        return distanceSquared <= radiusSum * radiusSum;
    }
    default:
        return false;
    }
}

SDL_Rect CircleShape::circleToRect() const {
    return SDL_Rect{
        center.x - radius,
        center.y - radius,
        2 * radius,
        2 * radius
    };
}