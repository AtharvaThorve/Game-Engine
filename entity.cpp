#include "Entity.hpp"

// Initialize the static member variable.
int64_t Entity::nextID = 0;  // Start the unique ID from 0 or any other value.

Entity::Entity(const Vector2& position, const Vector2& velocity, float mass,
    bool isAffectedByGravity, bool isMovable, bool isHittable, ShapeType shapeType,
    const SDL_Color& color, const SDL_Rect& rect, const SDL_Point& center, int radius,
    Timeline* anchor, int64_t tic)
    : position(position), velocity(velocity), mass(mass), isAffectedByGravity(isAffectedByGravity),
    isMovable(isMovable), isHittable(isHittable), shape(nullptr), color(color), timeline(anchor, tic),
    lastUpdateTime(timeline.getTime()), lastGlobalTicSize(timeline.getAnchorTic()), id(nextID++)  // Assign unique ID
{
    switch (shapeType) {
    case ShapeType::RECTANGLE:
        shape = std::make_unique<RectangleShape>(rect);
        break;
    case ShapeType::CIRCLE:
        shape = std::make_unique<CircleShape>(radius, center);
        break;
    default:
        shape = nullptr;
        break;
    }
}

void Entity::updatePosition(float deltaTime) {
    int64_t currentGlobalTicSize = timeline.getAnchorTic();
    if (currentGlobalTicSize != lastGlobalTicSize) {
        rescaleLastUpdateTime(lastGlobalTicSize, currentGlobalTicSize);
        lastGlobalTicSize = currentGlobalTicSize;
    }

    int64_t currentTime = timeline.getTime();
    deltaTime = (currentTime - lastUpdateTime) / NANOSECONDS_TO_SECONDS; // Nanoseconds to seconds
    lastUpdateTime = currentTime;
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

void Entity::rescaleLastUpdateTime(int64_t oldGlobalTicSize, int64_t newGlobalTicSize) {
    if (oldGlobalTicSize != newGlobalTicSize) {
        double scaleFactor = static_cast<double>(oldGlobalTicSize) / newGlobalTicSize;

        lastUpdateTime = static_cast<int64_t>(lastUpdateTime * scaleFactor);
    }
}

// Add this getter to retrieve the unique ID.
int64_t Entity::getID() const {
    return id;
}
