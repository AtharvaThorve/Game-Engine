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
//   auto ground1 = std::make_shared<Entity>(Vector2{-500, 550}, Vector2{2000, 50},
//                                           platformColor, &globalTimeline, 2);
//   auto ground2 = std::make_shared<Entity>(Vector2{1600, 550}, Vector2{2000, 50},
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
//       std::make_shared<EntityManager>(entityManager),
//       std::make_shared<EntityManager>(clientEntityManager)};

//   event_manager.register_handler("collision",
//                                  new CollisionHandler(&globalTimeline));

//   event_manager.register_handler("death", new DeathHandler(&globalTimeline));

//   RespawnHandler respawn_handler(&globalTimeline);
//   respawn_handler.add_spawn_points(spawnPoint1, spawnPoint2);

//   event_manager.register_handler("respawn", &respawn_handler);

//   event_manager.register_handler("input", new InputHandler(&globalTimeline));

//   event_manager.register_handler("move", new MovementHandler(&globalTimeline));

//   event_manager.register_handler("update_position",
//                                  new PositionHandler(&globalTimeline));

//   ReplayRecorder replay_recorder(&globalTimeline, entityManagers);
//   event_manager.register_wildcard_handler(&replay_recorder);

//   std::thread networkThread(runClient, std::ref(playerEntityManager),
//                             std::ref(clientEntityManager));
//   std::thread gravityThread(applyGravityOnEntities, std::ref(physicsSystem),
//                             std::ref(entityManager));

//   while (true) {
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
//       collision_event.parameters["collision_type"] =
//           std::hash<std::string>{}("platform");
//       event_manager.raise_event(collision_event);
//     }
//     if (player->isColliding(*ground2)) {
//       Event collision_event("collision", globalTimeline.getTime());
//       collision_event.parameters["entity1"] = player;
//       collision_event.parameters["entity2"] = ground2;
//       collision_event.parameters["collision_type"] =
//           std::hash<std::string>{}("platform");
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
// Helper function to spawn food in valid position
void spawnFood(std::shared_ptr<Entity> food, const Vector2& gameArea, int gridSize,
               std::shared_ptr<Entity> snakeHead,
               const std::vector<std::shared_ptr<Entity>>& snakeBody) {
  bool validPosition = false;
  Vector2 newPos;
  
  while (!validPosition) {
    validPosition = true;
    newPos.x = (rand() % static_cast<int>(gameArea.x / gridSize)) * gridSize;
    newPos.y = (rand() % static_cast<int>(gameArea.y / gridSize)) * gridSize;
    
    // Check if food spawns on snake head
    if (std::abs(newPos.x - snakeHead->getPosition().x) < 1 &&
        std::abs(newPos.y - snakeHead->getPosition().y) < 1) {
      validPosition = false;
      continue;
    }
    
    // Check if food spawns on snake body
    for (const auto& segment : snakeBody) {
      if (std::abs(newPos.x - segment->getPosition().x) < 1 &&
          std::abs(newPos.y - segment->getPosition().y) < 1) {
        validPosition = false;
        break;
      }
    }
  }
  
  food->setPosition(newPos);
}


