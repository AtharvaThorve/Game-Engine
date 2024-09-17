#pragma once 
#include <zmq.hpp>
#include <string>
#include <unordered_map>

class Server {
public:
    Server(const std::string& address);
    void start();  // Main server loop
private:
    zmq::context_t context;
    zmq::socket_t responder;
    int iteration;
    std::unordered_map<int, int> client_iterations; // Tracks iterations for each client
    void handle_client(int client_id);  // Handles a client's request
};
