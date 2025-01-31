#include "client.hpp"
#include <iostream>
#include <thread>

std::atomic<bool> Client::disconnectRequested = false;

Client::Client(EntityManager &entityManager, EntityManager &clientEntityManager)
    : context(1), requester(context, zmq::socket_type::req),
      pusher(context, zmq::socket_type::push),
      subscriber(context, zmq::socket_type::sub),
      peerPublisher(context, zmq::socket_type::pub),
      peerSubscriber1(context, zmq::socket_type::sub),
      peerSubscriber2(context, zmq::socket_type::sub),
      entityManager(entityManager), clientEntityManager(clientEntityManager) {
  dict = std::make_shared<
      std::unordered_map<std::string, std::shared_ptr<Entity>>>();
}

void Client::connectRequester(const std::string &address, int port) {
  requester.connect(address + ":" + std::to_string(port));
}

void Client::connectPusher(const std::string &address, int port) {
  pusher.connect(address + ":" + std::to_string(port));
}

void Client::connectSubscriber(const std::string &address, int port) {
  subscriber.connect(address + ":" + std::to_string(port));
  subscriber.set(zmq::sockopt::subscribe, "");
}

void Client::bindPeerPublisher(const std::string &address, int port) {
  peerPublisher.bind(
      address + ":" +
      std::to_string(port)); // Bind to an address for peer publishing
}

void Client::connectPeerSubscriber1(const std::string &address, int port) {
  peerSubscriber1.connect(
      address + ":" +
      std::to_string(port)); // Connect to another client's publisher
  peerSubscriber1.set(zmq::sockopt::subscribe, "");
}

void Client::connectPeerSubscriber2(const std::string &address, int port) {
  peerSubscriber2.connect(
      address + ":" +
      std::to_string(port)); // Connect to another client's publisher
  peerSubscriber2.set(zmq::sockopt::subscribe, "");
}

bool Client::connectServer(bool isP2P) {
  std::string helloMessage = isP2P ? "Hello_P2P" : "Hello";
  zmq::message_t request(helloMessage.size());
  memcpy(request.data(), helloMessage.c_str(), helloMessage.size());
  requester.send(request, zmq::send_flags::none);

  zmq::message_t reply;

  const int timeout = 5000;
  requester.set(zmq::sockopt::rcvtimeo, timeout);

  if (requester.recv(reply, zmq::recv_flags::none)) {
    std::string received(static_cast<char *>(reply.data()), reply.size());
    clientID = received;
    std::cout << "Client connected with ID: " << clientID << std::endl;

    std::thread subMsgThread(&Client::receiveSubMsg, this);
    subMsgThread.detach();

    if (isP2P) {
      std::thread peerMsgThread(&Client::receivePeerMsg, this);
      peerMsgThread.detach();
    }
    return true;
  } else {
    std::cerr << "Failed to receive reply from server. Check if the server is "
                 "running."
              << std::endl;

    return false;
  }
}

void Client::receivePeerMsg() {
  while (true) {
    bool shouldUpdateEntities = false;
    zmq::message_t peerMsg1, peerMsg2;

    // Receive data from both peers, don't block if no message
    (void)peerSubscriber1.recv(peerMsg1, zmq::recv_flags::dontwait);
    (void)peerSubscriber2.recv(peerMsg2, zmq::recv_flags::dontwait);

    std::string recvMsg1(static_cast<char *>(peerMsg1.data()), peerMsg1.size());
    std::string recvMsg2(static_cast<char *>(peerMsg2.data()), peerMsg2.size());

    // Check if messages are not empty
    if (!recvMsg1.empty()) {
      deserializeClientEntityMap(recvMsg1);
      shouldUpdateEntities = true;
    }

    if (!recvMsg2.empty()) {
      deserializeClientEntityMap(recvMsg2);
      shouldUpdateEntities = true;
    }

    if (shouldUpdateEntities)
      updateOtherEntities();
  }
}

