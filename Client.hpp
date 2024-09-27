#pragma once    
#include <zmq.hpp>
#include <string>
#include "main.hpp"
#include "EntityManager.hpp"

class Client {
public:
    Client(EntityManager& entityManager, EntityManager& clientEntityManager);
    void start();  // Main client loop
    void connectRequester(const std::string& address, int port);
    void connectPusher(const std::string& address, int port);
    void connectSubscriber(const std::string& address, int port);
    void connectServer();
    void deserializeClientEntityMap(const std::string& pubMsg);
    void updateOtherEntities();
    void printEntityMap();
private:
    zmq::context_t context;
    zmq::socket_t requester;
    zmq::socket_t pusher;
    zmq::socket_t subscriber;
    std::string clientID;
    EntityManager& entityManager;
    EntityManager& clientEntityManager;
    std::unordered_map<std::string, std::unordered_map<int, std::pair<float, float>>> clientEntityMap;
    void receiveSubMsg();
};