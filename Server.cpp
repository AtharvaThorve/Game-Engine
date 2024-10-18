#include "Server.hpp"

Server::Server()
    : context(1), responder(context, zmq::socket_type::rep),
      puller(context, zmq::socket_type::pull),
      publisher(context, zmq::socket_type::pub) {}

void Server::bindResponder(const std::string &address, int port) {
  responder.bind(address + ":" + std::to_string(port));
}

void Server::bindPuller(const std::string &address, int port) {
  puller.bind(address + ":" + std::to_string(port));
}

void Server::bindPublisher(const std::string &address, int port) {
  publisher.bind(address + ":" + std::to_string(port));
}

std::string Server::generateUniqueClientID() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(100, 199);

  std::string clientID;
  do {
    clientID = std::to_string(dis(gen)); // Generate random number
  } while (connectedClientIDs.find(clientID) != connectedClientIDs.end());

  connectedClientIDs.insert(clientID); // Mark the client ID as used
  return clientID;
}

void Server::start() {
  std::cout << "Server started, waiting for clients..." << std::endl;

  bool broadCastMsgThreadStarted = false;

  while (true) {
    if (connectedClientIDs.size() > 0 && !broadCastMsgThreadStarted) {
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
  }
}

void Server::handle_client_thread(const std::string &clientID) {
  // Define the timeout duration (e.g., 5 seconds)
  const std::chrono::seconds timeoutDuration(5);

  // Initialize the last received time to the current time
  auto lastReceivedTime = std::chrono::steady_clock::now();

  while (true) {
    zmq::message_t positionData;
    std::string receivedData;

    // Attempt to receive data from the client
    {
      std::unique_lock<std::mutex> lock(clientMutex);
      zmq::recv_result_t result =
          puller.recv(positionData, zmq::recv_flags::dontwait);

      if (result) {
        // Update the last received time when data is received
        lastReceivedTime = std::chrono::steady_clock::now();

        // Process the received data
        receivedData = std::string(static_cast<char *>(positionData.data()),
                                   positionData.size());
        std::istringstream stream(receivedData);

        std::string tempClientID;
        stream >> tempClientID;

        if (tempClientID != clientID)
          continue;

        std::unordered_map<int, std::pair<float, float>> entityPositionMap;
        parseString(receivedData, clientID, entityPositionMap);

        clientEntityMap[clientID] = entityPositionMap;
      }
    }

    // Check if the client has not sent data within the timeout period
    auto currentTime = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(
            currentTime - lastReceivedTime) >= timeoutDuration) {
      if (clientEntityMap.find(clientID) != clientEntityMap.end()) {
        clientEntityMap.erase(clientID);
      }
      std::cout << "Client " << clientID
                << " has stopped sending data for 5 seconds or more. Marking "
                   "as disconnected."
                << std::endl;
      break;
    }
  }
}

void Server::parseString(
    const std::string &input, const std::string &clientID,
    std::unordered_map<int, std::pair<float, float>> &entityPositionMap) {
  std::istringstream stream(input);

  std::string tempClientID;
  stream >> tempClientID;

  int entityID;
  float x, y;

  while (stream >> entityID >> x >> y) {
    entityPositionMap[entityID] = std::make_pair(x, y);
  }
  // std::cout << entityPositionMap.size() << std::endl;
}

void Server::printEntityMap() {
  // Debugging function to print out the state of clientEntityMap
  for (const auto &client : clientEntityMap) {
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
  clientEntityMap["1"] = entityPositionMap;
}

void Server::broadcastMsg() {
  while (true) {
    std::string pubMsg = generatePubMsg();
    zmq::message_t broadcastMsg(pubMsg.size());
    memcpy(broadcastMsg.data(), pubMsg.data(), pubMsg.size());
    publisher.send(broadcastMsg, zmq::send_flags::dontwait);

    // std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}

std::string Server::generatePubMsg() {
  std::stringstream pubMsg;

  for (const auto &clientPair : clientEntityMap) {
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