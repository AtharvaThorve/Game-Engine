#include "main.hpp"

Timeline globalTimeline(nullptr, 2);
PhysicsSystem physicsSystem(0.0f, 150.0f);

// Assuming Server and Client are properly defined classes with start() method
void runServer(Server &server)
{
    server.start();
}

void runClient(EntityManager &entityManager, EntityManager &clientEntityManager)
{
    Client client(entityManager, clientEntityManager);
    client.connectRequester("tcp://172.30.115.140", 5556);
    client.connectPusher("tcp://172.30.115.140", 5557);
    client.connectSubscriber("tcp://172.30.115.140", 5558);
    client.connectServer();
    client.start();
}

void runP2PClient(EntityManager &entityManager, EntityManager &clientEntityManager)
{
    Client client(entityManager, clientEntityManager);
    client.connectRequester("tcp://192.168.1.192", 5555);       // Put the server address and port
    client.connectSubscriber("tcp://192.168.1.192", 5556);      // Put the server address and port
    client.bindPeerPublisher("tcp://*", 5558);                  // Bind the client publisher to a port
    client.connectPeerSubscriber1("tcp://192.168.1.192", 5557); // Put peer 1's address and the port they are bounded on
    client.connectPeerSubscriber2("tcp://192.168.1.239", 5559); // Put peer 2's address and the port they are bounded on
    client.connectServer(true);
    client.start(true);
}

void applyGravityOnEntities(PhysicsSystem &physicsSystem, EntityManager &entityManager)
{
    while (1)
    {
        entityManager.applyGravityOnEntities(physicsSystem);
    }
}

void doServerEntities(Server &server)
{
    Vector2 initialPosition2{300, 300};
    Vector2 dimensions{50, 50};
    SDL_Color color = {255, 0, 0, 255};
    auto patternEntity = std::make_shared<Entity>(initialPosition2, dimensions, color, &globalTimeline, 1);

    MovementPattern pattern;
    pattern.addSteps(
        MovementStep({50, 50}, 2.0f),
        MovementStep({0, 0}, 1.0f, true),
        MovementStep({50, -50}, 2.0f),
        MovementStep({0, 0}, 1.0f, true),
        MovementStep({-50, 50}, 2.0f),
        MovementStep({0, 0}, 1.0f, true),
        MovementStep({-50, -50}, 2.0f),
        MovementStep({0, 0}, 1.0f, true));

    patternEntity->hasMovementPattern = true;
    patternEntity->movementPattern = pattern;

    EntityManager serverEntityManager;
    serverEntityManager.addEntities(patternEntity);

    std::thread gravityThread(applyGravityOnEntities, std::ref(physicsSystem), std::ref(serverEntityManager));
    gravityThread.detach();

    while (true)
    {
        serverEntityManager.updateEntityDeltaTime();
        serverEntityManager.updateMovementPatternEntities();
        serverEntityManager.updateEntities();
        server.updateClientEntityMap(serverEntityManager);
    }
}

