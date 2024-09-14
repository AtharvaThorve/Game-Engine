#include "main.hpp"

int main(int argc, char* argv[])
{
    initSDL();
    // Define scale factors
    float scale = 1.0f;
    float cached_scale = scale;

    PhysicsSystem physicsSystem(0.0f, 13.0f);

    Vector2 userInitialPosition{ (SCREEN_WIDTH/2) - 25, 100 };
    Vector2 userInitialVelocity{ 0, 0 };
    float userMass = 1.0f;
    bool isUserAffectedByGravity = false;
    bool isUserMovable = true;
    bool isUserHittable = true;
    ShapeType userShapeType = ShapeType::RECTANGLE;
    SDL_Color userColor = { 160, 32, 240, 255 };
    SDL_Rect userRect = { static_cast<int>(userInitialPosition.x), static_cast<int>(userInitialPosition.y), 50, 75 };
    SDL_Point userCenter = { 0, 0 };
    int userRadius = 0;

    Vector2 fallingBlockInitialPosition{ 0, 0 };
    Vector2 fallingBlockInitialVelocity{ 0, 0 };
    float fallingBlockMass = 10.0f;
    bool isFallingBlockAffectedByGravity = true;
    bool isFallingBlockMovable = true;
    bool isFallingBlockHittable = true;
    ShapeType fallingBlockShapeType = ShapeType::RECTANGLE;
    SDL_Color fallingBlockColor = { 128, 128, 128, 255 };
    SDL_Rect fallingBlockRect = { static_cast<int>(fallingBlockInitialPosition.x), static_cast<int>(fallingBlockInitialPosition.y), SCREEN_WIDTH, 50 };
    SDL_Point fallingBlockCenter = { 0, 0 };
    int fallingBlockRadius = 0;

    auto userEntity = std::make_shared<Entity>(userInitialPosition, userInitialVelocity, userMass, isUserAffectedByGravity, isUserMovable, isUserHittable, userShapeType, userColor, userRect, userCenter, userRadius);
    auto fallingBlock = std::make_shared<Entity>(fallingBlockInitialPosition, fallingBlockInitialVelocity, fallingBlockMass, isFallingBlockAffectedByGravity, isFallingBlockMovable, isFallingBlockHittable, fallingBlockShapeType, fallingBlockColor, fallingBlockRect, fallingBlockCenter, fallingBlockRadius);

    EntityManager entityManager;
    entityManager.addEntity(userEntity);
    entityManager.addEntity(fallingBlock);

    MovementPattern rightToLeft;
    rightToLeft.addStep(MovementStep({ 50, 0 }, 2.0f)); // Move right for 2 secs at an acceleration of 50
    rightToLeft.addStep(MovementStep({ 0 , 0 }, 1.0f, true)); // Stop for 1 sec
    rightToLeft.addStep(MovementStep({ -50, 0 }, 2.0f)); // Move left for 2 secs at an acceleratopn of 50
    rightToLeft.addStep(MovementStep({ 0 , 0 }, 1.0f, true)); // Stop for 1 sec

    MovementPattern leftToRight;
    leftToRight.addStep(MovementStep({ -50, 0 }, 2.0f)); // Move left for 2 secs at an acceleration of 50
    leftToRight.addStep(MovementStep({ 0 , 0 }, 1.0f, true)); // Stop for 1 sec
    leftToRight.addStep(MovementStep({ 50, 0 }, 2.0f)); // Move right for 2 secs at an acceleratopn of 50
    leftToRight.addStep(MovementStep({ 0 , 0 }, 1.0f, true)); // Stop for 1 sec

    std::vector<std::shared_ptr<Entity>> patternEntities;
    for (int i = 0; i < 20; ++i) {
        Vector2 patternEntityInitialPosition{ i * 100, i % 2 == 0 ? 600 : 300 };
        Vector2 patternEntityInitialVelocity{ 0, 0 };
        float patternEntityMass = 1.0f;
        bool isPatternEntityAffectedByGravity = false;
        bool isPatternEntityMovable = false;
        bool isPatternEntityHittable = true;
        ShapeType patternEntityShapeType = ShapeType::RECTANGLE;
        SDL_Color patternEntityColor = { 255, 0, 0, 255 };
        SDL_Rect patternEntityRect = { static_cast<int>(patternEntityInitialPosition.x), static_cast<int>(patternEntityInitialPosition.y), 50, 50 };
        SDL_Point patternEntityCenter = { 0, 0 };
        int patternEntityRadius = 0;

        auto patternEntity = std::make_shared<Entity>(patternEntityInitialPosition, patternEntityInitialVelocity, patternEntityMass, isPatternEntityAffectedByGravity, isPatternEntityMovable, isPatternEntityHittable, patternEntityShapeType, patternEntityColor, patternEntityRect, patternEntityCenter, patternEntityRadius);

        patternEntity->hasMovementPattern = true;
        patternEntity->movementPattern = i % 2 == 0 ? rightToLeft : leftToRight;
        
        entityManager.addEntity(patternEntity);
        patternEntities.push_back(patternEntity);
    }

    std::vector<float> possibleXCoordinates = { 162.5f, 362.5f, 562.5f, 762.5f, 962.5f, 1162.5f };
    std::random_device rd;
    std::mt19937 gen(rd());
    float yellowEntityX1, yellowEntityX2;
    do {
        std::shuffle(possibleXCoordinates.begin(), possibleXCoordinates.end(), gen);
        yellowEntityX1 = possibleXCoordinates[0];
        yellowEntityX2 = possibleXCoordinates[1];
    } while (std::abs(yellowEntityX1 - yellowEntityX2) >= 600.0f);

    Vector2 yellowEntityPosition1{ yellowEntityX1, 612.5f };
    Vector2 yellowEntityVelocity1{ 0, 0 };
    float yellowEntityMass = 1.0f;
    bool isYellowEntityAffectedByGravity1 = false;
    bool isYellowEntityMovable1 = false;
    bool isYellowEntityHittable1 = true;
    ShapeType yellowEntityShapeType = ShapeType::RECTANGLE;
    SDL_Color yellowEntityColor = { 255, 215, 0, 255 };
    SDL_Rect yellowEntityRect1 = { static_cast<int>(yellowEntityPosition1.x), static_cast<int>(yellowEntityPosition1.y), 25, 25 };
    SDL_Point yellowEntityCenter1 = { 0, 0 };
    int yellowEntityRadius1 = 0;

    auto yellowEntity1 = std::make_shared<Entity>(yellowEntityPosition1, yellowEntityVelocity1, yellowEntityMass, isYellowEntityAffectedByGravity1, isYellowEntityMovable1, isYellowEntityHittable1, yellowEntityShapeType, yellowEntityColor, yellowEntityRect1, yellowEntityCenter1, yellowEntityRadius1);
    entityManager.addEntity(yellowEntity1);

    Vector2 yellowEntityPosition2{ yellowEntityX2, 312.5f };
    Vector2 yellowEntityVelocity2{ 0, 0 };
    bool isYellowEntityAffectedByGravity2 = false;
    bool isYellowEntityMovable2 = false;
    bool isYellowEntityHittable2 = true;
    SDL_Rect yellowEntityRect2 = { static_cast<int>(yellowEntityPosition2.x), static_cast<int>(yellowEntityPosition2.y), 25, 25 };

    auto yellowEntity2 = std::make_shared<Entity>(yellowEntityPosition2, yellowEntityVelocity2, yellowEntityMass, isYellowEntityAffectedByGravity2, isYellowEntityMovable2, isYellowEntityHittable2, yellowEntityShapeType, yellowEntityColor, yellowEntityRect2, yellowEntityCenter1, yellowEntityRadius1);
    entityManager.addEntity(yellowEntity2);

    Vector2 whiteEntityPosition{ 0, SCREEN_HEIGHT - 50 };
    Vector2 whiteEntityVelocity{ 0, 0 };
    float whiteEntityMass = 1.0f;
    bool isWhiteEntityAffectedByGravity = false;
    bool isWhiteEntityMovable = false;
    bool isWhiteEntityHittable = true;
    ShapeType whiteEntityShapeType = ShapeType::RECTANGLE;
    SDL_Color whiteEntityColor = { 255, 255, 255, 255 };
    SDL_Rect whiteEntityRect = { static_cast<int>(whiteEntityPosition.x), static_cast<int>(whiteEntityPosition.y), SCREEN_WIDTH, 50 };
    SDL_Point whiteEntityCenter = { 0, 0 };
    int whiteEntityRadius = 0;

    auto whiteEntity = std::make_shared<Entity>(whiteEntityPosition, whiteEntityVelocity, whiteEntityMass, isWhiteEntityAffectedByGravity, isWhiteEntityMovable, isWhiteEntityHittable, whiteEntityShapeType, whiteEntityColor, whiteEntityRect, whiteEntityCenter, whiteEntityRadius);

    entityManager.addEntity(whiteEntity);

    Uint32 lastTime = SDL_GetTicks();

    int coinsColected = 0;
    bool hitYellow1 = false;
    bool hitYellow2 = false;

    while (1)
    {
        // Handle input, which might modify the entity's velocity
        doInput(userEntity, 80.0f, 80.0f);

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f; // Time in seconds since last frame
        lastTime = currentTime;

        entityManager.updateEntities(deltaTime, physicsSystem);

        SDL_Color backgroundColor = { 0, 0, 255, 255 };

        if (userEntity->isColliding(*yellowEntity1)) {
            entityManager.removeEntity(yellowEntity1);
            
            if (coinsColected < 2 && !hitYellow1) {
                coinsColected++;
                hitYellow1 = true;
            }
        }

        if (userEntity->isColliding(*yellowEntity2)) {
            entityManager.removeEntity(yellowEntity2);
            if (coinsColected < 2 && !hitYellow2) {
                coinsColected++;
                hitYellow2 = true;
            }
        }

        prepareScene(backgroundColor);

        entityManager.drawEntities();

        updateScaleFactor(scale);
        if (allowScaling && cached_scale != scale)
        {
            setRenderScale(scale, scale);
            cached_scale = scale;
        }
 
        auto gameOver = userEntity->isColliding(*fallingBlock);

        for (auto entity : patternEntities) {
            if (userEntity->isColliding(*entity)) {
                gameOver = true;
                break;
            }
        }
        if(gameOver) {
            break;
        }

        
        if (userEntity->isColliding(*whiteEntity) && coinsColected == 2) {
            backgroundColor = { 0, 255, 0, 255 }; // Turn the screen green
            prepareScene(backgroundColor);
            entityManager.drawEntities();
            presentScene();
            SDL_Delay(1500);
            break;
        }

        // Present the updated scene
        presentScene();
    }

    clean_up_sdl();

    return 0;
}
