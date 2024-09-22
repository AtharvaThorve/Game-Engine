#include "Client.hpp"
#include <iostream>
#include <thread>

Client::Client(EntityManager& entityManager)
    : context(1), requester(context, zmq::socket_type::req), pusher(context, zmq::socket_type::push), subscriber(context, zmq::socket_type::sub), entityManager(entityManager) {
}

void Client::connectRequester(const std::string& address, int port) {
    requester.connect(address + ":" + std::to_string(port));
}

void Client::connectPusher(const std::string& address, int port) {
    pusher.connect(address + ":" + std::to_string(port));
}

void Client::connectSubscriber(const std::string& address, int port) {
    subscriber.connect(address + ":" + std::to_string(port));
    subscriber.set(zmq::sockopt::subscribe, "");
}

void Client::connectServer() {
    std::string helloMessage = "Hello";
    zmq::message_t request(helloMessage.size());
    memcpy(request.data(), helloMessage.c_str(), helloMessage.size());
    requester.send(request, zmq::send_flags::none);

    zmq::message_t reply;
    requester.recv(reply, zmq::recv_flags::none);
    std::string received(static_cast<char*>(reply.data()), reply.size());
    clientID = received;
    std::cout << "Client connected with ID: " << clientID << std::endl;
}

void Client::start() {
    while (true) {
        // Serialize positions of all entities in the EntityManager
        std::string message = clientID + " ";
        for (const auto& entity : entityManager.getEntities()) {
            message += std::to_string(entity->getID()) + " " + std::to_string(entity->position.x) + " " + std::to_string(entity->position.y) + " ";
        }

        zmq::message_t request(message.size());
        memcpy(request.data(), message.c_str(), message.size());
        pusher.send(request, zmq::send_flags::none);

        printEntityMap();
        zmq::message_t subMsg;
        subscriber.recv(subMsg, zmq::recv_flags::none);
        std::string recvMsg(static_cast<char*>(subMsg.data()), subMsg.size());
        deserializeClientEntityMap(recvMsg);
        printEntityMap();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void Client::deserializeClientEntityMap(const std::string& pubMsg) {
    std::stringstream ss(pubMsg);
    std::string clientBlock;

    while (std::getline(ss, clientBlock, '#')) {
        std::stringstream clientStream(clientBlock);
        std::string clientId;
        clientStream >> clientId;

        std::unordered_map<int, std::pair<float, float>> entityMap;
        int entityId;
        float x, y;

        while (clientStream >> entityId >> x >> y) {
            entityMap[entityId] = { x, y };
        }

        if (entityMap.size() > 0) {
            clientEntityMap[clientId] = entityMap;
        }
    }
    return;
}

void Client::printEntityMap() {
    // Debugging function to print out the state of clientEntityMap
    for (const auto& client : clientEntityMap) {
        std::cout << "Client " << client.first << " entities:" << std::endl;
        for (const auto& entity : client.second) {
            std::cout << "  Entity " << entity.first << " -> (" << entity.second.first << ", " << entity.second.second << ")" << std::endl;
        }
    }
}