#include "Server.hpp"

Server::Server(): context(1), responder(context, ZMQ_REP) {
}

void Server::bindResponder(const std::string& address, int port){
    responder.bind(address + ":" + std::to_string(port));
}

void Server::bindPuller(const std::string& address, int port) {
    puller.bind(address + ":" + std::to_string(port));
}

void Server::bindPublisher(const std::string& address, int port) {
    publisher.bind(address + ":" + std::to_string(port));
}

void Server::start() {
    std::cout << "Server started, waiting for clients..." << std::endl;

    while (true) {
        zmq::message_t request;

        // Receive client message (serialized entity data)
        responder.recv(request, zmq::recv_flags::none);
        std::string received(static_cast<char*>(request.data()), request.size());
        int64_t clientId;
        std::map<int, std::pair<float, float>> entityPositionMap;
        parseString(received, clientId, entityPositionMap);

        std::cout << "Received data from client: " << received << std::endl;

        // Process the received data (for now, just echo it back)
        handle_client(received);

        // Simulate processing delay
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void Server::handle_client(const std::string& received_data) {
    // For now, just send back the same data we received from the client
    zmq::message_t reply_message(received_data.size());
    memcpy(reply_message.data(), received_data.c_str(), received_data.size());

    responder.send(reply_message, zmq::send_flags::none);

    std::cout << "Sent data back to client: " << received_data << std::endl;
}

void Server::parseString(const std::string& input, int64_t& clientID, std::map<int, std::pair<float, float>>& entityPositionMap) {
    std::istringstream stream(input);
    stream >> clientID; 

    int entityID;
    float x, y;

    while (stream >> entityID >> x >> y) {
        entityPositionMap[entityID] = std::make_pair(x, y);
    }
    std::cout << entityPositionMap.size() << std::endl;
}