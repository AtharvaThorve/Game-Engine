#include "main.hpp"
#include <memory>
#include <iostream>

int main(int argc, char* argv[])
{
    initSDL();
    // Define default scale factor
    float scale = 1.0f;
    float cached_scale = scale;

    // Gravity on y-axis
    PhysicsSystem physicsSystem(0.0f, 20.0f);

    // Bird entity
    Vector2 birdPosition{ 100, 300 }; // Initial Position
    Vector2 birdVelocity{ 0, 0 };
    float birdMass = 1.0f;
    bool isAffectedByGravity = true;
    bool isMovable = true;
    bool isHittable = true;
    ShapeType birdShapeType = ShapeType::RECTANGLE;
    SDL_Color birdColor = { 255, 0, 0, 255 }; // Red color
    SDL_Rect birdRect = { static_cast<int>(birdPosition.x), static_cast<int>(birdPosition.y), 60, 60 };
    SDL_Point birdCenter = { 0, 0 };
    int birdRadius = 0;

    EntityManager entityManager;

    auto bird = std::make_shared<Entity>(birdPosition, birdVelocity, birdMass, isAffectedByGravity, isMovable, isHittable, birdShapeType, birdColor, birdRect, birdCenter, birdRadius);
    entityManager.addEntity(bird);

    // Left moving pipe entity
    Vector2 pipePosition{ 400, 250 }; // Initial Pipe Position
    Vector2 pipeVelocity{ -100, 0 };  // Move left
    SDL_Color pipeColor = { 0, 255, 0, 255 }; // Green pipe
    SDL_Rect pipeRect = { static_cast<int>(pipePosition.x), static_cast<int>(pipePosition.y), 50, 300 };

    auto pipe = std::make_shared<Entity>(pipePosition, pipeVelocity, birdMass, false, isMovable, isHittable, birdShapeType, pipeColor, pipeRect, birdCenter, birdRadius);
    entityManager.addEntity(pipe);

    //Static Pipe entity
    Vector2 pipe0Position{ 850, 80 }; // Initial Pipe Position
    Vector2 pipe0Velocity{ 0, 0 };  // Static pipe 0 velocity
    SDL_Color pipe0Color = { 139, 79, 57, 255 }; // Brown Brick
    SDL_Rect pipe0Rect = { static_cast<int>(pipe0Position.x), static_cast<int>(pipe0Position.y), 50, 200 };

    auto pipe0 = std::make_shared<Entity>(pipe0Position, pipe0Velocity, birdMass, false, isMovable, isHittable, birdShapeType, pipe0Color, pipe0Rect, birdCenter, birdRadius);
    entityManager.addEntity(pipe0);

    // Define two moving pipes moving in Pattern
    Vector2 pipe1Position{ 600, 0 }; 
    Vector2 pipe2Position{ 600, 400 }; 
    Vector2 pipeVelocityVertical{ 0, 0 }; 
    SDL_Rect pipe1Rect = { static_cast<int>(pipe1Position.x), static_cast<int>(pipe1Position.y), 50, 200 };
    SDL_Rect pipe2Rect = { static_cast<int>(pipe2Position.x), static_cast<int>(pipe2Position.y), 50, 200 };

    auto pipe1 = std::make_shared<Entity>(pipe1Position, pipeVelocityVertical, birdMass, false, isMovable, isHittable, birdShapeType, pipeColor, pipe1Rect, birdCenter, birdRadius);
    auto pipe2 = std::make_shared<Entity>(pipe2Position, pipeVelocityVertical, birdMass, false, isMovable, isHittable, birdShapeType, pipeColor, pipe2Rect, birdCenter, birdRadius);

    // Pattern Movement definition
    MovementPattern pipeMovementPattern;
    pipeMovementPattern.addStep(MovementStep({ 0, -50 }, 2.5f)); // Move up for 2.5 seconds
    pipeMovementPattern.addStep(MovementStep({ 0, 0 }, 1.0f, true));
    pipeMovementPattern.addStep(MovementStep({ 0, 50 }, 2.5f));  // Move down for 2.5 seconds

    pipe1->hasMovementPattern = true;
    pipe1->movementPattern = pipeMovementPattern;

    pipe2->hasMovementPattern = true;
    pipe2->movementPattern = pipeMovementPattern;

    entityManager.addEntity(pipe1);
    entityManager.addEntity(pipe2);

    Uint32 lastTime = SDL_GetTicks();

    while (1)
    {
        // Handle SDL quit event
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                clean_up_sdl();
                return 0;
            }
        }

        // Handler function for defining velocity on x and y-axis
        doInput(bird, 50.0f, 300.0f);

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f; // Time in seconds since last frame
        lastTime = currentTime;

        entityManager.updateEntities(deltaTime, physicsSystem);

        // Boundary check to keep the bird within screen bounds
        if (bird->position.y < 0) {
            bird->position.y = 0;
            bird->velocity.y = 0;
        }
        if (bird->position.y + birdRect.h > SCREEN_HEIGHT) {
            bird->position.y = SCREEN_HEIGHT - birdRect.h;
            bird->velocity.y = 0;
        }

        if (pipe->position.x + pipeRect.w < 0) {
            pipe->position.x = SCREEN_WIDTH;
            pipe->position.y = rand() % 300 + 100;
        }

        // Clear the screen with a sky blue bg
        prepareScene(SDL_Color{ 30, 137, 193, 255 });

        entityManager.drawEntities();

        // Scale handling
        updateScaleFactor(scale);
        if (allowScaling && cached_scale != scale) //Press Right Shift button to toggle the scale
        {
            setRenderScale(scale, scale);
            cached_scale = scale;
        }

        bool collisionDetected = false;

        if (bird->isColliding(*pipe) || bird->isColliding(*pipe0) || bird->isColliding(*pipe1) || bird->isColliding(*pipe2)) {
            collisionDetected = true;
        }

        // Handle collision
        if (collisionDetected) {
            bird->color = SDL_Color{ 255, 255, 0, 255 }; // Change bird color to yellow on collision
            prepareScene(SDL_Color{ 0, 0, 0, 255 }); // Change screen colour to black on collision
            entityManager.drawEntities();
            presentScene();

            SDL_Delay(2000);

            break;
        }
        presentScene();
    }

    clean_up_sdl();

    return 0;
}
