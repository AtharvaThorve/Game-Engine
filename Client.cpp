#include "Client.hpp"
#include <iostream>
#include <thread>
#include "json.hpp"

using json = nlohmann::json;

std::unordered_map<std::string, std::shared_ptr<Entity>> dict;

Client::Client(EntityManager &entityManager, EntityManager &clientEntityManager) : context(1),
                                                                                   requester(context, zmq::socket_type::req),
                                                                                   pusher(context, zmq::socket_type::push),
                                                                                   subscriber(context, zmq::socket_type::sub),
                                                                                   peerPublisher(context, zmq::socket_type::pub),
                                                                                   peerSubscriber1(context, zmq::socket_type::sub),
                                                                                   peerSubscriber2(context, zmq::socket_type::sub),
                                                                                   entityManager(entityManager),
                                                                                   clientEntityManager(clientEntityManager)
{
}

void Client::connectRequester(const std::string &address, int port)
{
    requester.connect(address + ":" + std::to_string(port));
}

void Client::connectPusher(const std::string &address, int port)
{
    pusher.connect(address + ":" + std::to_string(port));
}

void Client::connectSubscriber(const std::string &address, int port)
{
    subscriber.connect(address + ":" + std::to_string(port));
    subscriber.set(zmq::sockopt::subscribe, "");
}

void Client::bindPeerPublisher(const std::string &address, int port)
{
    peerPublisher.bind(address + ":" + std::to_string(port)); // Bind to an address for peer publishing
}

void Client::connectPeerSubscriber1(const std::string &address, int port)
{
    peerSubscriber1.connect(address + ":" + std::to_string(port)); // Connect to another client's publisher
    peerSubscriber1.set(zmq::sockopt::subscribe, "");
}

void Client::connectPeerSubscriber2(const std::string &address, int port)
{
    peerSubscriber2.connect(address + ":" + std::to_string(port)); // Connect to another client's publisher
    peerSubscriber2.set(zmq::sockopt::subscribe, "");
}

void Client::connectServer(bool isP2P)
{
    std::string helloMessage = isP2P ? "Hello_P2P" : "Hello";
    zmq::message_t request(helloMessage.size());
    memcpy(request.data(), helloMessage.c_str(), helloMessage.size());
    requester.send(request, zmq::send_flags::none);

    zmq::message_t reply;
    (void)requester.recv(reply, zmq::recv_flags::none);
    std::string received(static_cast<char *>(reply.data()), reply.size());
    clientID = received;
    std::cout << "Client connected with ID: " << clientID << std::endl;

    std::thread subMsgThread(&Client::receiveSubMsg, this);
    subMsgThread.detach();

    if (isP2P)
    {
        std::thread peerMsgThread(&Client::receivePeerMsg, this);
        peerMsgThread.detach();
    }
}

void Client::receivePeerMsg()
{
    while (true)
    {
        bool shouldUpdateEntities = false;
        zmq::message_t peerMsg1, peerMsg2;

        // Receive data from both peers, don't block if no message
        (void)peerSubscriber1.recv(peerMsg1, zmq::recv_flags::dontwait);
        (void)peerSubscriber2.recv(peerMsg2, zmq::recv_flags::dontwait);

        std::string recvMsg1(static_cast<char *>(peerMsg1.data()), peerMsg1.size());
        std::string recvMsg2(static_cast<char *>(peerMsg2.data()), peerMsg2.size());

        // Check if messages are not empty
        if (!recvMsg1.empty())
        {
            deserializeClientEntityMap(recvMsg1);
            shouldUpdateEntities = true;
        }

        if (!recvMsg2.empty())
        {
            deserializeClientEntityMap(recvMsg2);
            shouldUpdateEntities = true;
        }

        if (shouldUpdateEntities)
            updateOtherEntities();
    }
}

