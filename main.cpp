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
  float gameScale = 1.0f;
  float cached_gameScale = gameScale;

  const int CELL_DIMENSION = 30;
  const int GRID_WIDTH = SCREEN_WIDTH / CELL_DIMENSION;
  const int GRID_HEIGHT = SCREEN_HEIGHT / CELL_DIMENSION;

  // Snake setup
  SDL_Color snakeColor = {75, 0, 130, 255}; // Indigo snake
  Vector2 snakePosition{CELL_DIMENSION * 5, CELL_DIMENSION * 5};
  Vector2 snakeSize{CELL_DIMENSION - 4, CELL_DIMENSION - 4};

  auto snakeHead = std::make_shared<Entity>(snakePosition, snakeSize,
                                            snakeColor, &globalTimeline, 2);
  snakeHead->maxVelocity = Vector2{1, 1};
  snakeHead->velocity = Vector2{1, 1};
  snakeHead->isMovable = true;
  snakeHead->isHittable = true;
  snakeHead->isAffectedByGravity = false;

  std::deque<std::shared_ptr<Entity>> snakeSegments;

  // Points setup
  SDL_Color pointsColor = {50, 205, 50, 255}; // Lime green diamond
  auto points = std::make_shared<Entity>(
      Vector2{CELL_DIMENSION * 10, CELL_DIMENSION * 10},
      Vector2{CELL_DIMENSION - 4, CELL_DIMENSION - 4}, pointsColor,
      &globalTimeline, 2);
  points->isHittable = true;

  // Obstacles setup
  std::vector<std::shared_ptr<Entity>> barriers;
  SDL_Color barrierColor = {139, 69, 19, 255}; // Saddle brown obstacles

  auto barrier1 =
      std::make_shared<Entity>(Vector2{CELL_DIMENSION * 8, CELL_DIMENSION * 8},
                               Vector2{CELL_DIMENSION * 3, CELL_DIMENSION},
                               barrierColor, &globalTimeline, 2);

  auto barrier2 =
      std::make_shared<Entity>(Vector2{CELL_DIMENSION * 3, CELL_DIMENSION * 12},
                               Vector2{CELL_DIMENSION, CELL_DIMENSION * 4},
                               barrierColor, &globalTimeline, 2);

  auto barrier3 =
      std::make_shared<Entity>(Vector2{CELL_DIMENSION * 15, CELL_DIMENSION * 5},
                               Vector2{CELL_DIMENSION * 2, CELL_DIMENSION * 3},
                               barrierColor, &globalTimeline, 2);

  auto barrier4 = std::make_shared<Entity>(
      Vector2{CELL_DIMENSION * 12, CELL_DIMENSION * 15},
      Vector2{CELL_DIMENSION * 4, CELL_DIMENSION}, barrierColor,
      &globalTimeline, 2);

  auto barrier5 = std::make_shared<Entity>(
      Vector2{CELL_DIMENSION * 18, CELL_DIMENSION * 10},
      Vector2{CELL_DIMENSION, CELL_DIMENSION * 5}, barrierColor,
      &globalTimeline, 2);

  auto barrier6 = std::make_shared<Entity>(
      Vector2{CELL_DIMENSION * 30, CELL_DIMENSION * 25},
      Vector2{CELL_DIMENSION, CELL_DIMENSION * 5}, barrierColor,
      &globalTimeline, 2);

  auto barrier7 =
      std::make_shared<Entity>(Vector2{CELL_DIMENSION * 20, CELL_DIMENSION * 2},
                               Vector2{CELL_DIMENSION, CELL_DIMENSION * 8},
                               barrierColor, &globalTimeline, 2);

  barriers.push_back(barrier1);
  barriers.push_back(barrier2);
  barriers.push_back(barrier3);
  barriers.push_back(barrier4);
  barriers.push_back(barrier5);
  barriers.push_back(barrier6);
  barriers.push_back(barrier7);

  EntityManager playerEntityManager;
  playerEntityManager.addEntities(snakeHead);

  EntityManager entityManager;
  EntityManager clientEntityManager;

  entityManager.addEntities(snakeHead);
  entityManager.addEntities(points);

  for (const auto &barrier : barriers) {
    entityManager.addEntities(barrier);
    barrier->isHittable = true;
  }

  // Game boundaries
  SDL_Color boundaryColor = {0, 0, 0, 255};
  auto topBoundary = std::make_shared<Entity>(
      Vector2{0, 0}, Vector2{SCREEN_WIDTH, CELL_DIMENSION}, boundaryColor,
      &globalTimeline, 2);
  auto bottomBoundary = std::make_shared<Entity>(
      Vector2{0, SCREEN_HEIGHT - CELL_DIMENSION},
      Vector2{SCREEN_WIDTH, CELL_DIMENSION}, boundaryColor, &globalTimeline, 2);
  auto leftBoundary = std::make_shared<Entity>(
      Vector2{0, 0}, Vector2{CELL_DIMENSION, SCREEN_HEIGHT}, boundaryColor,
      &globalTimeline, 2);
  auto rightBoundary =
      std::make_shared<Entity>(Vector2{SCREEN_WIDTH - CELL_DIMENSION, 0},
                               Vector2{CELL_DIMENSION, SCREEN_HEIGHT},
                               boundaryColor, &globalTimeline, 2);

  topBoundary->isHittable = true;
  bottomBoundary->isHittable = true;
  leftBoundary->isHittable = true;
  rightBoundary->isHittable = true;

  entityManager.addEntities(topBoundary, bottomBoundary, leftBoundary,
                            rightBoundary);
  entityManager.addDeathZones(topBoundary, bottomBoundary, leftBoundary,
                              rightBoundary);

  Vector2 currentDirection = {CELL_DIMENSION, 0};
  Vector2 nextDirection = currentDirection;

  Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT);
  EventManager &event_manager = EventManager::getInstance();

  std::vector<std::shared_ptr<EntityManager>> entityManagers = {
      std::shared_ptr<EntityManager>(std::addressof(entityManager)),
      std::shared_ptr<EntityManager>(std::addressof(clientEntityManager))};

  CollisionHandler collision_handler(&globalTimeline);
  collision_handler.register_collision_handler(
      "death_zone", collision_utils::handleDeathZoneCollision);

  collision_handler.register_collision_handler(
      "points",
      [&](std::shared_ptr<Entity> snakeHead, std::shared_ptr<Entity> points) {
        auto newSegment = std::make_shared<Entity>(
            snakeSegments.empty() ? snakeHead->position
                                  : snakeSegments.back()->position,
            snakeSize, snakeColor, &globalTimeline, 2);
        snakeSegments.push_back(newSegment);
        entityManager.addEntities(newSegment);

        int newX = ((rand() + 2) % (GRID_WIDTH - 2) + 1) * CELL_DIMENSION;
        int newY = ((rand() + 2) % (GRID_HEIGHT - 2) + 1) * CELL_DIMENSION;
        points->position = Vector2{(float)newX, (float)newY};
      });

  collision_handler.register_collision_handler(
      "barrier",
      [](std::shared_ptr<Entity> snakeHead, std::shared_ptr<Entity> barrier) {
        std::cout << "Game Over! You crashed into a barrier!" << std::endl;
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
  const int64_t MOVE_DELAY = 250000000; // 100ms - faster snake speed

  while (true) {
    int64_t currentTime = globalTimeline.getTime();
    int64_t elapsedTime = currentTime - previousTime;

    if (elapsedTime < FRAME_DURATION_NS) {
      int64_t sleepTime = FRAME_DURATION_NS - elapsedTime;
      std::this_thread::sleep_for(std::chrono::nanoseconds(sleepTime));
    }

    previousTime = currentTime;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        Client::disconnectRequested.store(true);
        terminateThreads.store(true);
        break;
      }
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);
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

    if (state[SDL_SCANCODE_W] && currentDirection.y == 0) {
      nextDirection = Vector2{0, -CELL_DIMENSION};
    } else if (state[SDL_SCANCODE_S] && currentDirection.y == 0) {
      nextDirection = Vector2{0, CELL_DIMENSION};
    } else if (state[SDL_SCANCODE_A] && currentDirection.x == 0) {
      nextDirection = Vector2{-CELL_DIMENSION, 0};
    } else if (state[SDL_SCANCODE_D] && currentDirection.x == 0) {
      nextDirection = Vector2{CELL_DIMENSION, 0};
    }

    moveTimer += elapsedTime;
    if (moveTimer >= MOVE_DELAY) {
      moveTimer = 0;
      currentDirection = nextDirection;

      Vector2 prevPos = snakeHead->position;

      snakeHead->position.x += currentDirection.x;
      snakeHead->position.y += currentDirection.y;

      for (size_t i = 0; i < snakeSegments.size(); i++) {
        Vector2 currentPos = snakeSegments[i]->position;
        snakeSegments[i]->position = prevPos;
        prevPos = currentPos;
      }

      if (snakeHead->isColliding(*points)) {
        Event collision_event("collision", currentTime);
        collision_event.parameters["entity1"] = snakeHead;
        collision_event.parameters["entity2"] = points;
        collision_event.parameters["collision_type"] = "points";
        event_manager.raise_event(collision_event);
      }

      for (const auto &barrier : barriers) {
        if (snakeHead->isColliding(*barrier)) {
          Event collision_event("collision", currentTime);
          collision_event.parameters["entity1"] = snakeHead;
          collision_event.parameters["entity2"] = barrier;
          collision_event.parameters["collision_type"] = "barrier";
          event_manager.raise_event(collision_event);
          break;
        }
      }

      for (const auto &segment : snakeSegments) {
        if (snakeHead->isColliding(*segment)) {
          Event death_event("death", currentTime);
          death_event.parameters["player"] = snakeHead;
          event_manager.raise_event(death_event);
          break;
        }
      }
    }

    prepareScene(SDL_Color{255, 255, 255, 255});

    entityManager.drawEntities(camera.position.x, camera.position.y);
    clientEntityManager.drawEntities(camera.position.x, camera.position.y);

    updateScaleFactor(gameScale);
    if (allowScaling && cached_gameScale != gameScale) {
      setRenderScale(gameScale, gameScale);
      cached_gameScale = gameScale;
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