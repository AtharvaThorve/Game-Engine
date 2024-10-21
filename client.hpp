#pragma once    
#include <zmq.hpp>
#include <string>
#include "main.hpp"
#include "entity_manager.hpp"

class Client {
public:
    Client(EntityManager& entityManager, EntityManager& clientEntityManager);
    void start(bool isP2P = false);  // Main client loop
    void connectRequester(const std::string& address, int port);
    void connectPusher(const std::string& address, int port);
    void connectSubscriber(const std::string& address, int port);
    void bindPeerPublisher(const std::string& address, int port);
    void connectPeerSubscriber1(const std::string& address, int port);
    void connectPeerSubscriber2(const std::string& address, int port);
    void connectServer(bool isP2P = false);
    void deserializeClientEntityMap(const std::string& pubMsg);
    void updateOtherEntities();
    void printEntityMap();
private:
    zmq::context_t context;
    zmq::socket_t requester;
    zmq::socket_t pusher;
    zmq::socket_t subscriber;
    zmq::socket_t peerPublisher;
    zmq::socket_t peerSubscriber1;
    zmq::socket_t peerSubscriber2;
    std::string clientID;
    EntityManager& entityManager;
    EntityManager& clientEntityManager;
    std::unordered_map<std::string, std::unordered_map<int, std::pair<float, float>>> clientEntityMap;
    void receiveSubMsg();
    void receivePeerMsg();
};