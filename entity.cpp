#include "Entity.hpp"

Entity::Entity(const Vector2& position, const Vector2& velocity, float mass, bool isAffectedByGravity,
    bool isMovable, bool isHittable, ShapeType shapeType, const SDL_Color& color,
    const SDL_Rect& rect, const SDL_Point& center, int radius)
    : position(position), velocity(velocity), mass(mass),
    isAffectedByGravity(isAffectedByGravity), isMovable(isMovable),
    isHittable(isHittable), shape(nullptr), color(color)
{
    switch (shapeType) {
    case ShapeType::RECTANGLE:
        shape = std::make_unique<RectangleShape>(rect);
        break;
    case ShapeType::CIRCLE:
        shape = std::make_unique<CircleShape>(center, radius);
        break;
        // Add more cases as needed
    default:
        shape = nullptr;
        break;
    }
}

void Entity::draw() {
    if (shape) {
        shape->draw(color);
    }
}