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
    void connectSubscriber(const std::string& address, int port);
    void deserializeClientEntityMap(const std::string& pubMsg);
    void printEntityMap();
private:
    zmq::context_t context;
    zmq::socket_t requester;
    zmq::socket_t pusher;
    zmq::socket_t subscriber;
    std::string clientID;
    EntityManager& entityManager;
    std::unordered_map<std::string, std::unordered_map<int, std::pair<float, float>>> clientEntityMap;
};
