#include "main.hpp"

constexpr int64_t TARGET_REFRESH_RATE = 120;
constexpr int64_t NANOSECONDS_PER_SECOND = 1000000000;
constexpr int64_t FRAME_DURATION_NS =
    NANOSECONDS_PER_SECOND / TARGET_REFRESH_RATE;

Timeline globalTimeline(nullptr, 2);
PhysicsSystem physicsSystem(0.0f, 150.0f);
std::atomic<bool> terminateThreads(false);

// Assuming Server and Client are properly defined classes with start() method
void runServer(Server &server) { server.start(); }

void runClient(EntityManager &entityManager,
               EntityManager &clientEntityManager) {
  Client client(entityManager, clientEntityManager);
  client.connectRequester("tcp://172.30.115.140", 5556);
  client.connectPusher("tcp://172.30.115.140", 5557);
  client.connectSubscriber("tcp://172.30.115.140", 5558);

  bool connected = false;
  if (client.connectServer()) {
    connected = true;
  }

  while (!terminateThreads.load()) {
    if (connected)
      client.start();
  }

  if (connected)
    client.start();
  std::cout << "Exiting client" << std::endl;
  client.cleanup();
  exit(0);
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
  while (!terminateThreads.load()) {
    entityManager.applyGravityOnEntities(physicsSystem);
  }
  std::cout << "Exiting gravity" << std::endl;
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

  EventManager &event_manager = EventManager::getInstance();

  event_manager.register_handler("collision",
                                 new CollisionHandler(&globalTimeline));

  event_manager.register_handler("move", new MovementHandler(&globalTimeline));

  while (true) {
    serverEntityManager.updateEntityDeltaTime();
    serverEntityManager.updateMovementPatternEntities();
    serverEntityManager.updateEntities(&globalTimeline);
    server.updateClientEntityMap(serverEntityManager);
  }
}

