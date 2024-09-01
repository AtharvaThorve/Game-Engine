#include "Shape.hpp"

RectangleShape::RectangleShape(const SDL_Rect& rectangle) : rect(rectangle) {
    type = ShapeType::RECTANGLE;
}

void RectangleShape::draw(const SDL_Color& color) {
    SDL_SetRenderDrawColor(app->renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(app->renderer, &rect);
}

CircleShape::CircleShape(int radius, const SDL_Point& center)
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