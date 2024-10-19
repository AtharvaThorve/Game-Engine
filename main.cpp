#include "main.hpp"

Timeline globalTimeline(nullptr, 2);
PhysicsSystem physicsSystem(0.0f, 150.0f);

// Assuming Server and Client are properly defined classes with start() method
void runServer(Server &server) { server.start(); }

void runClient(EntityManager &entityManager,
               EntityManager &clientEntityManager) {
  Client client(entityManager, clientEntityManager);
  client.connectRequester("tcp://172.30.115.140", 5556);
  client.connectPusher("tcp://172.30.115.140", 5557);
  client.connectSubscriber("tcp://172.30.115.140", 5558);
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
  // serverEntityManager.addEntities(patternEntity);

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
  float scale = 1.0f;
  float cached_scale = scale;

  // Player setup
  Vector2 playerPosition{100, 400};
  Vector2 playerDimensions{40, 40};
  SDL_Color playerColor = {255, 0, 0, 255};

  auto player = std::make_shared<Entity>(playerPosition, playerDimensions,
                                         playerColor, &globalTimeline, 2);
  player->maxVelocity = Vector2{200, 600};
  player->isMovable = true;
  player->isHittable = true;
  player->isAffectedByGravity = true;

  Vector2 player2Position{700, 400};
  Vector2 player2Dimensions{40, 40};
  SDL_Color player2Color = {0, 0, 255, 255}; // Blue player

  auto player2 = std::make_shared<Entity>(player2Position, player2Dimensions,
                                          player2Color, &globalTimeline, 2);

  EntityManager playerEntityManager;
  playerEntityManager.addEntities(player);
  EntityManager entityManager;
  EntityManager clientEntityManager;

  // Ground platforms
  SDL_Color platformColor = {139, 69, 19, 255}; // Brown color for platforms
  auto ground1 = std::make_shared<Entity>(Vector2{-500, 550}, Vector2{2000, 50},
                                          platformColor, &globalTimeline, 2);
  auto ground2 = std::make_shared<Entity>(Vector2{1600, 550}, Vector2{2000, 50},
                                          platformColor, &globalTimeline, 2);
  ground1->isHittable = true;
  ground2->isHittable = true;

  // Variety of platforms
  std::vector<std::shared_ptr<Entity>> platforms;

  // Static platforms
  platforms.push_back(std::make_shared<Entity>(
      Vector2{300, 400}, Vector2{200, 30}, platformColor, &globalTimeline, 2));
  platforms.push_back(std::make_shared<Entity>(
      Vector2{600, 300}, Vector2{200, 30}, platformColor, &globalTimeline, 2));
  platforms.push_back(std::make_shared<Entity>(
      Vector2{900, 250}, Vector2{200, 30}, platformColor, &globalTimeline, 2));
  platforms.push_back(std::make_shared<Entity>(
      Vector2{1300, 350}, Vector2{150, 30}, platformColor, &globalTimeline, 2));
  platforms.push_back(std::make_shared<Entity>(
      Vector2{1600, 200}, Vector2{180, 30}, platformColor, &globalTimeline, 2));

  // Moving platforms
  auto createMovingPlatform = [&](Vector2 pos, Vector2 size, SDL_Color color,
                                  MovementPattern pattern) {
    auto platform =
        std::make_shared<Entity>(pos, size, color, &globalTimeline, 2);
    platform->maxVelocity = Vector2{100, 100};
    platform->isHittable = true;
    platform->hasMovementPattern = true;
    platform->movementPattern = pattern;
    return platform;
  };

  SDL_Color movingPlatformColor = {255, 215, 0, 255}; // Golden color

  // Horizontal moving platform
  MovementPattern horizontalPattern;
  horizontalPattern.addSteps(
      MovementStep({300, 0}, 4.0f), MovementStep({0, 0}, 0.5f, true),
      MovementStep({-300, 0}, 4.0f), MovementStep({0, 0}, 0.5f, true));
  platforms.push_back(createMovingPlatform(Vector2{400, 200}, Vector2{100, 20},
                                           movingPlatformColor,
                                           horizontalPattern));

  // Vertical moving platform
  MovementPattern verticalPattern;
  verticalPattern.addSteps(
      MovementStep({0, 200}, 3.0f), MovementStep({0, 0}, 0.5f, true),
      MovementStep({0, -200}, 3.0f), MovementStep({0, 0}, 0.5f, true));
  platforms.push_back(createMovingPlatform(Vector2{700, 350}, Vector2{100, 20},
                                           movingPlatformColor,
                                           verticalPattern));

  // Circular moving platform
  MovementPattern circularPattern;
  for (int i = 0; i < 36; ++i) {
    float angle = i * 10 * M_PI / 180.0f;
    float x = 100 * cos(angle);
    float y = 100 * sin(angle);
    circularPattern.addSteps(MovementStep({x, y}, 0.2f));
  }
  platforms.push_back(createMovingPlatform(Vector2{1000, 400}, Vector2{80, 20},
                                           movingPlatformColor,
                                           circularPattern));

  // Figure-8 moving platform
  MovementPattern figure8Pattern;
  for (int i = 0; i < 60; ++i) {
    float t = i * 2 * M_PI / 60;
    float x = 100 * sin(t);
    float y = 50 * sin(2 * t);
    figure8Pattern.addSteps(MovementStep({x, y}, 0.1f));
  }
  platforms.push_back(createMovingPlatform(Vector2{1400, 300}, Vector2{80, 20},
                                           movingPlatformColor,
                                           figure8Pattern));

  // Add all platforms to entityManager
  entityManager.addEntities(ground1, ground2);
  entityManager.addEntities(player);
  for (const auto &platform : platforms) {
    platform->isHittable = true;
    entityManager.addEntities(platform);
  }

  // Collectibles (coins)
  SDL_Color coinColor = {255, 215, 0, 255}; // Gold color
  auto coin1 = std::make_shared<Entity>(Vector2{300, 350}, Vector2{20, 20},
                                        coinColor, &globalTimeline, 1);
  auto coin2 = std::make_shared<Entity>(Vector2{600, 250}, Vector2{20, 20},
                                        coinColor, &globalTimeline, 1);
  auto coin3 = std::make_shared<Entity>(Vector2{900, 200}, Vector2{20, 20},
                                        coinColor, &globalTimeline, 1);
  auto coin4 = std::make_shared<Entity>(Vector2{1300, 300}, Vector2{20, 20},
                                        coinColor, &globalTimeline, 1);
  auto coin5 = std::make_shared<Entity>(Vector2{1600, 150}, Vector2{20, 20},
                                        coinColor, &globalTimeline, 1);

  entityManager.addEntities(coin1, coin2, coin3, coin4, coin5);

  // Death zones (lava pits and invisible gap)
  SDL_Color lavaColor = {255, 69, 0, 255}; // Orange-red for lava
  SDL_Color invisibleColor = {
      135, 206, 235, 255};

  // Lava on top of a platform
  auto lavaPlatform = std::make_shared<Entity>(
      Vector2{1100, 500}, Vector2{200, 30}, platformColor, &globalTimeline, 2);
  lavaPlatform->isHittable = true;
  auto lava1 = std::make_shared<Entity>(Vector2{1100, 470}, Vector2{200, 30},
                                        lavaColor, &globalTimeline, 2);

  // Invisible death zone in the gap between ground platforms
  auto invisibleDeathZone =
      std::make_shared<Entity>(Vector2{1500, 700}, Vector2{100, 200},
                               invisibleColor, &globalTimeline, 2);

  entityManager.addEntities(lavaPlatform);
  entityManager.addDeathZones(lava1, invisibleDeathZone, lavaPlatform);

  // Spawn points
  auto spawnPoint1 =
      std::make_shared<Entity>(Vector2{100, 400}, Vector2{1, 1},
                               SDL_Color{0, 0, 0, 0}, &globalTimeline, 1);
  auto spawnPoint2 =
      std::make_shared<Entity>(Vector2{500, 200}, Vector2{1, 1},
                               SDL_Color{0, 0, 0, 0}, &globalTimeline, 1);
  entityManager.addSpawnPoint(spawnPoint1);
  entityManager.addSpawnPoint(spawnPoint2);

  int worldWidth = 5000;
  int worldHeight = 2000;

  Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT);

  std::thread networkThread(runClient, std::ref(playerEntityManager),
                            std::ref(clientEntityManager));
  std::thread gravityThread(applyGravityOnEntities, std::ref(physicsSystem),
                            std::ref(entityManager));

  while (true) {
    doInput(player, &globalTimeline, 200.0f);

    playerEntityManager.updateEntityDeltaTime();
    playerEntityManager.updateMovementPatternEntities();
    playerEntityManager.updateEntities();
    entityManager.updateEntityDeltaTime();
    entityManager.updateMovementPatternEntities();
    entityManager.updateEntities();

    camera.update(*player, worldWidth, worldHeight);

    // Sky blue background
    prepareScene(SDL_Color{135, 206, 235, 255});

    playerEntityManager.drawEntities(camera.position.x, camera.position.y);
    entityManager.drawEntities(camera.position.x, camera.position.y);
    clientEntityManager.drawEntities(camera.position.x, camera.position.y);

    updateScaleFactor(scale);
    if (allowScaling && cached_scale != scale) {
      setRenderScale(scale, scale);
      cached_scale = scale;
    }

    // Check death and respawn
    if (entityManager.checkPlayerDeath(player)) {
      entityManager.respawn(player);
    }

    // Handle collisions
    for (const auto &platform : platforms) {
      if (player->isColliding(*platform)) {
        collision_utils::handlePlatformCollision(player, platform);
      }
    }

    if (player->isColliding(*ground1)) {
      collision_utils::handlePlatformCollision(player, ground1);
    }
    if (player->isColliding(*ground2)) {
      collision_utils::handlePlatformCollision(player, ground2);
    }
    if (player->isColliding(*lavaPlatform)) {
      collision_utils::handlePlatformCollision(player, lavaPlatform);
    }

    // Handle coin collisions
    std::vector<std::shared_ptr<Entity>> coins = {coin1, coin2, coin3, coin4,
                                                  coin5};
    for (const auto &coin : coins) {
      if (player->isColliding(*coin)) {
        entityManager.removeEntity(coin);
      }
    }

    presentScene();
  }

  networkThread.join();
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