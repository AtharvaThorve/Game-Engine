#include "Entity.hpp"
#include <iostream>

Entity::Entity(const Vector2& position, const Vector2& velocity, float mass, bool isAffectedByGravity,
    bool isMovable, bool isHittable, ShapeType shapeType, const SDL_Color& color, const SDL_Rect& rect, const SDL_Point& center, int radius)
    : position(position), velocity(velocity), mass(mass), isAffectedByGravity(isAffectedByGravity), 
    isMovable(isMovable), isHittable(isHittable), shape(nullptr), color(color)
{
    switch (shapeType) {
    case ShapeType::RECTANGLE:
        shape = std::make_unique<RectangleShape>(rect);
        break;
    case ShapeType::CIRCLE:
        shape = std::make_unique<CircleShape>(radius, center);
        break;
        // Add more cases as needed
    default:
        shape = nullptr;
        break;
    }
}

void Entity::updatePosition(float deltaTime) {
    Vector2 finalVelocity = velocity;

    finalVelocity.x += inputVelocity.x;
    finalVelocity.y += inputVelocity.y;
    if (hasMovementPattern) {
        finalVelocity.x += patternVelocity.x;
        finalVelocity.y += patternVelocity.y;
    }

    position.x += finalVelocity.x * deltaTime;
    position.y += finalVelocity.y * deltaTime;

    if (shape->type == ShapeType::RECTANGLE) {
        RectangleShape* rectShape = dynamic_cast<RectangleShape*>(shape.get());
        if (rectShape) {
            rectShape->rect.x = static_cast<int>(position.x);
            rectShape->rect.y = static_cast<int>(position.y);
        }
    }
    else if (shape->type == ShapeType::CIRCLE) {
        CircleShape* circleShape = dynamic_cast<CircleShape*>(shape.get());
        if (circleShape) {
            circleShape->center.x = static_cast<int>(position.x);
            circleShape->center.y = static_cast<int>(position.y);
        }
    }
}

void Entity::draw() {
    if (shape) {
        shape->draw(color);
    }
}

bool Entity::isColliding(const Entity& other) const {
    if (shape && other.shape) {
        return shape->isColliding(*other.shape);
    }
    return false;
}