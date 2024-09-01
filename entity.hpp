#pragma once
#pragma once
#include "structs.hpp"
#include <SDL2/SDL.h>
#include <memory>
#include "Shape.hpp"

extern App* app;

class Entity {
public:
    Vector2 position;
    Vector2 velocity;
    float mass;
    bool isAffectedByGravity;
    bool isMovable;
    bool isHittable;
    std::unique_ptr<Shape> shape;
    SDL_Color color;

    Entity(const Vector2& position, const Vector2& velocity, float mass, bool isAffectedByGravity,
        bool isMovable, bool isHittable, ShapeType shapeType, const SDL_Color& color,
        const SDL_Rect& rect, const SDL_Point& center, int radius);


    ~Entity() = default;

    // Move constructor and move assignment operator (Rule of Five)
    Entity(Entity&&) noexcept = default;
    Entity& operator=(Entity&&) noexcept = default;

    // Delete copy constructor and copy assignment to enforce unique ownership
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    void updatePosition(float deltaTime);


    // Method to draw the entity
    void draw();
};