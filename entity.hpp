#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <memory>
#include "structs.hpp"
#include "Shape.hpp"
#include "MovementPattern.hpp"
#include "Timeline.hpp"

extern App* app;

class Entity {
public:
    Vector2 position;
    Vector2 velocity;
    Vector2 inputVelocity = { 0, 0 };
    Vector2 patternVelocity = { 0, 0 };
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

    Entity(const Vector2& position, const Vector2& velocity, float mass, bool isAffectedByGravity,
        bool isMovable, bool isHittable, ShapeType shapeType, const SDL_Color& color, const SDL_Rect& rect,
        const SDL_Point& center, int radius, Timeline* anchor, int64_t tic = 1);

    ~Entity() = default;

    Entity(Entity&&) noexcept = default;
    Entity& operator=(Entity&&) noexcept = default;

    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    void updatePosition(float deltaTime);

    // Method to draw the entity
    void draw();

    // Method to check for collisions with another entity
    bool isColliding(const Entity& other) const;

    void rescaleLastUpdateTime(int64_t oldGlobalTicSize, int64_t newGlobalTicSize);
};