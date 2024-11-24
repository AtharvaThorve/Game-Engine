#include "main.hpp"

Timeline globalTimeline(nullptr, 2);
PhysicsSystem physicsSystem(0.0f, 150.0f);
std::atomic<bool> terminateThreads(false);

// Assuming Server and Client are properly defined classes with start() method
void runServer(Server &server) { server.start(); }

void runClient(EntityManager &entityManager,
               EntityManager &clientEntityManager) {
  Client client(entityManager, clientEntityManager);
  client.connectRequester("tcp://172.25.111.173", 5556);
  client.connectPusher("tcp://172.25.111.173", 5557);
  client.connectSubscriber("tcp://172.25.111.173", 5558);

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

    // Paddles setup
    Vector2 leftPaddlePos{50, SCREEN_HEIGHT/2};
    Vector2 rightPaddlePos{SCREEN_WIDTH - 70, SCREEN_HEIGHT/2};
    Vector2 paddleDimensions{20, 100};
    SDL_Color paddleColor = {255, 255, 255, 255};

    auto leftPaddle = std::make_shared<Entity>(leftPaddlePos, paddleDimensions, paddleColor, &globalTimeline, 2);
    auto rightPaddle = std::make_shared<Entity>(rightPaddlePos, paddleDimensions, paddleColor, &globalTimeline, 2);
    
    // Ball setup
    Vector2 ballPos{SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
    SDL_Point ballCenter{static_cast<int>(ballPos.x), static_cast<int>(ballPos.y)};
    SDL_Color ballColor = {255, 255, 255, 255};
    auto ball = std::make_shared<Entity>(ballPos, ballCenter, 10, ballColor, &globalTimeline, 2);
    
    ball->velocity = Vector2{200, 200};
    ball->isMovable = true;
    ball->isHittable = true;

    EntityManager playerEntityManager;
    EntityManager entityManager;
    EntityManager clientEntityManager;

    entityManager.addEntities(leftPaddle, rightPaddle, ball);

    EventManager &event_manager = EventManager::getInstance();
    std::vector<std::shared_ptr<EntityManager>> entityManagers = {
        std::make_shared<EntityManager>(entityManager),
        std::make_shared<EntityManager>(clientEntityManager)
    };

    event_manager.register_handler("collision", new CollisionHandler(&globalTimeline));
    event_manager.register_handler("input", new InputHandler(&globalTimeline));
    event_manager.register_handler("move", new MovementHandler(&globalTimeline));
    event_manager.register_handler("update_position", new PositionHandler(&globalTimeline));

    ReplayRecorder replay_recorder(&globalTimeline, entityManagers);
    event_manager.register_wildcard_handler(&replay_recorder);

    std::thread networkThread(runClient, std::ref(playerEntityManager), std::ref(clientEntityManager));

    while (true) {
        // Handle paddle movement with keyboard input
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                terminateThreads.store(true);
                break;
            }
        }

        const Uint8* keystate = SDL_GetKeyboardState(NULL);
                
        // Left paddle control (W and S keys)
        if (keystate[SDL_SCANCODE_W]) {
            leftPaddle->position.y -= 300.0f * leftPaddle->deltaTime;
        } else if (keystate[SDL_SCANCODE_S]) {
            leftPaddle->position.y += 300.0f * leftPaddle->deltaTime;
        }

        // Right paddle control (Up and Down arrow keys)
        if (keystate[SDL_SCANCODE_UP]) {
            rightPaddle->position.y -= 300.0f * rightPaddle->deltaTime;
        } else if (keystate[SDL_SCANCODE_DOWN]) {
            rightPaddle->position.y += 300.0f * rightPaddle->deltaTime;
        }

        // Keep paddles within screen bounds
        leftPaddle->position.y = std::max(paddleDimensions.y/2.0f, 
                                std::min(leftPaddle->position.y, 
                                SCREEN_HEIGHT - paddleDimensions.y/2.0f));
        rightPaddle->position.y = std::max(paddleDimensions.y/2.0f, 
                                 std::min(rightPaddle->position.y, 
                                 SCREEN_HEIGHT - paddleDimensions.y/2.0f));

        prepareScene(SDL_Color{0, 0, 0, 255});
        
        entityManager.drawEntities(0, 0);
        clientEntityManager.drawEntities(0, 0);

        // Rest of the game logic remains the same
        if (ball->isColliding(*leftPaddle) || ball->isColliding(*rightPaddle)) {
            Event collision_event("collision", globalTimeline.getTime());
            collision_event.parameters["entity1"] = ball;
            collision_event.parameters["collision_type"] = std::hash<std::string>{}("paddle");
            event_manager.raise_event(collision_event);
            ball->velocity.x *= -1.1f;
        }

        if (ball->position.y <= 0 || ball->position.y >= SCREEN_HEIGHT) {
            ball->velocity.y *= -1;
        }

        if (ball->position.x <= 0 || ball->position.x >= SCREEN_WIDTH) {
            ball->position = Vector2{SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
            ball->velocity = Vector2{200, 200};
        }

        updateScaleFactor(scale);
        if (allowScaling && cached_scale != scale) {
            setRenderScale(scale, scale);
            cached_scale = scale;
        }

        entityManager.updateEntityDeltaTime();
        entityManager.updateEntities(&globalTimeline);
        event_manager.process_events(globalTimeline.getTime());

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