void doClientGame(bool isP2P = false) {
  initSDL();
  float scale = 1.0f;
  float cached_scale = scale;

  const int GRID_SIZE = 20; // Size of each grid cell
  const int GRID_WIDTH = SCREEN_WIDTH / GRID_SIZE;
  const int GRID_HEIGHT = SCREEN_HEIGHT / GRID_SIZE;

  // Snake head setup with a new dark purple color
  SDL_Color snakeColor = {128, 0, 128, 255}; // Dark purple snake
  Vector2 playerPosition{GRID_SIZE * 5,
                         GRID_SIZE * 5}; // Start at grid position (5,5)
  Vector2 playerDimensions{GRID_SIZE - 2,
                           GRID_SIZE - 2}; // Slightly smaller than grid

  auto player = std::make_shared<Entity>(playerPosition, playerDimensions,
                                         snakeColor, &globalTimeline, 2);
  player->maxVelocity =
      Vector2{GRID_SIZE, GRID_SIZE}; // Move one grid at a time
  player->isMovable = true;
  player->isHittable = true;
  player->isAffectedByGravity = false; // No gravity in Snake

  // Snake body segments
  std::deque<std::shared_ptr<Entity>> snakeBody;

  // Food setup
  SDL_Color foodColor = {255, 0, 0, 255}; // Red for food
  auto food = std::make_shared<Entity>(Vector2{GRID_SIZE * 10, GRID_SIZE * 10},
                                       Vector2{GRID_SIZE - 2, GRID_SIZE - 2},
                                       foodColor, &globalTimeline, 2);
  food->isHittable = true;

  // Obstacle setup - multiple static obstacles
  std::vector<std::shared_ptr<Entity>> obstacles;
  SDL_Color obstacleColor = {255, 165, 0, 255}; // Orange for obstacles

  // Create some fixed obstacles
  auto obstacle1 = std::make_shared<Entity>(
      Vector2{GRID_SIZE * 8, GRID_SIZE * 8}, Vector2{GRID_SIZE * 2, GRID_SIZE},
      obstacleColor, &globalTimeline, 2);

  auto obstacle2 = std::make_shared<Entity>(
      Vector2{GRID_SIZE * 3, GRID_SIZE * 12}, Vector2{GRID_SIZE, GRID_SIZE * 3},
      obstacleColor, &globalTimeline, 2);

  auto obstacle3 = std::make_shared<Entity>(
      Vector2{GRID_SIZE * 15, GRID_SIZE * 5}, Vector2{GRID_SIZE, GRID_SIZE * 2},
      obstacleColor, &globalTimeline, 2);

  obstacles.push_back(obstacle1);
  obstacles.push_back(obstacle2);
  obstacles.push_back(obstacle3);

  EntityManager playerEntityManager;
  playerEntityManager.addEntities(player);

  EntityManager entityManager;
  EntityManager clientEntityManager;

  // Add initial entities
  entityManager.addEntities(player);
  entityManager.addEntities(food);

  // Add obstacles to entity manager
  for (const auto &obstacle : obstacles) {
    entityManager.addEntities(obstacle);
    obstacle->isHittable = true;
  }

  // Game boundaries (same as before)
  SDL_Color borderColor = {128, 128, 128, 255}; // Gray for borders
  auto topWall =
      std::make_shared<Entity>(Vector2{0, 0}, Vector2{SCREEN_WIDTH, GRID_SIZE},
                               borderColor, &globalTimeline, 2);
  auto bottomWall = std::make_shared<Entity>(
      Vector2{0, SCREEN_HEIGHT - GRID_SIZE}, Vector2{SCREEN_WIDTH, GRID_SIZE},
      borderColor, &globalTimeline, 2);
  auto leftWall =
      std::make_shared<Entity>(Vector2{0, 0}, Vector2{GRID_SIZE, SCREEN_HEIGHT},
                               borderColor, &globalTimeline, 2);
  auto rightWall = std::make_shared<Entity>(
      Vector2{SCREEN_WIDTH - GRID_SIZE, 0}, Vector2{GRID_SIZE, SCREEN_HEIGHT},
      borderColor, &globalTimeline, 2);

  topWall->isHittable = true;
  bottomWall->isHittable = true;
  leftWall->isHittable = true;
  rightWall->isHittable = true;

  entityManager.addEntities(topWall, bottomWall, leftWall, rightWall);
  entityManager.addDeathZones(topWall, bottomWall, leftWall, rightWall);

  // Direction tracking
  Vector2 currentDirection = {GRID_SIZE, 0}; // Start moving right
  Vector2 nextDirection = currentDirection;

  Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT);
  EventManager &event_manager = EventManager::getInstance();

  std::vector<std::shared_ptr<EntityManager>> entityManagers = {
      std::shared_ptr<EntityManager>(std::addressof(entityManager)),
      std::shared_ptr<EntityManager>(std::addressof(clientEntityManager))};

  CollisionHandler collision_handler(&globalTimeline);
  collision_handler.register_collision_handler(
      "death_zone", collision_utils::handleDeathZoneCollision);

  // Food collision handler (same as before)
  collision_handler.register_collision_handler(
      "food", [&](std::shared_ptr<Entity> snake, std::shared_ptr<Entity> food) {
        // Add new body segment
        auto newSegment = std::make_shared<Entity>(
            snakeBody.empty() ? player->position : snakeBody.back()->position,
            playerDimensions, snakeColor, &globalTimeline, 2);
        snakeBody.push_back(newSegment);
        entityManager.addEntities(newSegment);

        // Move food to new random position
        int newX = (rand() % (GRID_WIDTH - 2) + 1) * GRID_SIZE;
        int newY = (rand() % (GRID_HEIGHT - 2) + 1) * GRID_SIZE;
        food->position = Vector2{(float)newX, (float)newY};
      });

  // Add obstacle collision handler to end the game
  collision_handler.register_collision_handler(
      "obstacle",
      [](std::shared_ptr<Entity> snake, std::shared_ptr<Entity> obstacle) {
        std::cout << "Game Over! You hit an obstacle!" << std::endl;
        exit(0);
      });

  event_manager.register_handler("collision", &collision_handler);
  event_manager.register_handler("death", new DeathHandler(&globalTimeline));
  event_manager.register_handler("input", new InputHandler(&globalTimeline));
  event_manager.register_handler("move", new MovementHandler(&globalTimeline));
  event_manager.register_handler("update_position",
                                 new PositionHandler(&globalTimeline));

  ReplayRecorder replay_recorder(&globalTimeline, entityManagers);
  event_manager.register_wildcard_handler(&replay_recorder);

  std::thread networkThread(runClient, std::ref(playerEntityManager),
                            std::ref(clientEntityManager));

  int64_t previousTime = globalTimeline.getTime();
  int64_t moveTimer = 0;
  const int64_t MOVE_DELAY = 150000000; // Snake movement speed (150ms)

  while (true) {
    int64_t currentTime = globalTimeline.getTime();
    int64_t elapsedTime = currentTime - previousTime;

    if (elapsedTime < FRAME_DURATION_NS) {
      int64_t sleepTime = FRAME_DURATION_NS - elapsedTime;
      std::this_thread::sleep_for(std::chrono::nanoseconds(sleepTime));
    }

    previousTime = currentTime;

    // Process events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        Client::disconnectRequested.store(true);
        terminateThreads.store(true);
        break;
      }
    }

    // Handle input for direction changes (same as before)
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    // Replay handling (same as before)
    bool enterPressed = state[SDL_SCANCODE_RETURN];
    static bool wasEnterPressed = false;
    static bool isRecording = false;

    if (enterPressed && !wasEnterPressed) {
      wasEnterPressed = true;
    } else if (!enterPressed && wasEnterPressed) {
      isRecording = !isRecording;
      if (isRecording) {
        std::cout << "Started Recording" << std::endl;
        Event start_recording_event("start_recording", currentTime);
        event_manager.raise_event(start_recording_event);
      } else {
        std::cout << "Stopped Recording" << std::endl;
        Event stop_recording_event("stop_recording", currentTime);
        event_manager.raise_event(stop_recording_event);
      }
      wasEnterPressed = false;
    }

    // Play recording handling (same as before)
    bool rShiftPressed = state[SDL_SCANCODE_RSHIFT];
    static bool wasRShiftPressed = false;
    if (rShiftPressed != wasRShiftPressed) {
      if (rShiftPressed) {
        std::cout << "Playing Recording" << std::endl;
        Event play_recording_event("play_recording", currentTime);
        event_manager.raise_event(play_recording_event);
      }
      wasRShiftPressed = rShiftPressed;
    }

    // Direction input (same as before)
    if (state[SDL_SCANCODE_W] && currentDirection.y == 0) {
      nextDirection = Vector2{0, -GRID_SIZE};
    } else if (state[SDL_SCANCODE_S] && currentDirection.y == 0) {
      nextDirection = Vector2{0, GRID_SIZE};
    } else if (state[SDL_SCANCODE_A] && currentDirection.x == 0) {
      nextDirection = Vector2{-GRID_SIZE, 0};
    } else if (state[SDL_SCANCODE_D] && currentDirection.x == 0) {
      nextDirection = Vector2{GRID_SIZE, 0};
    }

    // Move snake at fixed intervals (same as before)
    moveTimer += elapsedTime;
    if (moveTimer >= MOVE_DELAY) {
      moveTimer = 0;
      currentDirection = nextDirection;

      // Store previous positions
      Vector2 prevPos = player->position;

      // Move head
      player->position.x += currentDirection.x;
      player->position.y += currentDirection.y;

      // Move body segments
      for (size_t i = 0; i < snakeBody.size(); i++) {
        Vector2 currentPos = snakeBody[i]->position;
        snakeBody[i]->position = prevPos;
        prevPos = currentPos;
      }

      // Check collision with food (same as before)
      if (player->isColliding(*food)) {
        Event collision_event("collision", currentTime);
        collision_event.parameters["entity1"] = player;
        collision_event.parameters["entity2"] = food;
        collision_event.parameters["collision_type"] = "food";
        event_manager.raise_event(collision_event);
      }

      // Check collision with obstacles
      for (const auto &obstacle : obstacles) {
        if (player->isColliding(*obstacle)) {
          Event collision_event("collision", currentTime);
          collision_event.parameters["entity1"] = player;
          collision_event.parameters["entity2"] = obstacle;
          collision_event.parameters["collision_type"] = "obstacle";
          event_manager.raise_event(collision_event);
          break;
        }
      }

      // Check collision with body (same as before)
      for (const auto &segment : snakeBody) {
        if (player->isColliding(*segment)) {
          Event death_event("death", currentTime);
          death_event.parameters["player"] = player;
          event_manager.raise_event(death_event);
          break;
        }
      }
    }

    // Rendering (same as before)
    prepareScene(SDL_Color{255, 255, 255, 255});

    entityManager.drawEntities(camera.position.x, camera.position.y);
    clientEntityManager.drawEntities(camera.position.x, camera.position.y);

    updateScaleFactor(scale);
    if (allowScaling && cached_scale != scale) {
      setRenderScale(scale, scale);
      cached_scale = scale;
    }

    event_manager.process_events(currentTime);

    if (Client::disconnectRequested.load()) {
      terminateThreads.store(true);
      break;
    }

    presentScene();
  }

  networkThread.join();
  clean_up_sdl();
  exit(0);
}
// void doClientGame(bool isP2P = false) {
//   initSDL();
//   float scale = 1.0f;
//   float cached_scale = scale;

