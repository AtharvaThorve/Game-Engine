#include "Server.hpp"

Server::Server(): 
    context(1), 
    responder(context, zmq::socket_type::rep),
    puller(context, zmq::socket_type::pull), 
    publisher(context, zmq::socket_type::pub) 
{}

void Server::bindResponder(const std::string& address, int port) {
    responder.bind(address + ":" + std::to_string(port));
}

void Server::bindPuller(const std::string& address, int port) {
    puller.bind(address + ":" + std::to_string(port));
}

void Server::bindPublisher(const std::string& address, int port) {
    publisher.bind(address + ":" + std::to_string(port));
}

std::string Server::generateUniqueClientID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100, 199);

    std::string clientID;
    do {
        clientID = std::to_string(dis(gen));  // Generate random number
    } while (connectedClientIDs.find(clientID) != connectedClientIDs.end());

    connectedClientIDs.insert(clientID);  // Mark the client ID as used
    return clientID;
}

void Server::start() {
    std::cout << "Server started, waiting for clients..." << std::endl;

    while (true) {
        // Req/Rep setup for Hello message and clientId
        zmq::message_t request;
        responder.recv(request, zmq::recv_flags::none);
        std::string received(static_cast<char*>(request.data()), request.size());
        std::cout << "Received connection request: " << received << std::endl;

        if (received == "Hello") {
            std::string clientID = generateUniqueClientID();
            zmq::message_t reply(clientID.size());
            memcpy(reply.data(), clientID.c_str(), clientID.size());
            responder.send(reply, zmq::send_flags::none);
            std::cout << "Assigned client ID: " << clientID << std::endl;

            std::thread clientThread(&Server::handle_client_thread, this, clientID);
            clientThread.detach();
        }
    }
}

void Server::handle_client_thread(const std::string& clientID) {
    std::cout << "Client " << clientID << " thread started, waiting for data..." << std::endl;

    while (true) {
        zmq::message_t positionData;

        puller.recv(positionData, zmq::recv_flags::none);
        std::string receivedData(static_cast<char*>(positionData.data()), positionData.size());
        
        std::unordered_map<int, std::pair<float, float>> entityPositionMap;
        parseString(receivedData, clientID, entityPositionMap);
        {
            std::lock_guard<std::mutex> lock(clientMutex);
            std::cout << "Received data from client " << clientID << ": " << receivedData << std::endl;

            clientEntityMap[clientID] = entityPositionMap;
            printEntityMap();
        }
        broadcastMsg();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void Server::handle_client(const std::string& received_data) {
    // For now, just send back the same data we received from the client
    zmq::message_t reply_message(received_data.size());
    memcpy(reply_message.data(), received_data.c_str(), received_data.size());

    responder.send(reply_message, zmq::send_flags::none);

    std::cout << "Sent data back to client: " << received_data << std::endl;
}

void Server::parseString(const std::string& input, const std::string& clientID, std::unordered_map<int, std::pair<float, float>>& entityPositionMap) {
    std::istringstream stream(input);

    std::string tempClientID;
    stream >> tempClientID;

    if (tempClientID != clientID) {
        std::cerr << "Client ID mismatch. Expected: " << clientID << " but got: " << tempClientID << std::endl;
        return;
    }

    int entityID;
    float x, y;

    while (stream >> entityID >> x >> y) {
        entityPositionMap[entityID] = std::make_pair(x, y);
    }
    std::cout << entityPositionMap.size() << std::endl;
}

void Server::printEntityMap() {
    // Debugging function to print out the state of clientEntityMap
    for (const auto& client : clientEntityMap) {
        std::cout << "Client " << client.first << " entities:" << std::endl;
        for (const auto& entity : client.second) {
            std::cout << "  Entity " << entity.first << " -> (" << entity.second.first << ", " << entity.second.second << ")" << std::endl;
        }
    }
}

void Server::broadcastMsg() {    
    std::string pubMsg = generatePubMsg();
    std::cout << "Pub Msg: " + pubMsg << std::endl;
    zmq::message_t broadcastMsg(pubMsg.size());
    memcpy(broadcastMsg.data(), pubMsg.data(), pubMsg.size());
    publisher.send(broadcastMsg, zmq::send_flags::none);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
}

std::string Server::generatePubMsg() {
    std::stringstream pubMsg;

    for (const auto& clientPair : clientEntityMap) {
        const std::string& clientId = clientPair.first;
        const auto& entityMap = clientPair.second;

        pubMsg << clientId << " ";

        for (const auto& entityPair : entityMap) {
            int entityId = entityPair.first;
            float x = entityPair.second.first;
            float y = entityPair.second.second;

            pubMsg << entityId << " " << x << " " << y << " ";
        }
        pubMsg << "# ";
    }

    return pubMsg.str();
}