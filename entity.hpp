#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <memory>
#include "structs.hpp"
#include "Shape.hpp"
#include "MovementPattern.hpp"
#include "Timeline.hpp"
#include "defs.hpp"

extern App* app;

class Entity {
public:
    // Member variables for position, velocity, etc.
    Vector2 position;
    Vector2 velocity;
    Vector2 inputVelocity = { 0, 0 };
    Vector2 patternVelocity = { 0, 0 };
    Vector2 acceleration = { 0 , 0 };
    Vector2 inputAcceleration = { 0 ,0 };
    float mass;
    bool isAffectedByGravity;
    bool isMovable;
    bool isHittable;
    std::unique_ptr<Shape> shape;
    SDL_Color color;
    bool hasMovementPattern = false;
    MovementPattern movementPattern;
    Timeline timeline;
    int64_t lastUpdateTime;
    int64_t lastGlobalTicSize;
    float deltaTime;

    // Static member variable to track the next unique ID
    static int64_t nextID;

    // Member variable to store this entity's unique ID
    int64_t id;

    // Constructor
    Entity(const Vector2& position, const Vector2& velocity, const Vector2& acceleration, float mass, bool isAffectedByGravity,
        bool isMovable, bool isHittable, ShapeType shapeType, const SDL_Color& color, const SDL_Rect& rect,
        const SDL_Point& center, int radius, Timeline* anchor, int64_t tic = 1);

    // Destructor
    ~Entity() = default;

    // Move constructor and move assignment
    Entity(Entity&&) noexcept = default;
    Entity& operator=(Entity&&) noexcept = default;

    // Deleted copy constructor and assignment operator
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;
    
    void updateDeltaTime();

    void updatePosition();

    // Method to draw the entity
    void draw();

    // Method to check for collisions with another entity
    bool isColliding(const Entity& other) const;

    // Rescale the last update time when the global tic size changes
    void rescaleLastUpdateTime(int64_t oldGlobalTicSize, int64_t newGlobalTicSize);

    // Getter for the entity's unique ID
    int64_t getID() const;

private:
    void updateSDLObject();
};