//   // Player setup
//   Vector2 playerPosition{100, 400};
//   Vector2 playerDimensions{40, 40};
//   SDL_Color playerColor = {255, 0, 0, 255};

//   auto player = std::make_shared<Entity>(playerPosition, playerDimensions,
//                                          playerColor, &globalTimeline, 2);
//   player->maxVelocity = Vector2{100, 300};
//   player->isMovable = true;
//   player->isHittable = true;
//   player->isAffectedByGravity = true;

//   EntityManager playerEntityManager;
//   playerEntityManager.addEntities(player);

//   EntityManager entityManager;
//   EntityManager clientEntityManager;

//   // Ground platforms
//   SDL_Color platformColor = {139, 69, 19, 255}; // Brown color for platforms
//   auto ground1 = std::make_shared<Entity>(Vector2{-500, 550}, Vector2{2000,
//   50},
//                                           platformColor, &globalTimeline, 2);
//   auto ground2 = std::make_shared<Entity>(Vector2{1600, 550}, Vector2{2000,
//   50},
//                                           platformColor, &globalTimeline, 2);
//   ground1->isHittable = true;
//   ground2->isHittable = true;

//   // Add all platforms to entityManager
//   entityManager.addEntities(ground1, ground2);
//   entityManager.addEntities(player);

