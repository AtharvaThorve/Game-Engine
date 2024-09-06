#include "rushil_game1.hpp"

int rushil_game1()
{
    // Define scale factors
    float scale = 1.0f;
    float cached_scale = scale;

    // Physics system (gravity)
    PhysicsSystem physicsSystem(-3.0f, 3.0f);

    // Player entity
    Vector2 playerPosition{ 50, SCREEN_HEIGHT - 50 }; // Start position of the player on the ground
    Vector2 playerVelocity{ 0, 0 };
    float playerMass = 1.0f;
    bool isAffectedByGravity = true;
    bool isMovable = true;
    bool isHittable = true;
    ShapeType playerShapeType = ShapeType::RECTANGLE;
    SDL_Color playerColor = { 255, 255, 255, 255 }; // White color
    SDL_Rect playerRect = { static_cast<int>(playerPosition.x), static_cast<int>(playerPosition.y), 50, 50 };
    SDL_Point playerCenter = { 0, 0 };
    int playerRadius = 0;

    EntityManager entityManager;

    auto player = std::make_shared<Entity>(playerPosition, playerVelocity, playerMass, isAffectedByGravity, isMovable, isHittable, playerShapeType, playerColor, playerRect, playerCenter, playerRadius);
    entityManager.addEntity(player);

    // Obstacle entities (falling from above)
    Vector2 obstaclePosition{ 400, -50 };
    Vector2 obstacleVelocity{ 0, 200 };
    SDL_Color obstacleColor = { 255, 0, 0, 255 }; // Red color for the obstacles
    SDL_Rect obstacleRect = { static_cast<int>(obstaclePosition.x), static_cast<int>(obstaclePosition.y), 100, 20 };

    auto obstacle = std::make_shared<Entity>(obstaclePosition, obstacleVelocity, playerMass, false, isMovable, isHittable, playerShapeType, obstacleColor, obstacleRect, playerCenter, playerRadius);
    entityManager.addEntity(obstacle);

    Vector2 obstacle2Position{ 600, -50 };
    SDL_Rect obstacle2Rect = { static_cast<int>(obstacle2Position.x), static_cast<int>(obstacle2Position.y), 100, 20 };

    auto obstacle2 = std::make_shared<Entity>(obstacle2Position, obstacleVelocity, playerMass, false, isMovable, isHittable, playerShapeType, obstacleColor, obstacle2Rect, playerCenter, playerRadius);
    entityManager.addEntity(obstacle2);

    Vector2 obstacle3Position{ 800, -50 };
    SDL_Rect obstacle3Rect = { static_cast<int>(obstacle3Position.x), static_cast<int>(obstacle3Position.y), 100, 20 };

    auto obstacle3 = std::make_shared<Entity>(obstacle3Position, obstacleVelocity, playerMass, false, isMovable, isHittable, playerShapeType, obstacleColor, obstacle3Rect, playerCenter, playerRadius);
    entityManager.addEntity(obstacle3);

    // Right-hand side vertical rectangle
    Vector2 rightRectPosition{ SCREEN_WIDTH - 50 };
    SDL_Rect rightRectSize = { static_cast<int>(rightRectPosition.x), static_cast<int>(rightRectPosition.y), 50, SCREEN_HEIGHT }; // Full screen height

    SDL_Color rightRectColor = { 0, 255, 0, 255 }; // Green color for the right rectangle
    auto rightRect = std::make_shared<Entity>(rightRectPosition, Vector2{ 0, 0 }, playerMass, false, false, false, playerShapeType, rightRectColor, rightRectSize, playerCenter, playerRadius);
    entityManager.addEntity(rightRect);

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

        doInput(player, 200.0f, 100.0f);

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        entityManager.updateEntities(deltaTime, physicsSystem);

        // Boundary check to keep the player within screen bounds
        if (player->position.x < 0) {
            player->position.x = 0;
        }
        if (player->position.x + playerRect.w > SCREEN_WIDTH) {
            player->position.x = (float)(SCREEN_WIDTH - playerRect.w);
        }

        if (player->position.y < 0) {
            player->position.y = 0;
        }
        if (player->position.y + playerRect.h > SCREEN_HEIGHT) {
            player->position.y = (float)(SCREEN_HEIGHT - playerRect.h);
        }

        for (auto& obstacle : { obstacle, obstacle2, obstacle3 }) {
            if (obstacle->position.y > SCREEN_HEIGHT) {
                obstacle->position.x = (float)(rand() % (SCREEN_WIDTH - obstacleRect.w));
                obstacle->position.y = (float) - obstacleRect.h;
            }
        }

        prepareScene(SDL_Color{ 0, 0, 0, 255 });

        entityManager.drawEntities();

        // Scale handling
        updateScaleFactor(scale);
        if (allowScaling && cached_scale != scale)
        {
            setRenderScale(scale, scale);
            cached_scale = scale;
        }

        bool collisionDetected = (player->isColliding(*obstacle) || player->isColliding(*obstacle2) || player->isColliding(*obstacle3));

        if (player->isColliding(*rightRect)) {
            std::cout << "YOU WON!!!!\n";
            break;
        }

        // Handle collision
        if (collisionDetected) {
            player->color = SDL_Color{ 255, 0, 0, 255 };
            prepareScene(SDL_Color{ 180, 0, 255, 255 });
            entityManager.drawEntities();
            presentScene();

            SDL_Delay(2500);
            std::cout << "YOU LOSE!\n";
            break;
        }
        presentScene();
    }
    return 0;
}
