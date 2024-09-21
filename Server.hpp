#pragma once 
#include <zmq.hpp>
#include <string>
#include <unordered_map>
#include <map>
#include <utility>
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>

class Server {
public:
    Server(const std::string& address);
    void start();  // Main server loop
private:
    zmq::context_t context;
    zmq::socket_t responder;
    int iteration;
    std::unordered_map<int, int> client_iterations; // Tracks iterations for each client
    void handle_client(const std::string& received_data);  // Handles a client's request
    void parseString(const std::string& input, int64_t& clientID, std::map<int, std::pair<float, float>>& entityPositionMap);
};