//   // Death zones (lava pits and invisible gap)
//   SDL_Color lavaColor = {255, 69, 0, 255}; // Orange-red for lava
//   SDL_Color invisibleColor = {135, 206, 235, 255};

//   // Lava on top of a platform
//   auto lavaPlatform = std::make_shared<Entity>(
//       Vector2{1100, 500}, Vector2{200, 30}, lavaColor, &globalTimeline, 2);
//   lavaPlatform->isHittable = true;

//   // Invisible death zone in the gap between ground platforms
//   auto invisibleDeathZone =
//       std::make_shared<Entity>(Vector2{1500, 700}, Vector2{100, 200},
//                                invisibleColor, &globalTimeline, 2);

//   entityManager.addEntities(lavaPlatform);
//   entityManager.addDeathZones(invisibleDeathZone, lavaPlatform);

//   // Spawn points
//   auto spawnPoint1 =
//       std::make_shared<Entity>(Vector2{100, 400}, Vector2{1, 1},
//                                SDL_Color{0, 0, 0, 0}, &globalTimeline, 1);
//   auto spawnPoint2 =
//       std::make_shared<Entity>(Vector2{500, 200}, Vector2{1, 1},
//                                SDL_Color{0, 0, 0, 0}, &globalTimeline, 1);

//   int worldWidth = 5000;
//   int worldHeight = 2000;

//   Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT);

//   EventManager &event_manager = EventManager::getInstance();

//   std::vector<std::shared_ptr<EntityManager>> entityManagers = {
//       std::shared_ptr<EntityManager>(std::addressof(entityManager)),
//       std::shared_ptr<EntityManager>(std::addressof(clientEntityManager))};

//   CollisionHandler collision_handler(&globalTimeline);
//   collision_handler.register_collision_handler(
//       "platform", collision_utils::handlePlatformCollision);
//   collision_handler.register_collision_handler(
//       "death_zone", collision_utils::handleDeathZoneCollision);

//   event_manager.register_handler("collision", &collision_handler);

//   event_manager.register_handler("death", new DeathHandler(&globalTimeline));

//   RespawnHandler respawn_handler(&globalTimeline);
//   respawn_handler.add_spawn_points(spawnPoint1, spawnPoint2);

//   event_manager.register_handler("respawn", &respawn_handler);

//   event_manager.register_handler("input", new InputHandler(&globalTimeline));

//   event_manager.register_handler("move", new
//   MovementHandler(&globalTimeline));