void doClientGame(bool isP2P = false)
{
    initSDL();
    float scale = 1.0f;
    float cached_scale = scale;

    // Player setup
    Vector2 playerPosition{100, 400};
    Vector2 playerDimensions{40, 40};
    SDL_Color playerColor = {238,28,37, 255};  // Red player
    
    auto player = std::make_shared<Entity>(playerPosition, playerDimensions, playerColor, &globalTimeline, 2);
    player->maxVelocity = Vector2{300, 400};
    player->isMovable = true;
    player->isHittable = true;
    player->isAffectedByGravity = true;
    
    Vector2 player2Position{700, 400};
    Vector2 player2Dimensions{40, 40};
    SDL_Color player2Color = {0, 100, 255, 255};
    
    auto player2 = std::make_shared<Entity>(player2Position, player2Dimensions, player2Color, &globalTimeline, 2);
    
    EntityManager playerEntityManager;
    playerEntityManager.addEntities(player);

    EntityManager entityManager;
    EntityManager clientEntityManager;

    // Extended ground platforms
    SDL_Color platformColor = {34, 139, 34, 255};  // Dark green
    // Made ground platforms longer and added more sections
    auto ground1 = std::make_shared<Entity>(Vector2{0, 550}, Vector2{1200, 50}, platformColor, &globalTimeline, 2);
    auto ground2 = std::make_shared<Entity>(Vector2{1400, 550}, Vector2{1200, 50}, platformColor, &globalTimeline, 2);
    auto ground3 = std::make_shared<Entity>(Vector2{2800, 550}, Vector2{1200, 50}, platformColor, &globalTimeline, 2);
    ground1->isHittable = true;
    ground2->isHittable = true;
    ground3->isHittable = true;

    // Floating platforms - Added more with varying heights
    auto platform1 = std::make_shared<Entity>(Vector2{300, 400}, Vector2{200, 30}, platformColor, &globalTimeline, 2);
    auto platform2 = std::make_shared<Entity>(Vector2{600, 300}, Vector2{200, 30}, platformColor, &globalTimeline, 2);
    auto platform3 = std::make_shared<Entity>(Vector2{900, 180}, Vector2{200, 30}, platformColor, &globalTimeline, 2);
    auto platform4 = std::make_shared<Entity>(Vector2{1300, 350}, Vector2{200, 30}, platformColor, &globalTimeline, 2);
    auto platform5 = std::make_shared<Entity>(Vector2{1700, 280}, Vector2{200, 30}, platformColor, &globalTimeline, 2);
    auto platform6 = std::make_shared<Entity>(Vector2{2100, 320}, Vector2{200, 30}, platformColor, &globalTimeline, 2);
    auto platform7 = std::make_shared<Entity>(Vector2{2500, 250}, Vector2{200, 30}, platformColor, &globalTimeline, 2);
    auto platform8 = std::make_shared<Entity>(Vector2{2900, 400}, Vector2{200, 30}, platformColor, &globalTimeline, 2);

    platform1->isHittable = true;
    platform2->isHittable = true;
    platform3->isHittable = true;
    platform4->isHittable = true;
    platform5->isHittable = true;
    platform6->isHittable = true;
    platform7->isHittable = true;
    platform8->isHittable = true;

    // Moving platforms - Added more with different patterns
    SDL_Color movingPlatformColor = {137,76,47, 255};  // Medium purple
    
    auto movingPlatform1 = std::make_shared<Entity>(Vector2{400, 200}, Vector2{100, 20}, 
        movingPlatformColor, &globalTimeline, 2);
    movingPlatform1->maxVelocity = Vector2{100, 100};
    movingPlatform1->isHittable = true;
    movingPlatform1->hasMovementPattern = true;
    
    MovementPattern pattern1;
    pattern1.addSteps(
        MovementStep({200, 0}, 3.0f),
        MovementStep({0, 0}, 0.5f, true),
        MovementStep({-200, 0}, 3.0f),
        MovementStep({0, 0}, 0.5f, true));
    movingPlatform1->movementPattern = pattern1;

    auto movingPlatform2 = std::make_shared<Entity>(Vector2{1000, 350}, Vector2{100, 20}, 
        movingPlatformColor, &globalTimeline, 2);
    MovementPattern pattern2;
    pattern2.addSteps(
        MovementStep({0, 150}, 2.0f),
        MovementStep({0, 0}, 0.5f, true),
        MovementStep({0, -150}, 2.0f),
        MovementStep({0, 0}, 0.5f, true));
    movingPlatform2->movementPattern = pattern2;
    movingPlatform2->maxVelocity = Vector2{100, 100};
    movingPlatform2->isHittable = true;
    movingPlatform2->hasMovementPattern = true;

    auto movingPlatform3 = std::make_shared<Entity>(Vector2{1500, 300}, Vector2{100, 20}, 
        movingPlatformColor, &globalTimeline, 2);
    MovementPattern pattern3;
    pattern3.addSteps(
        MovementStep({150, 150}, 3.0f),
        MovementStep({0, 0}, 0.5f, true),
        MovementStep({-150, -150}, 3.0f),
        MovementStep({0, 0}, 0.5f, true));
    movingPlatform3->movementPattern = pattern3;
    movingPlatform3->maxVelocity = Vector2{100, 100};
    movingPlatform3->isHittable = true;
    movingPlatform3->hasMovementPattern = true;

    auto movingPlatform4 = std::make_shared<Entity>(Vector2{2200, 250}, Vector2{100, 20}, 
        movingPlatformColor, &globalTimeline, 2);
    MovementPattern pattern4;
    pattern4.addSteps(
        MovementStep({200, 0}, 2.0f),
        MovementStep({0, 100}, 2.0f),
        MovementStep({-200, 0}, 2.0f),
        MovementStep({0, -100}, 2.0f));
    movingPlatform4->movementPattern = pattern4;
    movingPlatform4->maxVelocity = Vector2{100, 100};
    movingPlatform4->isHittable = true;
    movingPlatform4->hasMovementPattern = true;

    // Collectibles (coins) - Added more throughout the level
    SDL_Color coinColor = {255, 215, 0, 255};  // Gold
    auto coin1 = std::make_shared<Entity>(Vector2{300, 350}, Vector2{20, 20}, coinColor, &globalTimeline, 1);
    auto coin2 = std::make_shared<Entity>(Vector2{600, 250}, Vector2{20, 20}, coinColor, &globalTimeline, 1);
    auto coin3 = std::make_shared<Entity>(Vector2{900, 140}, Vector2{20, 20}, coinColor, &globalTimeline, 1);
    auto coin4 = std::make_shared<Entity>(Vector2{1300, 300}, Vector2{20, 20}, coinColor, &globalTimeline, 1);
    auto coin5 = std::make_shared<Entity>(Vector2{1700, 230}, Vector2{20, 20}, coinColor, &globalTimeline, 1);
    auto coin6 = std::make_shared<Entity>(Vector2{2100, 270}, Vector2{20, 20}, coinColor, &globalTimeline, 1);
    auto coin7 = std::make_shared<Entity>(Vector2{2500, 200}, Vector2{20, 20}, coinColor, &globalTimeline, 1);
    auto coin8 = std::make_shared<Entity>(Vector2{2900, 350}, Vector2{20, 20}, coinColor, &globalTimeline, 1);

    // Death zones (including gaps and final death zone)
    SDL_Color lavaColor = {255, 20, 0, 255};
    auto lava1 = std::make_shared<Entity>(Vector2{1200, 580}, Vector2{200, 20}, lavaColor, &globalTimeline, 2);
    auto lava2 = std::make_shared<Entity>(Vector2{2600, 580}, Vector2{200, 20}, lavaColor, &globalTimeline, 2);
    // Final death zone at the end
    auto finalLava = std::make_shared<Entity>(Vector2{3500, 0}, Vector2{100, 600}, lavaColor, &globalTimeline, 2);

    // Add all entities to manager
    entityManager.addEntities(
        ground1, ground2, ground3,
        platform1, platform2, platform3, platform4, platform5, platform6, platform7, platform8,
        movingPlatform1, movingPlatform2, movingPlatform3, movingPlatform4,
        coin1, coin2, coin3, coin4, coin5, coin6, coin7, coin8
    );
    
    // Add death zones
    entityManager.addDeathZone(lava1);
    entityManager.addDeathZone(lava2);
    entityManager.addDeathZone(finalLava);

    // Spawn points - Added more throughout the level
    auto spawnPoint1 = std::make_shared<Entity>(Vector2{100, 400}, Vector2{1, 1}, 
        SDL_Color{0, 0, 0, 0}, &globalTimeline, 1);
    auto spawnPoint2 = std::make_shared<Entity>(Vector2{1000, 200}, Vector2{1, 1}, 
        SDL_Color{0, 0, 0, 0}, &globalTimeline, 1);
    auto spawnPoint3 = std::make_shared<Entity>(Vector2{2000, 200}, Vector2{1, 1}, 
        SDL_Color{0, 0, 0, 0}, &globalTimeline, 1);
    entityManager.addSpawnPoint(spawnPoint1);
    entityManager.addSpawnPoint(spawnPoint2);
    entityManager.addSpawnPoint(spawnPoint3);

    // Increased world size to accommodate the longer level
    int worldWidth = 4000;  // Extended world width
    int worldHeight = 800;


    Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Network and gravity threads
    std::thread networkThread(runClient, std::ref(playerEntityManager),
                            std::ref(clientEntityManager));
    std::thread gravityThread(applyGravityOnEntities, std::ref(physicsSystem),
                            std::ref(playerEntityManager));


    while (true)
    {
        doInput(player, &globalTimeline, 200.0f);  // Increased movement speed
        playerEntityManager.updateEntityDeltaTime();
        playerEntityManager.updateMovementPatternEntities();
        playerEntityManager.updateEntities();

        entityManager.updateEntityDeltaTime();
        entityManager.updateMovementPatternEntities();
        entityManager.updateEntities();

        camera.update(*player, worldWidth, worldHeight);

        prepareScene(SDL_Color{25, 25, 112, 255});  // Midnight blue background
        
        playerEntityManager.drawEntities(camera.position.x, camera.position.y);
        entityManager.drawEntities(camera.position.x, camera.position.y);
        clientEntityManager.drawEntities(camera.position.x, camera.position.y);

        updateScaleFactor(scale);
        if (allowScaling && cached_scale != scale)
        {
            setRenderScale(scale, scale);
            cached_scale = scale;
        }

        // Check death and respawn
        if (entityManager.checkPlayerDeath(player)) {
            entityManager.respawn(player);
        }

        // Collision handling for all platforms and collectibles
        if (player->isColliding(*ground1))
        {
            collision_utils::handlePlatformCollision(player, ground1);
        }
        else if (player->isColliding(*ground2))
        {
            collision_utils::handlePlatformCollision(player, ground2);
        }
        else if (player->isColliding(*ground3))
        {
            collision_utils::handlePlatformCollision(player, ground3);
        }
        else if (player->isColliding(*platform1))
        {
            collision_utils::handlePlatformCollision(player, platform1);
        }
        else if (player->isColliding(*platform2))
        {
            collision_utils::handlePlatformCollision(player, platform2);
        }
        else if (player->isColliding(*platform3))
        {
            collision_utils::handlePlatformCollision(player, platform3);
        }
        else if (player->isColliding(*platform4))
        {
            collision_utils::handlePlatformCollision(player, platform4);
        }
        else if (player->isColliding(*platform5))
        {
            collision_utils::handlePlatformCollision(player, platform5);
        }
        else if (player->isColliding(*platform6))
        {
            collision_utils::handlePlatformCollision(player, platform6);
        }
        else if (player->isColliding(*platform7))
        {
            collision_utils::handlePlatformCollision(player, platform7);
        }
        else if (player->isColliding(*platform8))
        {
            collision_utils::handlePlatformCollision(player, platform8);
        }
        else if (player->isColliding(*movingPlatform1))
        {
            collision_utils::handlePlatformCollision(player, movingPlatform1);
        }
        else if (player->isColliding(*movingPlatform2))
        {
            collision_utils::handlePlatformCollision(player, movingPlatform2);
        }
        else if (player->isColliding(*movingPlatform3))
        {
            collision_utils::handlePlatformCollision(player, movingPlatform3);
        }
        else if (player->isColliding(*movingPlatform4))
        {
            collision_utils::handlePlatformCollision(player, movingPlatform4);
        }

        // Coin collisions - these don't need to be in the else-if chain since they don't affect movement
        if (player->isColliding(*coin1))
        {
            coin1->color = SDL_Color{25, 25, 112, 255};  // Changed to match new background color
        }
        if (player->isColliding(*coin2))
        {
            coin2->color = SDL_Color{25, 25, 112, 255};
        }
        if (player->isColliding(*coin3))
        {
            coin3->color = SDL_Color{25, 25, 112, 255};
        }
        if (player->isColliding(*coin4))
        {
            coin4->color = SDL_Color{25, 25, 112, 255};
        }
        if (player->isColliding(*coin5))
        {
            coin5->color = SDL_Color{25, 25, 112, 255};
        }
        if (player->isColliding(*coin6))
        {
            coin6->color = SDL_Color{25, 25, 112, 255};
        }
        if (player->isColliding(*coin7))
        {
            coin7->color = SDL_Color{25, 25, 112, 255};
        }
        if (player->isColliding(*coin8))
        {
            coin8->color = SDL_Color{25, 25, 112, 255};
        }


        presentScene();
    }
    networkThread.join();
    gravityThread.join();
    clean_up_sdl();
}

