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

    // Paddles setup
    Vector2 leftPaddlePos{50, SCREEN_HEIGHT/2};
    Vector2 rightPaddlePos{SCREEN_WIDTH - 70, SCREEN_HEIGHT/2};
    Vector2 paddleDimensions{20, 100};
    SDL_Color paddleColor = {255, 255, 255, 255};

    auto leftPaddle = std::make_shared<Entity>(leftPaddlePos, paddleDimensions, paddleColor, &globalTimeline, 2);
    auto rightPaddle = std::make_shared<Entity>(rightPaddlePos, paddleDimensions, paddleColor, &globalTimeline, 2);
    
    // Ball setup
    // Ball setup as rectangle
    Vector2 ballPos{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    Vector2 ballDimensions{20, 20}; // Width and Height of the ball
    SDL_Color ballColor = {255, 255, 255, 255};

    // Create the ball entity using dimensions instead of a center point and radius
    auto ball = std::make_shared<Entity>(ballPos, ballDimensions, paddleColor, &globalTimeline, 2);

    // Set the velocity and other properties
    ball->velocity = Vector2{200, 200};
    ball->maxVelocity = Vector2{300, 300};
    ball->isMovable = true;
    ball->isHittable = true;

    EntityManager playerEntityManager;
    EntityManager entityManager;
    EntityManager clientEntityManager;

    entityManager.addEntities(leftPaddle, rightPaddle, ball);

    EventManager &event_manager = EventManager::getInstance();
  std::vector<std::shared_ptr<EntityManager>> entityManagers = {
      std::shared_ptr<EntityManager>(std::addressof(entityManager)),
      std::shared_ptr<EntityManager>(std::addressof(clientEntityManager))};

    // Event handlers
    CollisionHandler collision_handler(&globalTimeline);
    event_manager.register_handler("collision", &collision_handler);
    event_manager.register_handler("input", new InputHandler(&globalTimeline));
    event_manager.register_handler("move", new MovementHandler(&globalTimeline));
    event_manager.register_handler("update_position", new PositionHandler(&globalTimeline));

    // Replay recorder
    ReplayRecorder replay_recorder(&globalTimeline, entityManagers);
    event_manager.register_wildcard_handler(&replay_recorder);

    std::thread networkThread(runClient, std::ref(playerEntityManager), 
                               std::ref(clientEntityManager));

    int64_t previousTime = globalTimeline.getTime();

    while (true) {
        int64_t currentTime = globalTimeline.getTime();
        int64_t elapsedTime = currentTime - previousTime;

        if (elapsedTime < FRAME_DURATION_NS) {
            int64_t sleepTime = FRAME_DURATION_NS - elapsedTime;
            std::this_thread::sleep_for(std::chrono::nanoseconds(sleepTime));
        }

        previousTime = currentTime;

        // Handle SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                Client::disconnectRequested.store(true);
                terminateThreads.store(true);
                break;
            }
        }

        // Handle keyboard input
        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        
        // Replay recording controls
        bool enterPressed = keystate[SDL_SCANCODE_RETURN];
        static bool wasEnterPressed = false;
        static bool isRecording = false;
        
        // Toggle recording
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

        // Play recording with Right Shift
        bool rShiftPressed = keystate[SDL_SCANCODE_RSHIFT];
        static bool wasRShiftPressed = false;
        if (rShiftPressed != wasRShiftPressed) {
            if (rShiftPressed) {
                std::cout << "Playing Recording" << std::endl;
                Event play_recording_event("play_recording", currentTime);
                event_manager.raise_event(play_recording_event);
            }
            wasRShiftPressed = rShiftPressed;
        }
        
        // Paddle movement
        if (keystate[SDL_SCANCODE_W]) {
            leftPaddle->position.y -= 300.0f * leftPaddle->deltaTime;
        } else if (keystate[SDL_SCANCODE_S]) {
            leftPaddle->position.y += 300.0f * leftPaddle->deltaTime;
        }

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

        // Collision handling
        // Collision handling
        // Keep paddles within screen bounds - adjusted to consider full paddle height
        leftPaddle->position.y = std::max(paddleDimensions.y/2.0f, 
                                        std::min(leftPaddle->position.y, 
                                        SCREEN_HEIGHT - paddleDimensions.y/2.0f));
        rightPaddle->position.y = std::max(paddleDimensions.y/2.0f, 
                                          std::min(rightPaddle->position.y, 
                                          SCREEN_HEIGHT - paddleDimensions.y/2.0f));

        // Collision handling with paddles
        if (ball->isColliding(*leftPaddle) || ball->isColliding(*rightPaddle)) {
            Event collision_event("collision", currentTime);
            collision_event.parameters["entity1"] = ball;
            collision_event.parameters["entity2"] = (ball->isColliding(*leftPaddle)) ? leftPaddle : rightPaddle;
            collision_event.parameters["collision_type"] = "paddle";
            
            event_manager.raise_event(collision_event);
            
            // Handle immediate physics response
            ball->velocity.x *= -1.1f;
            
            // Calculate paddle collision response
            auto paddle = (ball->isColliding(*leftPaddle)) ? leftPaddle : rightPaddle;
            float relativeIntersectY = (paddle->position.y - ball->position.y) / (paddleDimensions.y / 2.0f);
            float bounceAngle = relativeIntersectY * 0.75f;
            float speed = std::sqrt(ball->velocity.x * ball->velocity.x + ball->velocity.y * ball->velocity.y);
            
            ball->velocity.y = -speed * std::sin(bounceAngle);
        }

        // Ball boundary handling - adjusted to consider ball dimensions
        if (ball->position.y <= ballDimensions.y/2.0f) {
            ball->position.y = ballDimensions.y/2.0f;
            ball->velocity.y *= -1;
        } else if (ball->position.y >= SCREEN_HEIGHT - ballDimensions.y/2.0f) {
            ball->position.y = SCREEN_HEIGHT - ballDimensions.y/2.0f;
            ball->velocity.y *= -1;
        }

        // Ball scoring boundary handling
        if (ball->position.x <= ballDimensions.x/2.0f) {
            // Left boundary (right player scores)
            ball->position = Vector2{SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
            ball->velocity = Vector2{200, 200};
        } else if (ball->position.x >= SCREEN_WIDTH - ballDimensions.x/2.0f) {
            // Right boundary (left player scores)
            ball->position = Vector2{SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
            ball->velocity = Vector2{-200, 200};  // Start moving toward the other player
        }
        // Update scale factor
        updateScaleFactor(scale);
        if (allowScaling && cached_scale != scale) {
            setRenderScale(scale, scale);
            cached_scale = scale;
        }

        // Update entities and process events
        entityManager.updateEntityDeltaTime();
        entityManager.updateEntities(&globalTimeline);
        event_manager.process_events(currentTime);

        // Check for disconnect
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