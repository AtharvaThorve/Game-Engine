#include "main.hpp"

Timeline globalTimeline(nullptr, 2);
PhysicsSystem physicsSystem(0.0f, 150.0f);

// Assuming Server and Client are properly defined classes with start() method
void runServer(Server &server) { server.start(); }

void runClient(EntityManager &entityManager,
               EntityManager &clientEntityManager) {
  Client client(entityManager, clientEntityManager);
  client.connectRequester("tcp://172.21.100.173", 5556);
  client.connectPusher("tcp://172.21.100.173", 5557);
  client.connectSubscriber("tcp://172.21.100.173", 5558);
  client.connectServer();
  client.start();
}

void runP2PClient(EntityManager &entityManager,
                  EntityManager &clientEntityManager) {
  Client client(entityManager, clientEntityManager);
  client.connectRequester("tcp://192.168.1.192",
                          5555); // Put the server address and port
  client.connectSubscriber("tcp://192.168.1.192",
                           5556); // Put the server address and port
  client.bindPeerPublisher("tcp://*",
                           5558); // Bind the client publisher to a port
  client.connectPeerSubscriber1(
      "tcp://192.168.1.192",
      5557); // Put peer 1's address and the port they are bounded on
  client.connectPeerSubscriber2(
      "tcp://192.168.1.239",
      5559); // Put peer 2's address and the port they are bounded on
  client.connectServer(true);
  client.start(true);
}

void applyGravityOnEntities(PhysicsSystem &physicsSystem,
                            EntityManager &entityManager) {
  while (1) {
    entityManager.applyGravityOnEntities(physicsSystem);
  }
}

void doServerEntities(Server &server) {
  Vector2 initialPosition2{300, 300};
  Vector2 dimensions{50, 50};
  SDL_Color color = {255, 0, 0, 255};
  auto patternEntity = std::make_shared<Entity>(initialPosition2, dimensions,
                                                color, &globalTimeline, 1);

  MovementPattern pattern;
  pattern.addSteps(
      MovementStep({50, 50}, 2.0f), MovementStep({0, 0}, 1.0f, true),
      MovementStep({50, -50}, 2.0f), MovementStep({0, 0}, 1.0f, true),
      MovementStep({-50, 50}, 2.0f), MovementStep({0, 0}, 1.0f, true),
      MovementStep({-50, -50}, 2.0f), MovementStep({0, 0}, 1.0f, true));

  patternEntity->hasMovementPattern = true;
  patternEntity->movementPattern = pattern;

  EntityManager serverEntityManager;
  serverEntityManager.addEntities(patternEntity);

  std::thread gravityThread(applyGravityOnEntities, std::ref(physicsSystem),
                            std::ref(serverEntityManager));
  gravityThread.detach();

  while (true) {
    serverEntityManager.updateEntityDeltaTime();
    serverEntityManager.updateMovementPatternEntities();
    serverEntityManager.updateEntities();
    server.updateClientEntityMap(serverEntityManager);
  }
}

