#include "main.hpp"
#include <memory>
#include <iostream>

void setRenderScale(float scaleX, float scaleY) {
    SDL_RenderSetScale(app->renderer, scaleX, scaleY);
}

void updateScaleFactors(float& scale) {
    int currentScreenWidth, currentScreenHeight;
    SDL_GetWindowSize(app->window, &currentScreenWidth, &currentScreenHeight);

    // Calculate the scale factor based on the smaller dimension
    float widthScale = static_cast<float>(currentScreenWidth) / SCREEN_WIDTH;
    float heightScale = static_cast<float>(currentScreenHeight) / SCREEN_HEIGHT;

    // Use the smaller scale to maintain the aspect ratio
    scale = std::min(widthScale, heightScale);
}

int main(int argc, char* argv[])
{
    initSDL();

    // Initialize the physics system with gravity (e.g., 9.8 m/s^2 in the y-direction)
    PhysicsSystem physicsSystem(0.0f, 9.8f);

    // Define entity's initial properties
    Vector2 initialPosition{ 100, 100 };  // Starting position of the entity
    Vector2 initialVelocity{ 0, 0 };      // Starting velocity of the entity
    float mass = 1.0f;                    // Mass of the entity
    bool isAffectedByGravity = true;      // Gravity is applied
    bool isMovable = true;                // Entity can move
    bool isHittable = true;               // Entity can be hit
    ShapeType shapeType = ShapeType::RECTANGLE;  // Choose RECTANGLE or CIRCLE
    SDL_Color color = { 255, 0, 0, 255 }; // Red color
    SDL_Rect rect = { static_cast<int>(initialPosition.x), static_cast<int>(initialPosition.y), 50, 50 }; // Size of rectangle
    SDL_Point center = { 0, 0 };
    int radius = 0;

    Entity entity(initialPosition, initialVelocity, mass, isAffectedByGravity, isMovable, isHittable, shapeType, color, rect, center, radius);

    Vector2 initialPosition1{ 200, 100 };  // Starting position of the entity
    Vector2 initialVelocity1{ 0, 0 };      // Starting velocity of the entity
    float mass1 = 1.0f;                    // Mass of the entity
    bool isAffectedByGravity1 = true;      // Gravity is applied
    bool isMovable1 = true;                // Entity can move
    bool isHittable1 = true;               // Entity can be hit
    ShapeType shapeType1 = ShapeType::RECTANGLE;  // Choose RECTANGLE or CIRCLE
    SDL_Color color1 = { 255, 0, 0, 255 }; // Red color
    SDL_Rect rect1 = { static_cast<int>(initialPosition.x), static_cast<int>(initialPosition.y), 50, 50 }; // Size of rectangle
    SDL_Point center1 = { 0, 0 };
    int radius1 = 0;

    // Define scale factors
    float scaleX = 1.0f;
    //float scaleY = 1.0f;

    updateScaleFactors(scaleX);
    setRenderScale(scaleX, scaleX);

    Entity entity1(initialPosition1, initialVelocity1, mass1, isAffectedByGravity1, isMovable1, isHittable1, shapeType1, color1, rect1, center1, radius1);

    Uint32 lastTime = SDL_GetTicks();

    while (1)
    {
        // Handle input, which might modify the entity's velocity
        doInput(&entity, 50.0f);

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f; // Time in seconds since last frame
        lastTime = currentTime;

        // Apply gravity to the entity
        physicsSystem.applyGravity(entity, deltaTime);
        physicsSystem.applyGravity(entity1, deltaTime);

        // Update the entity's position based on its velocity
        entity.updatePosition(deltaTime);
        entity1.updatePosition(deltaTime);

        // Clear the screen with a blue background
        prepareScene(SDL_Color{ 0, 0, 255, 255 });

        // Draw the entity in its new position
        entity.draw();
        entity1.draw();

        updateScaleFactors(scaleX);
        setRenderScale(scaleX, scaleX);

        auto collision = entity.isColliding(entity1);
        if(collision) {
            break;
        }

        // Present the updated scene
        presentScene();
    }

    clean_up_sdl();

    return 0;
}
