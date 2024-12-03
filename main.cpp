#include "main.hpp"

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
  int worldWidth = 1280;
  int worldHeight = 720;

  auto player = std::make_shared<Entity>(
      Vector2{SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 100}, Vector2{50, 50},
      SDL_Color{0, 255, 0, 255}, &globalTimeline, 1);
  player->isMovable = true;
  player->isHittable = true;
  player->maxVelocity = {100, 0};

  entityManager.addEntity(player);

  std::vector<std::shared_ptr<Entity>> aliens;
  int rows = 3;
  int cols = 5;

  int alienWidth = 40;
  int alienHeight = 30;
  int padding = (SCREEN_WIDTH - (cols * alienWidth)) / (cols + 1);

  std::srand(static_cast<unsigned>(std::time(nullptr)));
  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      SDL_Color brickColor = {static_cast<Uint8>(std::rand() % 256),
                              static_cast<Uint8>(std::rand() % 256),
                              static_cast<Uint8>(std::rand() % 256), 255};
      int x = padding + col * (alienWidth + padding);
      int y = row * (alienHeight + padding);

      auto alien = std::make_shared<Entity>(Vector2{x, y},
                                            Vector2{alienWidth, alienHeight},
                                            brickColor, &globalTimeline, 1);

      alien->isHittable = true;
      alien->isMovable = true;
      alien->velocity = Vector2{50, 0};
      aliens.push_back(alien);
      entityManager.addEntity(alien);
    }
  }

  Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT);

  EventManager &event_manager = EventManager::getInstance();

  std::vector<std::shared_ptr<EntityManager>> entityManagers = {
      std::shared_ptr<EntityManager>(std::addressof(entityManager))};

  CollisionHandler collision_handler(&globalTimeline);
  collision_handler.register_collision_handler(
      "playerBulletAlien", collision_utils::handlePlayerBulletAlienCollision);
  collision_handler.register_collision_handler(
      "alienBulletPlayer", collision_utils::handleAlienBulletPlayerCollision);

  event_manager.register_handler("collision", &collision_handler);

  event_manager.register_handler("death", new DeathHandler(&globalTimeline));

  event_manager.register_handler("input", new InputHandler(&globalTimeline));

  event_manager.register_handler("move", new MovementHandler(&globalTimeline));

  event_manager.register_handler("update_position",
                                 new PositionHandler(&globalTimeline));

  ReplayRecorder replay_recorder(&globalTimeline, entityManagers);
  event_manager.register_wildcard_handler(&replay_recorder);

  std::thread gravityThread(applyGravityOnEntities, std::ref(physicsSystem),
                            std::ref(entityManager));

  std::unordered_set<std::shared_ptr<Entity>> playerBullets;
  std::unordered_set<std::shared_ptr<Entity>> alienBullets;

  while (!terminateThreads.load()) {
    doInput(player, &globalTimeline, entityManager, playerBullets, 200000.0f,
            0.0f, 0.0f);

    camera.update(*player, worldWidth, worldHeight);

    // Sky blue background
    prepareScene(SDL_Color{135, 206, 235, 255});

    entityManager.drawEntities(camera.position.x, camera.position.y);

    updateScaleFactor(scale);
    if (allowScaling && cached_scale != scale) {
      setRenderScale(scale, scale);
      cached_scale = scale;
    }

    bool shouldMoveDown = false;
    for (auto &alien : aliens) {
      if (alien->position.x <= 0 ||
          alien->position.x + alien->dimensions.x >= worldWidth) {
        shouldMoveDown = true;
        break;
      }
    }
    if (shouldMoveDown) {
      for (auto &alien : aliens) {
        Vector2 alienPos = alien->getPosition();
        alien->velocity.x *= -1;
        alien->setPosition(Vector2{alienPos.x, alienPos.y + 5});
      }
    }

    if (std::rand() % 150 < 1) {
      auto shooter = aliens[std::rand() % aliens.size()];
      if (shooter->isHittable) {
        auto alienBullet = std::make_shared<Entity>(
            Vector2{shooter->position.x + shooter->dimensions.x / 2 - 5,
                    shooter->position.y + shooter->dimensions.y},
            Vector2{10, 20}, SDL_Color{255, 0, 0, 255}, &globalTimeline, 1);
        alienBullet->isMovable = true;
        alienBullet->velocity = Vector2{0, 200};
        entityManager.addEntity(alienBullet);
        alienBullets.insert(alienBullet);
      }
    }

    for (auto it = alienBullets.begin(); it != alienBullets.end();) {
      auto &alienBullet = *it;
      if (alienBullet->isColliding(*player)) {
        exit(0);
      } else if (alienBullet->position.y >= worldHeight) {
        entityManager.removeEntity(alienBullet);
        it = alienBullets.erase(it);
      } else {
        ++it;
      }
    }

    std::unordered_set<std::shared_ptr<Entity>> aliensToRemove;
    std::unordered_set<std::shared_ptr<Entity>> bulletsToRemove;

    for (auto bulletIt = playerBullets.begin();
         bulletIt != playerBullets.end();) {
      auto &playerBullet = *bulletIt;
      bool bulletRemoved = false;

      for (auto &alien : aliens) {
        if (alien->isColliding(*playerBullet)) {
          Event collision_event("collision", globalTimeline.getTime());
          collision_event.parameters["entity1"] = playerBullet;
          collision_event.parameters["entity2"] = alien;
          collision_event.parameters["collision_type"] = "playerBulletAlien";
          EventManager &em = EventManager::getInstance();
          em.raise_event(collision_event);

          aliensToRemove.insert(alien);
          bulletsToRemove.insert(playerBullet);
          bulletRemoved = true;
        }
      }

      if (playerBullet->position.y < 0) {
        bulletsToRemove.insert(playerBullet);
      }

      ++bulletIt;
    }

    for (auto &alien : aliensToRemove) {
      auto alienIt = std::find(aliens.begin(), aliens.end(), alien);
      if (alienIt != aliens.end()) {
        aliens.erase(alienIt);
        entityManager.removeEntity(alien);
      }
    }

    for (auto &bullet : bulletsToRemove) {
      auto bulletIt =
          std::find(playerBullets.begin(), playerBullets.end(), bullet);
      if (bulletIt != playerBullets.end()) {
        playerBullets.erase(bulletIt);
        entityManager.removeEntity(bullet);
      }
    }

    collision_utils::handlePlayerWallCollision(player, worldWidth);

    // Need to do this after collision
    entityManager.updateEntityDeltaTime();
    entityManager.updateMovementPatternEntities();
    entityManager.updateEntities(&globalTimeline);

    event_manager.process_events(globalTimeline.getTime());

    if (Client::disconnectRequested.load() ||
        std::all_of(aliens.begin(), aliens.end(),
                    [](auto &alien) { return !alien->isHittable; })) {
      terminateThreads.store(true);
      break;
    }

    presentScene();
  }

  gravityThread.join();
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