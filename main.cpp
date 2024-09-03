#include "main.hpp"
#include <memory>
#include <iostream>

int main(int argc, char* argv[])
{
    initSDL();
    // Define scale factors
    float scale = 1.0f;
    float cached_scale = scale;

    PhysicsSystem physicsSystem(0.0f, 9.8f);

    Vector2 initialPosition{ 100, 100 };
    Vector2 initialVelocity{ 0, 0 };
    Vector2 inputInitialVelocity{ 0, 0 };
    float mass = 1.0f;
    bool isAffectedByGravity = true;
    bool isMovable = true;
    bool isHittable = true;
    ShapeType shapeType = ShapeType::RECTANGLE;
    SDL_Color color = { 255, 0, 0, 255 };
    SDL_Rect rect = { static_cast<int>(initialPosition.x), static_cast<int>(initialPosition.y), 50, 50 };
    SDL_Point center = { 0, 0 };
    int radius = 0;

    Vector2 initialPosition1{ 200, 100 };
    SDL_Rect rect1 = { static_cast<int>(initialPosition1.x), static_cast<int>(initialPosition1.y), 50, 50 };

    EntityManager entityManager;

    auto entity = std::make_shared<Entity>(initialPosition, initialVelocity, inputInitialVelocity, mass, isAffectedByGravity, isMovable, isHittable, shapeType, color, rect, center, radius);
    auto entity1 = std::make_shared<Entity>(initialPosition1, initialVelocity, inputInitialVelocity, mass, isAffectedByGravity, isMovable, isHittable, shapeType, color, rect1, center, radius);

    Vector2 initialPosition2{ 300, 300 };
    SDL_Rect rect2 = { static_cast<int>(initialPosition2.x), static_cast<int>(initialPosition2.y), 50, 50 };
    auto patternEntity = std::make_shared<Entity>(initialPosition2, initialVelocity, inputInitialVelocity, mass, !isAffectedByGravity, isMovable, isHittable, shapeType, color, rect2, center, radius);

    Vector2 initialPosition3{ 600, 100 };
    SDL_Rect rect3 = { static_cast<int>(initialPosition3.x), static_cast<int>(initialPosition3.y), 50, 50 };
    auto patternEntity1 = std::make_shared<Entity>(initialPosition3, initialVelocity, inputInitialVelocity, mass, isAffectedByGravity, isMovable, isHittable, shapeType, color, rect3, center, radius);

    MovementPattern pattern;
    pattern.addStep(MovementStep({ 50, 50 }, 2.0f)); // Move right for 2 secs at an acceleration of 50
    pattern.addStep(MovementStep({ 0 , 0 }, 1.0f, true)); // Stop for 1 sec
    pattern.addStep(MovementStep({ -50, -50 }, 2.0f)); // Move left for 2 secs at an acceleratopn of 50
    pattern.addStep(MovementStep({ 0 , 0 }, 1.0f, true)); // Stop for 1 sec
    patternEntity->hasMovementPattern = true;
    patternEntity->movementPattern = pattern;

    patternEntity1->hasMovementPattern = true;
    patternEntity1->movementPattern = pattern;

    entityManager.addEntity(entity);
    entityManager.addEntity(entity1);
    entityManager.addEntity(patternEntity);
    entityManager.addEntity(patternEntity1);

    Uint32 lastTime = SDL_GetTicks();

    while (1)
    {
        // Handle input, which might modify the entity's velocity
        doInput(entity, 50.0f, 100.0f);

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f; // Time in seconds since last frame
        lastTime = currentTime;

        //Create a bounding box
        int boxX = 100, boxY = 100, boxWidth = 600, boxHeight = 400;
        BoundingBox boundingBox(boxX, boxY, boxWidth, boxHeight);

        entityManager.updateEntities(deltaTime, physicsSystem, boundingBox);

        // Clear the screen with a blue background
        prepareScene(SDL_Color{ 0, 0, 255, 255 });

        entityManager.drawEntities();
        
        //Bounding box Debug
        SDL_Rect boundingRect = boundingBox.getRect();
        SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255); 
        SDL_RenderDrawRect(app->renderer, &boundingRect);

        updateScaleFactor(scale);
        if (allowScaling && cached_scale != scale)
        {
            setRenderScale(scale, scale);
            cached_scale = scale;
        }
 
        auto collision = entity->isColliding(*entity1);
        if(collision) {
            break;
        }

        // Present the updated scene
        presentScene();
    }

    clean_up_sdl();

    return 0;
}
