#pragma once 
#include <zmq.hpp>
#include <string>
#include <unordered_map>
#include <utility>
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <set>
#include <mutex>
#include <numeric>
#include <fstream>
#include "EntityManager.hpp"

class Server {
public:
    Server();
    void start();  // Main server loop
    void bindResponder(const std::string& address, int port);
    void bindPuller(const std::string& address, int port);
    void bindPublisher(const std::string& address, int port);
    void updateClientEntityMap(EntityManager& serverEntityManager);
    void broadcastMsg();

private:
    std::mutex clientMutex;
    zmq::context_t context;
    zmq::socket_t responder;
    zmq::socket_t puller;
    zmq::socket_t publisher;
    std::set<std::string> connectedClientIDs;
    std::unordered_map<std::string, std::unordered_map<int, std::pair<float, float>>> clientEntityMap;
    std::string generateUniqueClientID(void);
    std::string generatePubMsg();
    void handle_client_thread(const std::string& clientID);
    void parseString(const std::string& input, const std::string& clientID, std::unordered_map<int, std::pair<float, float>>& entityPositionMap);
    void printEntityMap();
};
