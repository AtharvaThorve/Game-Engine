#include "Client.hpp"
#include <iostream>
#include <thread>

Client::Client(int id, const std::string& address)
    : context(1), requester(context, ZMQ_REQ), client_id(id) {
    requester.connect(address);  // Connect to the specified address
}

void Client::start() {
    std::cout << "Client " << client_id << " started." << std::endl;

    while (true) {
        // Send client ID to server
        std::string message = std::to_string(client_id);
        zmq::message_t request(message.size());
        memcpy(request.data(), message.c_str(), message.size());
        requester.send(request, zmq::send_flags::none);

        // Receive server's response
        zmq::message_t reply;
        requester.recv(reply, zmq::recv_flags::none);
        std::string received_message(static_cast<char*>(reply.data()), reply.size());

        // Print the received message
        std::cout << "Received: " << received_message << std::endl;

        // Simulate some delay before sending the next request
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
