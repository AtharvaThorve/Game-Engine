#include "Server.hpp"
#include <iostream>
#include <thread>
#include <chrono>

Server::Server(const std::string& address)
    : context(1), responder(context, ZMQ_REP), iteration(0) {
    responder.bind(address);  // Bind to the specified address
}

void Server::start() {
    std::cout << "Server started, waiting for clients..." << std::endl;

    while (true) {
        zmq::message_t request;

        // Receive client message
        responder.recv(request, zmq::recv_flags::none);
        std::string received(static_cast<char*>(request.data()), request.size());

        // Extract client ID from message
        int client_id = std::stoi(received);

        // Handle client's request
        handle_client(client_id);

        // Simulate processing delay
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void Server::handle_client(int client_id) {
    iteration++;

    // If it's the first message from the client, initialize its iteration count
    if (client_iterations.find(client_id) == client_iterations.end()) {
        client_iterations[client_id] = 1;
    }
    else {
        client_iterations[client_id]++;
    }

    // Prepare response message for the client
    std::string reply = "Client " + std::to_string(client_id) + ": Iteration " + std::to_string(client_iterations[client_id]);

    zmq::message_t reply_message(reply.size());
    memcpy(reply_message.data(), reply.c_str(), reply.size());

    // Send the message back to the client
    responder.send(reply_message, zmq::send_flags::none);

    std::cout << "Sent to Client " << client_id << ": " << reply << std::endl;
}