void doClientGame(bool isP2P = false) {
  initSDL();
  float scale = 1.0f;
  float cached_scale = scale;

  // Constants for the game
  const int GRID_SIZE = 20;  // Size of each grid cell
  const int GRID_WIDTH = 40; // Number of cells horizontally
  const int GRID_HEIGHT = 30; // Number of cells vertically
  Vector2 gameArea{GRID_SIZE * GRID_WIDTH, GRID_SIZE * GRID_HEIGHT};
  
  // Snake head setup
  Vector2 startPos{GRID_SIZE * (GRID_WIDTH/2), GRID_SIZE * (GRID_HEIGHT/2)};
  Vector2 dimensions{GRID_SIZE, GRID_SIZE};
  SDL_Color snakeColor = {0, 255, 0, 255}; // Green for snake
  
  auto snakeHead = std::make_shared<Entity>(startPos, dimensions,
                                          snakeColor, &globalTimeline, 2);
  snakeHead->isMovable = true;
  
  // Snake body segments
  std::vector<std::shared_ptr<Entity>> snakeBody;
  
  // Food setup
  SDL_Color foodColor = {255, 0, 0, 255}; // Red for food
  auto food = std::make_shared<Entity>(
      Vector2{0, 0}, dimensions, foodColor, &globalTimeline, 2);
  
  // Entity management
  EntityManager entityManager;
std::vector<std::shared_ptr<EntityManager>> entityManagers = {
      std::shared_ptr<EntityManager>(std::addressof(entityManager))};

  entityManager.addEntities(snakeHead);
  entityManager.addEntities(food);
  EventManager &em = EventManager::getInstance();
  // Spawn initial food
  spawnFood(food, gameArea, GRID_SIZE, snakeHead, snakeBody);
  
  EventManager &event_manager = EventManager::getInstance();
  event_manager.register_handler("collision", new CollisionHandler(&globalTimeline));
  event_manager.register_handler("input", new InputHandler(&globalTimeline));
  event_manager.register_handler("move", new MovementHandler(&globalTimeline));
  
  ReplayRecorder replay_recorder(&globalTimeline, entityManagers);
  event_manager.register_wildcard_handler(&replay_recorder);

  
  // Current direction of snake movement
  enum class Direction { UP, DOWN, LEFT, RIGHT };
  Direction currentDir = Direction::RIGHT;
  
  // Game state
  bool gameOver = false;
  int moveTimer = 0;
  const int MOVE_DELAY = 15; // Controls snake speed
  bool isRecording = false;
  bool wasEnterPressed = false;
  bool wasRShiftPressed = false;

  int64_t previousTime = globalTimeline.getTime();
  
  while (!gameOver) {
    int64_t currentTime = globalTimeline.getTime();
    int64_t elapsedTime = currentTime - previousTime;

    if (elapsedTime < FRAME_DURATION_NS) {
      int64_t sleepTime = FRAME_DURATION_NS - elapsedTime;
      std::this_thread::sleep_for(std::chrono::nanoseconds(sleepTime));
    }
    // Handle input for direction changes
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    
    if (state[SDL_SCANCODE_W] && currentDir != Direction::DOWN) {
      currentDir = Direction::UP;
    }
    else if (state[SDL_SCANCODE_S] && currentDir != Direction::UP) {
      currentDir = Direction::DOWN;
    }
    else if (state[SDL_SCANCODE_A] && currentDir != Direction::RIGHT) {
      currentDir = Direction::LEFT;
    }
    else if (state[SDL_SCANCODE_D] && currentDir != Direction::LEFT) {
      currentDir = Direction::RIGHT;
    }
    
    // Move snake every MOVE_DELAY frames
    if (++moveTimer >= MOVE_DELAY) {
      moveTimer = 0;
          bool enterPressed = state[SDL_SCANCODE_RETURN];
    if (enterPressed && !wasEnterPressed) {
      wasEnterPressed = true;
    } else if (!enterPressed && wasEnterPressed) {
      isRecording = !isRecording;
      if (isRecording) {
        std::cout << "Started Recording" << std::endl;
        Event start_recording_event("start_recording", globalTimeline.getTime());
        em.raise_event(start_recording_event);
      } else {
        std::cout << "Stopped Recording" << std::endl;
        Event stop_recording_event("stop_recording", globalTimeline.getTime());
        em.raise_event(stop_recording_event);
      }
      wasEnterPressed = false;
    }

    // Play recording logic
    bool rShiftPressed = state[SDL_SCANCODE_RSHIFT];
    if (rShiftPressed != wasRShiftPressed) {
      if (rShiftPressed) {
        std::cout << "Playing Recording" << std::endl;
        Event play_recording_event("play_recording", globalTimeline.getTime());
        em.raise_event(play_recording_event);
      }
      wasRShiftPressed = rShiftPressed;
    }

      // Store previous position for body segments
      Vector2 prevHeadPos = snakeHead->getPosition();
      std::vector<Vector2> prevPositions;
      prevPositions.push_back(prevHeadPos);
      
      for (const auto& segment : snakeBody) {
        prevPositions.push_back(segment->getPosition());
      }
      
      // Move head
      Vector2 newPos = snakeHead->getPosition();
      switch (currentDir) {
        case Direction::UP:
          newPos.y -= GRID_SIZE;
          break;
        case Direction::DOWN:
          newPos.y += GRID_SIZE;
          break;
        case Direction::LEFT:
          newPos.x -= GRID_SIZE;
          break;
        case Direction::RIGHT:
          newPos.x += GRID_SIZE;
          break;
      }
      
      // Check wall collision
      if (newPos.x < 0 || newPos.x >= gameArea.x ||
          newPos.y < 0 || newPos.y >= gameArea.y) {
        gameOver = true;
        break;
      }
      
      // Check self collision
      for (const auto& segment : snakeBody) {
        if (std::abs(newPos.x - segment->getPosition().x) < 1 &&
            std::abs(newPos.y - segment->getPosition().y) < 1) {
          gameOver = true;
          break;
        }
      }
      
      snakeHead->setPosition(newPos);
      
      // Move body segments
      for (size_t i = 0; i < snakeBody.size(); ++i) {
        snakeBody[i]->setPosition(prevPositions[i]);
      }
      
      // Check food collision
      if (std::abs(snakeHead->getPosition().x - food->getPosition().x) < GRID_SIZE &&
          std::abs(snakeHead->getPosition().y - food->getPosition().y) < GRID_SIZE) {
        // Grow snake
        auto newSegment = std::make_shared<Entity>(
            prevPositions.back(), dimensions, snakeColor, &globalTimeline, 2);
        snakeBody.push_back(newSegment);
        entityManager.addEntities(newSegment);
        
        // Spawn new food
        spawnFood(food, gameArea, GRID_SIZE, snakeHead, snakeBody);
      }
    }
    
    // Clear screen with black background
    prepareScene(SDL_Color{0, 0, 0, 255});
    
    // Draw game objects
    entityManager.drawEntities(0, 0);
    
    presentScene();
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        gameOver = true;
      }
    }
  }
  
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