void doClientGame(bool isP2P = false) {
  initSDL();
  // Define scale factors
  float scale = 1.0f;
  float cached_scale = scale;

  Vector2 playerPosition{400, 100};
  Vector2 platformPosition{200, 300};
  Vector2 referenceObjectPosition{100, 300};

  Vector2 playerDimensions{50, 50};
  Vector2 platformDimensions{500, 500};
  Vector2 referenceObjectDimensions{50, 100};

  SDL_Color color = {0, 255, 0, 255};

  auto player = std::make_shared<Entity>(playerPosition, playerDimensions,
                                         color, &globalTimeline, 2);

  player->maxVelocity = Vector2{300, 300};
  player->isMovable = true;
  player->isHittable = true;
  player->isAffectedByGravity = true;

  auto referenceObject = std::make_shared<Entity>(referenceObjectPosition,
                                                  referenceObjectDimensions,
                                                  color, &globalTimeline, 1);
  auto platform = std::make_shared<Entity>(platformPosition, platformDimensions,
                                           color, &globalTimeline, 2);

  platform->maxVelocity = Vector2{75, 75};
  platform->isHittable = true;

  MovementPattern pattern;
  pattern.addSteps(
      MovementStep({50, 0}, 2.0f), MovementStep({0, 0}, 1.0f, true),
      MovementStep({-50, 0}, 2.0f), MovementStep({0, 0}, 1.0f, true));

  platform->hasMovementPattern = true;
  platform->movementPattern = pattern;

  EntityManager entityManager;
  EntityManager clientEntityManager;
  entityManager.addEntity(player);
  entityManager.addEntities(referenceObject, platform);

  int worldWidth = 5000;
  int worldHeight = 5000;

  Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT);

  if (!isP2P) {
    std::thread networkThread(runClient, std::ref(entityManager),
                              std::ref(clientEntityManager));
    networkThread.detach();
  } else {
    std::thread networkThread(runP2PClient, std::ref(entityManager),
                              std::ref(clientEntityManager));
    networkThread.detach();
  }

  std::thread gravityThread(applyGravityOnEntities, std::ref(physicsSystem),
                            std::ref(entityManager));

  while (true) {
    doInput(player, &globalTimeline, 150.0f);

    entityManager.updateEntityDeltaTime();
    entityManager.updateMovementPatternEntities();
    entityManager.updateEntities();

    camera.update(*player, worldWidth, worldHeight);

    // Clear the screen with a blue background
    prepareScene(SDL_Color{0, 0, 255, 255});

    entityManager.drawEntities(camera.position.x, camera.position.y);
    clientEntityManager.drawEntities(camera.position.x, camera.position.y);

    updateScaleFactor(scale);
    if (allowScaling && cached_scale != scale) {
      setRenderScale(scale, scale);
      cached_scale = scale;
    }

    // std::string collisionDirection = checkCollisionDirection(entity,
    // platform); std::cout << collisionDirection << std::endl;

    if (player->isColliding(*platform)) {
      collision_utils::handlePlatformCollision(player, platform);
    } else {
      player->clearPlatformReference();
    }

    // if (entityManager.checkCollisions(clientEntityManager)) {
    //     exit(1);
    // }

    // Present the updated scene

    presentScene();
  }

  gravityThread.join();
  clean_up_sdl();
}

int main(int argc, char *argv[]) {

  if (argc < 2 || argc > 3) {
    std::cerr << "Usage: " << argv[0]
              << " [server/client] [optional: client_id] or [server "
                 "client/client server]"
              << std::endl;
    return 1;
  }

  std::string mode1 = argv[1];
  std::string mode2 = (argc == 3) ? argv[2] : "";

  if (mode1 == "server" && mode2.empty()) {

    Server server;
    server.bindResponder("tcp://*", 5556);
    server.bindPuller("tcp://*", 5557);
    server.bindPublisher("tcp://*", 5558);

    std::cout << "Starting server..." << std::endl;
    std::thread serverThread(runServer, std::ref(server));

    doServerEntities(std::ref(server));

    serverThread.join();
  } else if (mode1 == "client" && mode2.empty()) {
    doClientGame();
  } else if ((mode1 == "client" && mode2 == "P2P") ||
             (mode1 == "P2P" && mode2 == "client")) {
    doClientGame(true);
  } else if ((mode1 == "server" && mode2 == "client") ||
             (mode1 == "client" && mode2 == "server")) {
    Server server;
    server.bindResponder("tcp://*", 5555);
    server.bindPublisher("tcp://*", 5556);

    std::cout << "Starting server..." << std::endl;
    std::thread serverThread(runServer, std::ref(server));

    std::thread serverEntities(doServerEntities, std::ref(server));

    doClientGame(true);
    serverThread.join();
    serverEntities.join();
  } else {
    std::cerr
        << "Invalid mode. Use 'server', 'client', or both 'server client'."
        << std::endl;
    return 1;
  }

  return 0;
}
