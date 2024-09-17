#pragma once    
#include <zmq.hpp>
#include <string>

class Client {
public:
    Client(int id, const std::string& address);
    void start();  // Main client loop
private:
    zmq::context_t context;
    zmq::socket_t requester;
    int client_id;
};
