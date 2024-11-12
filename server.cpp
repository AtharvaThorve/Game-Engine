#include "server.hpp"

Server::Server(EventManager *em, Timeline *timeline)
    : context(1), responder(context, zmq::socket_type::rep),
      puller(context, zmq::socket_type::pull),
      publisher(context, zmq::socket_type::pub), em(em), timeline(timeline) {
  clientEntityMap = std::make_shared<std::unordered_map<
      std::string, std::unordered_map<int, std::pair<float, float>>>>();
  connectedClientIDs = std::make_shared<std::unordered_set<std::string>>();
}

void Server::bindResponder(const std::string &address, int port) {
  responder.bind(address + ":" + std::to_string(port));
}

void Server::bindPuller(const std::string &address, int port) {
  puller.bind(address + ":" + std::to_string(port));
}

void Server::bindPublisher(const std::string &address, int port) {
  publisher.bind(address + ":" + std::to_string(port));
}

void Server::raiseClientDisconnectEvent(std::string clientID) {
  Event disconnectEvent("disconnect", timeline->getTime());
  disconnectEvent.parameters["clientEntityMap"] = clientEntityMap;
  disconnectEvent.parameters["clientID"] = clientID;
  disconnectEvent.parameters["connectedClientIDs"] = connectedClientIDs;
  em->raise_event(disconnectEvent);
}

std::string Server::generateUniqueClientID() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(100, 199);

  std::string clientID;
  do {
    clientID = std::to_string(dis(gen)); // Generate random number
  } while (connectedClientIDs->find(clientID) != connectedClientIDs->end());

  connectedClientIDs->insert(clientID); // Mark the client ID as used
  return clientID;
}

void Server::start() {
  std::cout << "Server started, waiting for clients..." << std::endl;

  bool broadCastMsgThreadStarted = false;

  while (true) {
    if (connectedClientIDs->size() > 0 && !broadCastMsgThreadStarted) {
      broadCastMsgThreadStarted = true;
      std::thread broadCastMssgThread(&Server::broadcastMsg, this);
      broadCastMssgThread.detach();
    }
    // Req/Rep setup for Hello message and clientId
    zmq::message_t request;
    (void)responder.recv(request, zmq::recv_flags::dontwait);
    std::string received(static_cast<char *>(request.data()), request.size());

    if (received == "Hello") {
      std::string clientID = generateUniqueClientID();
      zmq::message_t reply(clientID.size());
      memcpy(reply.data(), clientID.c_str(), clientID.size());
      responder.send(reply, zmq::send_flags::dontwait);

      std::thread clientThread(&Server::handle_client_thread, this, clientID);
      clientThread.detach();
    } else if (received == "Hello_P2P") {
      std::string clientID = generateUniqueClientID();
      zmq::message_t reply(clientID.size());
      memcpy(reply.data(), clientID.c_str(), clientID.size());
      responder.send(reply, zmq::send_flags::dontwait);
    }
    em->process_events(timeline->getTime());
  }
}

void Server::handle_client_thread(const std::string &clientID) {
  const std::chrono::seconds timeoutDuration(5);
  auto lastReceivedTime = std::chrono::system_clock::now();
  bool disconnectMessageReceived = false;

  while (connectedClientIDs->find(clientID) != connectedClientIDs->end()) {
    zmq::message_t positionData;
    std::string receivedData;

    zmq::recv_result_t result;
    {
      std::unique_lock<std::mutex> lock(clientMutex);
      result = puller.recv(positionData, zmq::recv_flags::dontwait);
    }

    // Process received data
    if (result) {
      lastReceivedTime =
          std::chrono::system_clock::now(); // Update time on receiving data
      receivedData = std::string(static_cast<char *>(positionData.data()),
                                 positionData.size());
      std::istringstream stream(receivedData);

      std::string tempClientID;
      stream >> tempClientID;

      if (tempClientID != clientID)
        continue;

      std::unordered_map<int, std::pair<float, float>> entityPositionMap;
      disconnectMessageReceived =
          parseString(receivedData, clientID, entityPositionMap);

      if (disconnectMessageReceived)
        break;

      if (connectedClientIDs->find(clientID) != connectedClientIDs->end())
        (*clientEntityMap)[clientID] = entityPositionMap;
    }

    // Check for timeout
    auto currentTime = std::chrono::system_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(
        currentTime - lastReceivedTime);

    if (elapsedTime >= timeoutDuration) {
      break; // Exit the loop if timeout has been reached
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  // Check if a disconnect message was received
  if (disconnectMessageReceived) {
    std::cout << "Disconnect command received for client: " << clientID
              << std::endl;
  } else {
    std::cout << "Client disconnected due to timeout: " << clientID
              << std::endl;
  }
  raiseClientDisconnectEvent(clientID);
}

bool Server::parseString(
    const std::string &input, const std::string &clientID,
    std::unordered_map<int, std::pair<float, float>> &entityPositionMap) {
  std::istringstream stream(input);

  std::string tempClientID;
  stream >> tempClientID;

  // Check if the message is a disconnect message
  std::string command;
  stream >> command;

  if (command == "disconnect" &&
      connectedClientIDs->find(clientID) != connectedClientIDs->end()) {
    return true;
  } else {
    stream.putback(command[0]);
  }

  int entityID;
  float x, y;

  while (stream >> entityID >> x >> y) {
    entityPositionMap[entityID] = std::make_pair(x, y);
  }

  return false;
}

void Server::printEntityMap() {
  // Debugging function to print out the state of clientEntityMap
  for (const auto &client : *clientEntityMap) {
    std::cout << "Client " << client.first << " entities:" << std::endl;
    for (const auto &entity : client.second) {
      std::cout << "  Entity " << entity.first << " -> (" << entity.second.first
                << ", " << entity.second.second << ")" << std::endl;
    }
  }
}

void Server::updateClientEntityMap(EntityManager &serverEntityManager) {
  std::unordered_map<int, std::pair<float, float>> entityPositionMap;
  for (const auto &entity : serverEntityManager.getEntities()) {
    entityPositionMap[entity->getID()] =
        std::make_pair(entity->position.x, entity->position.y);
  }
  (*clientEntityMap)["1"] = entityPositionMap;
}

void Server::broadcastMsg() {
  while (true) {
    std::string pubMsg = generatePubMsg();
    zmq::message_t broadcastMsg(pubMsg.size());
    memcpy(broadcastMsg.data(), pubMsg.data(), pubMsg.size());
    publisher.send(broadcastMsg, zmq::send_flags::dontwait);

    std::cout << pubMsg << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}

std::string Server::generatePubMsg() {
  std::stringstream pubMsg;
  std::unique_lock<std::mutex> lock(clientMutex);
  for (const auto &clientPair : *clientEntityMap) {
    const std::string &clientId = clientPair.first;
    const auto &entityMap = clientPair.second;

    pubMsg << clientId << " ";

    for (const auto &entityPair : entityMap) {
      int entityId = entityPair.first;
      float x = entityPair.second.first;
      float y = entityPair.second.second;

      pubMsg << entityId << " " << x << " " << y << " ";
    }
    pubMsg << "# ";
  }

  return pubMsg.str();
}
