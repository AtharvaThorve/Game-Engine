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

class Server {
public:
    Server();
    void start();  // Main server loop
    void bindResponder(const std::string& address, int port);
    void bindPuller(const std::string& address, int port);
    void bindPublisher(const std::string& address, int port);

private:
    std::mutex clientMutex;
    zmq::context_t context;
    zmq::socket_t responder;
    zmq::socket_t puller;
    zmq::socket_t publisher;
    std::set<std::string> connectedClientIDs;
    std::unordered_map<std::string, std::unordered_map<int, std::pair<float, float>>> clientEntityMap;
    std::string generateUniqueClientID(void);
    void handle_client(const std::string& received_data);  // Handles a client's request
    void handle_client_thread(const std::string& clientID);
    void parseString(const std::string& input, const std::string& clientID, std::unordered_map<int, std::pair<float, float>>& entityPositionMap);
    void printEntityMap();
};
