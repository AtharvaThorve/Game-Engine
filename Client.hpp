#pragma once    
#include <zmq.hpp>
#include <string>
#include "EntityManager.hpp"

class Client {
public:
    Client(EntityManager& entityManager);
    void start();  // Main client loop
    void connectRequester(const std::string& address, int port);
    void connectPusher(const std::string& address, int port);
    void connectServer();
private:
    zmq::context_t context;
    zmq::socket_t requester;
    zmq::socket_t pusher;
    std::string clientID;
    EntityManager& entityManager;
};
