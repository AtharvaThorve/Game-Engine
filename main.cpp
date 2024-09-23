#include "main.hpp"
#include <memory>
#include <iostream>

// Assuming Server and Client are properly defined classes with start() method
void runServer() {
    Server server;
    server.bindResponder("tcp://*", 5556);
    server.bindPuller("tcp://*", 5557);
    server.bindPublisher("tcp://*", 5558);
    server.start();
}

void runClient(EntityManager& entityManager) {
    Client client(entityManager);
    client.connectRequester("tcp://localhost", 5556);
    client.connectPusher("tcp://localhost", 5557);
    client.connectSubscriber("tcp://localhost", 5558);
    client.connectServer();
    client.start();
}

void updatePatternEntitiesThread(EntityManager& entityManager) {
    while (1) {
        entityManager.updateMovementPatternEntities();
    }
}

int main(int argc, char* argv[])
{

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [server/client] [optional: client_id]" << std::endl;
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "server") {
        std::cout << "Starting server..." << std::endl;
        runServer();
    }
    else if (mode == "client") {

        initSDL();
        // Define scale factors
        float scale = 1.0f;
        float cached_scale = scale;

        EntityManager entityManager;
        Timeline globalTimeline(nullptr, 2);
        PhysicsSystem physicsSystem(0.0f, 10.0f);

        Vector2 initialPosition{ 100, 100 };
        Vector2 initialVelocity{ 0, 0 };
        Vector2 inputInitialVelocity{ 0, 0 };
        Vector2 initialAcceleration{0, 0};
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


        auto entity = std::make_shared<Entity>(initialPosition, initialVelocity,initialAcceleration, mass, isAffectedByGravity, isMovable, isHittable, shapeType, color, rect, center, radius, &globalTimeline, 2);
        auto entity1 = std::make_shared<Entity>(initialPosition1, initialVelocity,initialAcceleration, mass, isAffectedByGravity, isMovable, isHittable, shapeType, color, rect1, center, radius, &globalTimeline, 4);

        Vector2 initialPosition2{ 300, 300 };
        SDL_Rect rect2 = { static_cast<int>(initialPosition2.x), static_cast<int>(initialPosition2.y), 50, 50 };
        auto patternEntity = std::make_shared<Entity>(initialPosition2, initialVelocity, initialAcceleration, mass, !isAffectedByGravity, isMovable, isHittable, shapeType, color, rect2, center, radius, &globalTimeline, 1);

        //Vector2 initialPosition3{ 600, 100 };
        //SDL_Rect rect3 = { static_cast<int>(initialPosition3.x), static_cast<int>(initialPosition3.y), 50, 50 };
        //auto patternEntity1 = std::make_shared<Entity>(initialPosition3, initialVelocity, mass, isAffectedByGravity, isMovable, isHittable, shapeType, color, rect3, center, radius);

        MovementPattern pattern;
        pattern.addStep(MovementStep({ 50, 50 }, 2.0f)); // Move diagonally right and down for 2 secs at an acceleration of 50
        pattern.addStep(MovementStep({ 0 , 0 }, 1.0f, true)); // Stop for 1 sec
        pattern.addStep(MovementStep({ 50, -50 }, 2.0f));
        pattern.addStep(MovementStep({ 0 , 0 }, 1.0f, true));
        pattern.addStep(MovementStep({ -50 , 50 }, 2.0f));
        pattern.addStep(MovementStep({ 0 , 0 }, 1.0f, true));
        pattern.addStep(MovementStep({ -50, -50 }, 2.0f)); // Move diagonally left and up for 2 secs at an acceleratopn of 50
        pattern.addStep(MovementStep({ 0 , 0 }, 1.0f, true)); // Stop for 1 sec

        patternEntity->hasMovementPattern = true;
        patternEntity->movementPattern = pattern;

        //patternEntity1->hasMovementPattern = true;
        //patternEntity1->movementPattern = pattern;

        entityManager.addEntity(entity);
        //entityManager.addEntity(entity1);
        entityManager.addEntity(patternEntity);
        //entityManager.addEntity(patternEntity1);

        std::thread clientThread1(runClient, std::ref(entityManager));

        //std::thread updateThread(updatePatternEntitiesThread, std::ref(entityManager));

        int64_t lastUpdateTime = globalTimeline.getTime();
                
        while (1)
        {
            doInput(entity, &globalTimeline, 50.0f);

            int64_t currentTime = globalTimeline.getTime();
            float globalDeltaTime = (currentTime - lastUpdateTime) / NANOSECONDS_TO_SECONDS; // nanosecond to sec
            lastUpdateTime = currentTime;

            
            entityManager.updateEntityDeltaTime();
            entityManager.applyGravityOnEntities(globalDeltaTime, physicsSystem);
            entityManager.updateMovementPatternEntities();
            entityManager.updateEntities();
            
            // Clear the screen with a blue background
            prepareScene(SDL_Color{ 0, 0, 255, 255 });

            entityManager.drawEntities();

            updateScaleFactor(scale);
            if (allowScaling && cached_scale != scale)
            {
                setRenderScale(scale, scale);
                cached_scale = scale;
            }

            auto collision = entity->isColliding(*entity1);
            if (collision) {
                break;
            }

            // Present the updated scene
            presentScene();
        }

        clean_up_sdl();
        clientThread1.join();
    }
    else {
        std::cerr << "Invalid mode. Use 'server' or 'client'." << std::endl;
        return 1;
    }

    return 0;
}