int main(int argc, char *argv[])
{

    if (argc < 2 || argc > 3)
    {
        std::cerr << "Usage: " << argv[0] << " [server/client] [optional: client_id] or [server client/client server]" << std::endl;
        return 1;
    }

    std::string mode1 = argv[1];
    std::string mode2 = (argc == 3) ? argv[2] : "";

    if (mode1 == "server" && mode2.empty())
    {

        Server server;
        server.bindResponder("tcp://*", 5556);
        server.bindPuller("tcp://*", 5557);
        server.bindPublisher("tcp://*", 5558);

        std::cout << "Starting server..." << std::endl;
        std::thread serverThread(runServer, std::ref(server));

        doServerEntities(std::ref(server));

        serverThread.join();
    }
    else if (mode1 == "client" && mode2.empty())
    {
        doClientGame();
    }
    else if ((mode1 == "client" && mode2 == "P2P") || (mode1 == "P2P" && mode2 == "client"))
    {
        doClientGame(true);
    }
    else if ((mode1 == "server" && mode2 == "client") || (mode1 == "client" && mode2 == "server"))
    {
        Server server;
        server.bindResponder("tcp://*", 5555);
        server.bindPublisher("tcp://*", 5556);

        std::cout << "Starting server..." << std::endl;
        std::thread serverThread(runServer, std::ref(server));

        std::thread serverEntities(doServerEntities, std::ref(server));

        doClientGame(true);
        serverThread.join();
        serverEntities.join();
    }
    else
    {
        std::cerr << "Invalid mode. Use 'server', 'client', or both 'server client'." << std::endl;
        return 1;
    }

    return 0;
}