void Client::receiveSubMsg()
{
    while (true)
    {
        zmq::message_t subMsg;
        (void)subscriber.recv(subMsg, zmq::recv_flags::dontwait);
        std::string recvMsg(static_cast<char *>(subMsg.data()), subMsg.size());
        if (recvMsg.empty())
        {
            continue; // Skip empty messages
        }
        deserializeClientEntityMap(recvMsg);
        // update the entities accoring to the subMsg
        updateOtherEntities();
        // printEntityMap();
    }
}
void Client::start(bool isP2P)
{
    while (true)
    {
        // Create a JSON object
        json j;
        j["clientID"] = clientID;

        // Add entities to the JSON object
        j["entities"] = json::array();
        for (const auto &entity : entityManager.getEntities())
        {
            json entityJson;
            entityJson["entityID"] = entity->getID();
            entityJson["x"] = entity->position.x;
            entityJson["y"] = entity->position.y;
            // Add other entity properties if needed
            j["entities"].push_back(entityJson);
        }

        // Serialize JSON to string
        std::string message = j.dump();
        // std::cout << "Sending JSON message: " << message << std::endl;

        // Send the message
        zmq::message_t zmqMessage(message.size());
        memcpy(zmqMessage.data(), message.c_str(), message.size());

        if (!isP2P)
        {
            pusher.send(zmqMessage, zmq::send_flags::none);
        }
        else
        {
            peerPublisher.send(zmqMessage, zmq::send_flags::none);
        }
    }
}

void Client::deserializeClientEntityMap(const std::string &pubMsg)
{
    try
    {
        json j = json::parse(pubMsg);

        // Clear existing data
        clientEntityMap.clear();

        // Check if the JSON is an array of clients
        if (j.is_array())
        {
            for (const auto &clientData : j)
            {
                std::string clientId = clientData["clientID"];
                if (clientId == clientID) continue; // Skip own data

                std::unordered_map<int, std::pair<float, float>> entityMap;

                for (const auto &entityJson : clientData["entities"])
                {
                    int entityId = entityJson["entityID"];
                    float x = entityJson["x"];
                    float y = entityJson["y"];
                    entityMap[entityId] = {x, y};
                }

                clientEntityMap[clientId] = entityMap;
            }
        }
    }
    catch (const json::exception &e)
    {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }
}

void Client::printEntityMap()
{
    // Debugging function to print out the state of clientEntityMap
    for (const auto &client : clientEntityMap)
    {
        std::cout << "Client " << client.first << " entities:" << std::endl;
        for (const auto &entity : client.second)
        {
            std::cout << "  Entity " << entity.first << " -> (" << entity.second.first << ", " << entity.second.second << ")" << std::endl;
        }
    }
}
void Client::updateOtherEntities()
{
    // Remove the current client's entities from clientEntityMap
    clientEntityMap.erase(clientID);

    // Prepare to track entities that are still present
    std::unordered_set<std::string> currentEntityKeys;

    for (const auto &clientPair : clientEntityMap)
    {
        const std::string &cID = clientPair.first;
        for (const auto &entityPair : clientPair.second)
        {
            int entityID = entityPair.first;
            Vector2 newPosition{entityPair.second.first, entityPair.second.second};
            std::string identifier = cID + "_" + std::to_string(entityID);
            currentEntityKeys.insert(identifier);

            if (dict.find(identifier) == dict.end())
            {
                // Create a new entity if it doesn't exist
                auto newEntity = std::make_shared<Entity>(newPosition, Vector2{50, 50}, SDL_Color{255, 0, 0, 255}, &globalTimeline, 2);
                dict[identifier] = newEntity;
                clientEntityManager.addEntity(newEntity);
            }
            else
            {
                // Update the existing entity's position
                dict[identifier]->position = newPosition;
            }
        }
    }

    // Remove entities that are no longer present
    for (auto it = dict.begin(); it != dict.end(); )
    {
        if (currentEntityKeys.find(it->first) == currentEntityKeys.end())
        {
            clientEntityManager.removeEntity(it->second);
            it = dict.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
