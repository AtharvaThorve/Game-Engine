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

void usage() {
  // Usage
  std::cout << "\n"
            << "JavaScript <--> C++ Integration Example\n"
            << "---------------------------------------\n"
            << "\n"
            << "Commands:\n"
            << "\th: run hello_world.js\n"
            << "\tk: To kill the player\n"
            << std::endl;
}

void doClientGame() {
  std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
  v8::V8::InitializePlatform(platform.get());
  v8::V8::InitializeICU();
  v8::V8::Initialize();
  v8::Isolate::CreateParams create_params;
  create_params.array_buffer_allocator =
      v8::ArrayBuffer::Allocator::NewDefaultAllocator();
  v8::Isolate *isolate = v8::Isolate::New(create_params);

  {

    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);

    global->Set(isolate, "print",
                v8::FunctionTemplate::New(isolate, v8helpers::Print));
    global->Set(
        isolate, "gethandle",
        v8::FunctionTemplate::New(isolate, ScriptManager::getHandleFromScript));
    bind_event_manager(isolate, global);

    v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, global);
    v8::Context::Scope context_scope(context);

    ScriptManager *sm = new ScriptManager(isolate, context);

    sm->addScript("hello_world", "scripts/hello_world.js");
    sm->addScript("player", "scripts/player.js");

    usage();

    initSDL();
    float scale = 1.0f;
    float cached_scale = scale;

    // Player setup
    auto player = std::make_shared<Entity>("entities/player.json", &globalTimeline);
    player->maxVelocity = Vector2{100, 300};
    player->isMovable = true;
    player->isHittable = true;
    player->isAffectedByGravity = true;

    expose_entity_to_v8(player, isolate, context);

    EntityManager playerEntityManager;
    playerEntityManager.addEntities(player);

    EntityManager entityManager;
    EntityManager clientEntityManager;

    // Ground platforms
    SDL_Color platformColor = {139, 69, 19, 255}; // Brown color for platforms
    auto ground1 =
        std::make_shared<Entity>(Vector2{-500, 550}, Vector2{2000, 50},
                                 platformColor, &globalTimeline, 2);
    auto ground2 =
        std::make_shared<Entity>(Vector2{1600, 550}, Vector2{2000, 50},
                                 platformColor, &globalTimeline, 2);
    ground1->isHittable = true;
    ground2->isHittable = true;

    // Add all platforms to entityManager
    entityManager.addEntities(ground1, ground2);
    entityManager.addEntities(player);

    // Death zones (lava pits and invisible gap)
    SDL_Color lavaColor = {255, 69, 0, 255}; // Orange-red for lava
    SDL_Color invisibleColor = {135, 206, 235, 255};

    // Lava on top of a platform
    auto lavaPlatform = std::make_shared<Entity>(
        Vector2{1100, 500}, Vector2{200, 30}, lavaColor, &globalTimeline, 2);
    lavaPlatform->isHittable = true;

    // Invisible death zone in the gap between ground platforms
    auto invisibleDeathZone =
        std::make_shared<Entity>(Vector2{1500, 700}, Vector2{100, 200},
                                 invisibleColor, &globalTimeline, 2);

    entityManager.addEntities(lavaPlatform);
    entityManager.addDeathZones(invisibleDeathZone, lavaPlatform);

    // Spawn points
    auto spawnPoint1 =
        std::make_shared<Entity>(Vector2{100, 400}, Vector2{1, 1},
                                 SDL_Color{0, 0, 0, 0}, &globalTimeline, 1);
    auto spawnPoint2 =
        std::make_shared<Entity>(Vector2{500, 200}, Vector2{1, 1},
                                 SDL_Color{0, 0, 0, 0}, &globalTimeline, 1);

    int worldWidth = 5000;
    int worldHeight = 2000;

    Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT);

    EventManager &event_manager = EventManager::getInstance();

    std::vector<std::shared_ptr<EntityManager>> entityManagers = {
        std::shared_ptr<EntityManager>(std::addressof(entityManager)),
        std::shared_ptr<EntityManager>(std::addressof(clientEntityManager))};

    CollisionHandler collision_handler(&globalTimeline);
    collision_handler.register_collision_handler(
        "platform", collision_utils::handlePlatformCollision);
    collision_handler.register_collision_handler(
        "death_zone", collision_utils::handleDeathZoneCollision);

    event_manager.register_handler("collision", &collision_handler);

    event_manager.register_handler("death", new DeathHandler(&globalTimeline));

    RespawnHandler respawn_handler(&globalTimeline);
    respawn_handler.add_spawn_points(spawnPoint1, spawnPoint2);

    event_manager.register_handler("respawn", &respawn_handler);

    event_manager.register_handler("input", new InputHandler(&globalTimeline));

    event_manager.register_handler("move",
                                   new MovementHandler(&globalTimeline));

    event_manager.register_handler("update_position",
                                   new PositionHandler(&globalTimeline));

    ReplayRecorder replay_recorder(&globalTimeline, entityManagers);
    event_manager.register_wildcard_handler(&replay_recorder);

    std::thread networkThread(runClient, std::ref(playerEntityManager),
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

      doInput(player, &globalTimeline, sm, 50.0f, 200.0f);

      camera.update(*player, worldWidth, worldHeight);

      // Sky blue background
      prepareScene(SDL_Color{135, 206, 235, 255});

      entityManager.drawEntities(camera.position.x, camera.position.y);
      clientEntityManager.drawEntities(camera.position.x, camera.position.y);

      updateScaleFactor(scale);
      if (allowScaling && cached_scale != scale) {
        setRenderScale(scale, scale);
        cached_scale = scale;
      }

      // Check death and respawn
      if (entityManager.checkPlayerDeath(player)) {
        Event death_event("death", globalTimeline.getTime());
        death_event.parameters["player"] = player;
        event_manager.raise_event(death_event);
      }

      if (player->isColliding(*ground1)) {
        Event collision_event("collision", globalTimeline.getTime());
        collision_event.parameters["entity1"] = player;
        collision_event.parameters["entity2"] = ground1;
        collision_event.parameters["collision_type"] = "platform";
        event_manager.raise_event(collision_event);
      }
      if (player->isColliding(*ground2)) {
        Event collision_event("collision", globalTimeline.getTime());
        collision_event.parameters["entity1"] = player;
        collision_event.parameters["entity2"] = ground2;
        collision_event.parameters["collision_type"] = "platform";
        event_manager.raise_event(collision_event);
      }

      // Need to do this after collision
      entityManager.updateEntityDeltaTime();
      entityManager.updateMovementPatternEntities();
      entityManager.updateEntities(&globalTimeline);

      event_manager.process_events(globalTimeline.getTime());

      if (Client::disconnectRequested.load()) {
        terminateThreads.store(true);
        break;
      }

      presentScene();
    }

    gravityThread.join();
    networkThread.join();
  }

  isolate->Dispose();
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
  delete create_params.array_buffer_allocator;

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