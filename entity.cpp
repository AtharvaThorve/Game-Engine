#include "Entity.hpp"

// Initialize the static member variable.
int64_t Entity::nextID = 0;  // Start the unique ID from 0 or any other value.

Entity::Entity(const Vector2& position, const Vector2& velocity, const Vector2& acceleration, float mass,
    bool isAffectedByGravity, bool isMovable, bool isHittable, ShapeType shapeType,
    const SDL_Color& color, const SDL_Rect& rect, const SDL_Point& center, int radius,
    Timeline* anchor, int64_t tic)
    : position(position), velocity(velocity), acceleration(acceleration), mass(mass), isAffectedByGravity(isAffectedByGravity),
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

void Entity::updateDeltaTime() {
    int64_t currentGlobalTicSize = timeline.getAnchorTic();
    if (currentGlobalTicSize != lastGlobalTicSize) {
        rescaleLastUpdateTime(lastGlobalTicSize, currentGlobalTicSize);
        lastGlobalTicSize = currentGlobalTicSize;
    }
    int64_t currentTime = timeline.getTime();
    deltaTime = (currentTime - lastUpdateTime) / NANOSECONDS_TO_SECONDS; // Nanoseconds to seconds
    lastUpdateTime = currentTime;
}

void Entity::updatePosition() {

    //Vector2 finalVelocity = velocity;
    Vector2 finalAcceleration = acceleration;

    finalAcceleration.x += inputAcceleration.x;
    finalAcceleration.y += inputAcceleration.y;

    velocity.x += finalAcceleration.x * deltaTime;
    velocity.y += finalAcceleration.y * deltaTime;

    if (hasMovementPattern) {
        // Todo: With this if gravity is turned on any effects from that would be gone, so need to fix it.
        velocity.x = patternVelocity.x;
        velocity.y = patternVelocity.y;
    }

    float maxVelocity = 300.0f;
    if (velocity.x > maxVelocity) velocity.x = maxVelocity;
    if (velocity.x < -maxVelocity) velocity.x = -maxVelocity;
    if (velocity.y > maxVelocity) velocity.y = maxVelocity;
    if (velocity.y < -maxVelocity) velocity.y = -maxVelocity;

    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
}

void Entity::draw(float cameraX, float cameraY) {
    if (shape) {
        updateSDLObject(cameraX, cameraY);
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
        lastUpdateTime = lastUpdateTime * timeline.getTic() + timeline.getStartTime();
        lastUpdateTime = static_cast<int64_t>(lastUpdateTime * scaleFactor);
        lastUpdateTime -= timeline.getStartTime();
        lastUpdateTime /= timeline.getTic();
    }
}

// Add this getter to retrieve the unique ID.
int64_t Entity::getID() const {
    return id;
}

void Entity::updateSDLObject(float cameraX, float cameraY) {
    if (shape->type == ShapeType::RECTANGLE) {
        RectangleShape* rectShape = dynamic_cast<RectangleShape*>(shape.get());
        if (rectShape) {
            rectShape->rect.x = static_cast<int>(position.x - cameraX);
            rectShape->rect.y = static_cast<int>(position.y - cameraY);
        }
    }
    else if (shape->type == ShapeType::CIRCLE) {
        CircleShape* circleShape = dynamic_cast<CircleShape*>(shape.get());
        if (circleShape) {
            circleShape->center.x = static_cast<int>(position.x - cameraX);
            circleShape->center.y = static_cast<int>(position.y - cameraY);
        }
    }
}
