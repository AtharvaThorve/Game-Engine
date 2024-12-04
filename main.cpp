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

void doClientGame() {
  initSDL();
  float scale = 1.0f;
  float cached_scale = scale;

  EntityManager entityManager;
  EntityManager clientEntityManager;

  auto paddle =
      std::make_shared<Entity>(Vector2{300, 550}, Vector2{100, 20},
                               SDL_Color{0, 0, 255, 255}, &globalTimeline, 1);
  paddle->isMovable = true;
  paddle->isHittable = true;
  paddle->maxVelocity = {200, 0};

  auto ball = std::make_shared<Entity>(Vector2{350, 500}, Vector2{20, 20},
                                       SDL_Color{255, 255, 255, 255},
                                       &globalTimeline, 1);
  ball->isMovable = true;
  ball->isHittable = true;
  ball->velocity = Vector2{-100, -100};
  ball->maxVelocity = Vector2{100, 100};

  int worldWidth = 1280;
  int worldHeight = 720;

  std::vector<std::shared_ptr<Entity>> bricks;
  int rows = 5, cols = 10, brickWidth = 60, brickHeight = 20;
  int padding =
      (worldWidth - (cols * brickWidth)) / (cols + 1); // Dynamic padding
  std::srand(static_cast<unsigned>(std::time(nullptr)));
  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      SDL_Color brickColor = {
          static_cast<Uint8>(std::rand() % 256), // Red
          static_cast<Uint8>(std::rand() % 256), // Green
          static_cast<Uint8>(std::rand() % 256), // Blue
          255                                    // Alpha
      };
      int x = padding + col * (brickWidth + padding); // X position
      int y = row * (brickHeight + padding);          // Y position

      auto brick = std::make_shared<Entity>(Vector2{x, y},
                                            Vector2{brickWidth, brickHeight},
                                            brickColor, &globalTimeline, 1);

      brick->isHittable = true;
      bricks.push_back(brick);
      entityManager.addEntity(brick);
    }
  }

  entityManager.addEntities(paddle, ball);

  Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT);

  EventManager &event_manager = EventManager::getInstance();

  std::vector<std::shared_ptr<EntityManager>> entityManagers = {
      std::shared_ptr<EntityManager>(std::addressof(entityManager))};

  CollisionHandler collision_handler(&globalTimeline);
  collision_handler.register_collision_handler(
      "paddle", collision_utils::handleBallPaddleCollision);
  collision_handler.register_collision_handler(
      "brick", collision_utils::handleBallBrickCollision);

  event_manager.register_handler("collision", &collision_handler);

  event_manager.register_handler("death", new DeathHandler(&globalTimeline));

  event_manager.register_handler("input", new InputHandler(&globalTimeline));

  event_manager.register_handler("move", new MovementHandler(&globalTimeline));

  event_manager.register_handler("update_position",
                                 new PositionHandler(&globalTimeline));

  ReplayRecorder replay_recorder(&globalTimeline, entityManagers);
  event_manager.register_wildcard_handler(&replay_recorder);

  std::thread networkThread(runClient, std::ref(entityManager),
                            std::ref(clientEntityManager));

  std::thread gravityThread(applyGravityOnEntities, std::ref(physicsSystem),
                            std::ref(entityManager));

  int64_t previousTime = globalTimeline.getTime();

  while (true) {
    int64_t currentTime = globalTimeline.getTime();
    int64_t elapsedTime = currentTime - previousTime;

    if (elapsedTime < FRAME_DURATION_NS) {
      int64_t sleepTime = FRAME_DURATION_NS - elapsedTime;
      std::this_thread::sleep_for(std::chrono::nanoseconds(sleepTime));
    }

    previousTime = globalTimeline.getTime();

    doInput(paddle, &globalTimeline, 200000.0f, 0.0f);

    camera.update(*paddle, worldWidth, worldHeight);

    // Sky blue background
    prepareScene(SDL_Color{135, 206, 235, 255});

    entityManager.drawEntities(camera.position.x, camera.position.y);

    updateScaleFactor(scale);
    if (allowScaling && cached_scale != scale) {
      setRenderScale(scale, scale);
      cached_scale = scale;
    }

    if (ball->isColliding(*paddle)) {
      Event collision_event("collision", globalTimeline.getTime());
      collision_event.parameters["entity1"] = ball;
      collision_event.parameters["entity2"] = paddle;
      collision_event.parameters["collision_type"] = "paddle";
      event_manager.raise_event(collision_event);
    }

    for (auto &brick : bricks) {
      if (brick->isHittable && ball->isColliding(*brick)) {
        Event collision_event("collision", globalTimeline.getTime());
        collision_event.parameters["entity1"] = ball;
        collision_event.parameters["entity2"] = brick;
        collision_event.parameters["collision_type"] = "brick";
        event_manager.raise_event(collision_event);
      }
    }

    collision_utils::handleBallWallCollision(ball, worldWidth, worldHeight);

    // Need to do this after collision
    entityManager.updateEntityDeltaTime();
    entityManager.updateMovementPatternEntities();
    entityManager.updateEntities(&globalTimeline);

    event_manager.process_events(globalTimeline.getTime());

    if (Client::disconnectRequested.load() || ball->position.y > worldHeight ||
        std::all_of(bricks.begin(), bricks.end(),
                    [](auto &brick) { return !brick->isHittable; })) {
      terminateThreads.store(true);
      break;
    }

    presentScene();
  }

  gravityThread.join();
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
  } else {
    std::cerr
        << "Invalid mode. Use 'server', 'client', or both 'server client'."
        << std::endl;
    return 1;
  }

  return 0;
}