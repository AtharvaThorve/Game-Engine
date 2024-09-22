#include "Client.hpp"
#include <iostream>
#include <thread>

Client::Client(EntityManager& entityManager)
    : context(1), requester(context, ZMQ_REQ), pusher(context, ZMQ_PUSH), entityManager(entityManager) {
}

void Client::connectRequester(const std::string& address, int port) {
    requester.connect(address + ":" + std::to_string(port));
}

void Client::connectPusher(const std::string& address, int port) {
    pusher.connect(address + ":" + std::to_string(port));
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
     
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}