//   event_manager.register_handler("update_position",
//                                  new PositionHandler(&globalTimeline));

//   ReplayRecorder replay_recorder(&globalTimeline, entityManagers);
//   event_manager.register_wildcard_handler(&replay_recorder);

//   std::thread networkThread(runClient, std::ref(playerEntityManager),
//                             std::ref(clientEntityManager));
//   std::thread gravityThread(applyGravityOnEntities, std::ref(physicsSystem),
//                             std::ref(entityManager));

//   int64_t previousTime = globalTimeline.getTime();

//   while (true) {
//     int64_t currentTime = globalTimeline.getTime();
//     int64_t elapsedTime = currentTime - previousTime;

//     if (elapsedTime < FRAME_DURATION_NS) {
//       int64_t sleepTime = FRAME_DURATION_NS - elapsedTime;
//       std::this_thread::sleep_for(std::chrono::nanoseconds(sleepTime));
//     }

//     previousTime = globalTimeline.getTime();

//     doInput(player, &globalTimeline, 50.0f, 200.0f);

//     camera.update(*player, worldWidth, worldHeight);

//     // Sky blue background
//     prepareScene(SDL_Color{135, 206, 235, 255});

//     entityManager.drawEntities(camera.position.x, camera.position.y);
//     clientEntityManager.drawEntities(camera.position.x, camera.position.y);

//     updateScaleFactor(scale);
//     if (allowScaling && cached_scale != scale) {
//       setRenderScale(scale, scale);
//       cached_scale = scale;
//     }

//     // Check death and respawn
//     if (entityManager.checkPlayerDeath(player)) {
//       Event death_event("death", globalTimeline.getTime());
//       death_event.parameters["player"] = player;
//       event_manager.raise_event(death_event);
//     }

//     if (player->isColliding(*ground1)) {
//       Event collision_event("collision", globalTimeline.getTime());
//       collision_event.parameters["entity1"] = player;
//       collision_event.parameters["entity2"] = ground1;
//       collision_event.parameters["collision_type"] = "platform";
//       event_manager.raise_event(collision_event);
//     }
//     if (player->isColliding(*ground2)) {
//       Event collision_event("collision", globalTimeline.getTime());
//       collision_event.parameters["entity1"] = player;
//       collision_event.parameters["entity2"] = ground2;
//       collision_event.parameters["collision_type"] = "platform";
//       event_manager.raise_event(collision_event);
//     }

//     // Need to do this after collision
//     entityManager.updateEntityDeltaTime();
//     entityManager.updateMovementPatternEntities();
//     entityManager.updateEntities(&globalTimeline);

//     event_manager.process_events(globalTimeline.getTime());

//     if (Client::disconnectRequested.load()) {
//       terminateThreads.store(true);
//       break;
//     }

//     presentScene();
//   }

//   gravityThread.join();
//   networkThread.join();
//   clean_up_sdl();
//   exit(0);
// }

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

    EventManager &em = EventManager::getInstance();
    em.register_handler("disconnect", new DisconnectHandler(&globalTimeline));

    Server server(&globalTimeline);
    server.bindResponder("tcp://*", 5556);
    server.bindPuller("tcp://*", 5557);
    server.bindPublisher("tcp://*", 5558);

    std::cout << "Starting server..." << std::endl;
    std::thread serverThread(runServer, std::ref(server));

    doServerEntities(std::ref(server));

    serverThread.join();
  } else if (mode1 == "client" && mode2.empty()) {
    doClientGame();
  }
  // else if ((mode1 == "client" && mode2 == "P2P") ||
  //            (mode1 == "P2P" && mode2 == "client")) {
  //   doClientGame(true);
  // } else if ((mode1 == "server" && mode2 == "client") ||
  //            (mode1 == "client" && mode2 == "server")) {
  //   Server server;
  //   server.bindResponder("tcp://*", 5555);
  //   server.bindPublisher("tcp://*", 5556);

  //   std::cout << "Starting server..." << std::endl;
  //   std::thread serverThread(runServer, std::ref(server));

  //   std::thread serverEntities(doServerEntities, std::ref(server));

  //   doClientGame(true);
  //   serverThread.join();
  //   serverEntities.join();
  // }
  else {
    std::cerr
        << "Invalid mode. Use 'server', 'client', or both 'server client'."
        << std::endl;
    return 1;
  }

  return 0;
}