void Client::receiveSubMsg() {
  while (!terminateThreads.load()) {
    zmq::message_t subMsg;
    (void)subscriber.recv(subMsg, zmq::recv_flags::dontwait);
    std::string recvMsg(static_cast<char *>(subMsg.data()), subMsg.size());
    deserializeClientEntityMap(recvMsg);
    // update the entities according to the subMsg
    updateOtherEntities();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}

void Client::start(bool isP2P) {
  std::string message = clientID + " ";

  if (disconnectRequested.load()) {
    message += "disconnect";
  } else {
    // Serialize positions of all entities in the EntityManager
    for (const auto &entity : entityManager.getEntities()) {
      Vector2 position = EventManager::getInstance().get_replay_only_mode()
          ? Vector2{10000, 10000}:
              entity->getPosition();
      message += std::to_string(entity->getID()) + " " +
                 std::to_string(position.x) + " " + std::to_string(position.y) +
                 " ";
    }
  }

  // If not in P2P mode, also send to server
  if (!isP2P) {
    zmq::message_t request(message.size());
    memcpy(request.data(), message.c_str(), message.size());
    pusher.send(request, zmq::send_flags::none);
  } else {
    zmq::message_t peerMessage(message.size());
    memcpy(peerMessage.data(), message.c_str(), message.size());
    peerPublisher.send(peerMessage, zmq::send_flags::none);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(45));
}

void Client::deserializeClientEntityMap(const std::string &pubMsg) {
  std::stringstream ss(pubMsg);
  std::string clientBlock;

  // Clear the entity map if no data is received for a client
  std::unordered_set<std::string> receivedClientIDs;

  while (std::getline(ss, clientBlock, '#')) {
    std::stringstream clientStream(clientBlock);
    std::string clientId;
    clientStream >> clientId;

    // Store received client IDs
    receivedClientIDs.insert(clientId);

    std::unordered_map<int, std::pair<float, float>> entityMap;
    int entityId;
    float x, y;

    while (clientStream >> entityId >> x >> y) {
      entityMap[entityId] = {x, y};
    }

    if (entityMap.size() > 0) {
      clientEntityMap[clientId] = entityMap; // Update or add new entities
    }
  }

  // Remove entities for clients that are no longer sending data
  for (auto it = clientEntityMap.begin(); it != clientEntityMap.end();) {
    if (receivedClientIDs.find(it->first) == receivedClientIDs.end()) {
      // Client is no longer active, remove their entities
      clientEntityMap.erase(it++); // Erase the client and its entities
    } else {
      ++it; // Move to the next client
    }
  }
}

void Client::printEntityMap() {
  // Debugging function to print out the state of clientEntityMap
  for (const auto &client : clientEntityMap) {
    std::cout << "Client " << client.first << " entities:" << std::endl;
    for (const auto &entity : client.second) {
      std::cout << "  Entity " << entity.first << " -> (" << entity.second.first
                << ", " << entity.second.second << ")" << std::endl;
    }
  }
}

void Client::updateOtherEntities() {
  // Remove current client's entities from clientEntityMap
  clientEntityMap.erase(clientID);

  // Track which entities are active in this update
  activeEntities.clear();

  // Iterate over received data and update entities
  for (const auto &clientData : clientEntityMap) {
    std::string cID = clientData.first;

    for (const auto &entityData : clientData.second) {
      int entityID = entityData.first;
      Vector2 newPosition{entityData.second.first, entityData.second.second};
      std::string identifier = cID + "_" + std::to_string(entityID);

      // Mark entity as active
      activeEntities.insert(identifier);

      // Add or update entity in the dictionary
      if (dict->find(identifier) == dict->end()) {
        Vector2 position{100, 100};
        Vector2 dimensions{40, 40};
        SDL_Color color = {0, 0, 0, 255};
        auto newEntity = std::make_shared<Entity>(position, dimensions, color,
                                                  &globalTimeline, 2);
        (*dict)[identifier] = newEntity;
        clientEntityManager.addEntities(newEntity);
      }
      if ((*dict)[identifier]->position.x != newPosition.x ||
          (*dict)[identifier]->position.y != newPosition.y) {

        // (*dict)[identifier]->position = newPosition;
        Event update_position_event("update_position",
                                    globalTimeline.getTime());
        update_position_event.parameters["dict"] = dict;
        update_position_event.parameters["identifier"] = identifier;
        update_position_event.parameters["newPosition"] = newPosition;
        EventManager &em = EventManager::getInstance();
        em.raise_event(update_position_event);
      }
    }
  }

  // Now remove entities not in the current active set
  for (auto it = dict->begin(); it != dict->end();) {
    if (activeEntities.find(it->first) == activeEntities.end()) {
      // Entity is no longer active, remove it
      clientEntityManager.removeEntity(it->second);
      it = dict->erase(it); // Erase returns the next iterator
    } else {
      ++it; // Move to the next entity
    }
  }
}

void Client::cleanup() {
  requester.close();
  pusher.close();
  subscriber.close();
  peerPublisher.close();
  peerSubscriber1.close();
  peerSubscriber2.close();
}