#include "Server.hpp"
#include "json.hpp"

using json = nlohmann::json;

Server::Server() : context(1),
                   responder(context, zmq::socket_type::rep),
                   puller(context, zmq::socket_type::pull),
                   publisher(context, zmq::socket_type::pub)
{
}

void Server::bindResponder(const std::string &address, int port)
{
    responder.bind(address + ":" + std::to_string(port));
}

void Server::bindPuller(const std::string &address, int port)
{
    puller.bind(address + ":" + std::to_string(port));
}

void Server::bindPublisher(const std::string &address, int port)
{
    publisher.bind(address + ":" + std::to_string(port));
}

std::string Server::generateUniqueClientID()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100, 199);

    std::string clientID;
    do
    {
        clientID = std::to_string(dis(gen)); // Generate random number
    } while (connectedClientIDs.find(clientID) != connectedClientIDs.end());

    connectedClientIDs.insert(clientID); // Mark the client ID as used
    return clientID;
}

void Server::start()
{
    std::cout << "Server started, waiting for clients..." << std::endl;

    bool broadCastMsgThreadStarted = false;

    while (true)
    {
        if (connectedClientIDs.size() > 0 && !broadCastMsgThreadStarted)
        {
            broadCastMsgThreadStarted = true;
            std::thread broadCastMssgThread(&Server::broadcastMsg, this);
            broadCastMssgThread.detach();
        }
        // Req/Rep setup for Hello message and clientId
        zmq::message_t request;
        (void)responder.recv(request, zmq::recv_flags::dontwait);
        std::string received(static_cast<char *>(request.data()), request.size());

        if (received == "Hello")
        {
            std::string clientID = generateUniqueClientID();
            zmq::message_t reply(clientID.size());
            memcpy(reply.data(), clientID.c_str(), clientID.size());
            responder.send(reply, zmq::send_flags::dontwait);

            std::thread clientThread(&Server::handle_client_thread, this, clientID);
            clientThread.detach();
        }
        else if (received == "Hello_P2P")
        {
            std::string clientID = generateUniqueClientID();
            zmq::message_t reply(clientID.size());
            memcpy(reply.data(), clientID.c_str(), clientID.size());
            responder.send(reply, zmq::send_flags::dontwait);
        }
    }
}

void Server::handle_client_thread(const std::string &clientID)
{
    while (true)
    {
        zmq::message_t positionData;
        {
            std::unique_lock<std::mutex> lock(clientMutex);
            (void)puller.recv(positionData, zmq::recv_flags::dontwait);
        }

        std::string receivedData(static_cast<char *>(positionData.data()), positionData.size());
        // std::cout << "Received JSON message from client " << clientID << ": " << receivedData << std::endl;
        if (receivedData.empty())
        {
            continue; // Skip empty messages
        }

        try
        {
            json j = json::parse(receivedData);

            std::string tempClientID = j["clientID"];
            if (tempClientID != clientID) continue;

            std::unordered_map<int, std::pair<float, float>> entityPositionMap;

            for (const auto &entityJson : j["entities"])
            {
                int entityID = entityJson["entityID"];
                float x = entityJson["x"];
                float y = entityJson["y"];
                entityPositionMap[entityID] = {x, y};
            }

            clientEntityMap[clientID] = entityPositionMap;
            
        }
        catch (const json::exception &e)
        {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
        }
    }
}

void Server::parseString(const std::string &input, const std::string &clientID, std::unordered_map<int, std::pair<float, float>> &entityPositionMap)
{
    std::istringstream stream(input);

    std::string tempClientID;
    stream >> tempClientID;

    int entityID;
    float x, y;

    while (stream >> entityID >> x >> y)
    {
        entityPositionMap[entityID] = std::make_pair(x, y);
    }
    // std::cout << entityPositionMap.size() << std::endl;
}

void Server::printEntityMap()
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

void Server::updateClientEntityMap(EntityManager &serverEntityManager)
{
    std::unordered_map<int, std::pair<float, float>> entityPositionMap;
    for (const auto &entity : serverEntityManager.getEntities())
    {
        entityPositionMap[entity->getID()] = std::make_pair(entity->position.x, entity->position.y);
    }
    clientEntityMap["1"] = entityPositionMap;
}

void Server::broadcastMsg()
{
    int iterations = 0;
    std::vector<long long> broadcastTimes;

    while (true)
    {
        iterations++;
        auto start = std::chrono::high_resolution_clock::now();
        
        std::string pubMsg = generatePubMsg();
        zmq::message_t broadcastMsg(pubMsg.size());
        memcpy(broadcastMsg.data(), pubMsg.data(), pubMsg.size());
        publisher.send(broadcastMsg, zmq::send_flags::dontwait);

        // std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if(iterations <= 100000) {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

            broadcastTimes.push_back(duration);
            if(iterations == 100000) {
                long long totalTime = std::accumulate(broadcastTimes.begin(), broadcastTimes.end(), 0LL);
                double avgTime = static_cast<double>(totalTime) / broadcastTimes.size();

                // Calculate variance
                double variance = 0.0;
                for (const auto &time : broadcastTimes)
                {
                    variance += (time - avgTime) * (time - avgTime);
                }
                variance /= broadcastTimes.size();

                // Output results to file
                std::ofstream outfile("broadcast_benchmark_results.csv", std::ios_base::app); // Append to CSV file
                if (outfile.is_open())
                {
                    // If it's the first time you're writing to the file, include headers
                    if (outfile.tellp() == 0)
                    {
                        outfile << "Total Broadcast Time (microseconds),Average Broadcast Time (microseconds),Variance (microseconds^2)" << std::endl;
                    }

                    outfile << totalTime << "," << avgTime << "," << variance << std::endl; // Write the data in CSV format
                    outfile.close();
                }
                std::cout << "Benchmark data written to broadcast_benchmark_results.csv" << std::endl;
            }
        }
    }
}
std::string Server::generatePubMsg()
{
    json j = json::array();

    for (const auto &clientPair : clientEntityMap)
    {
        const std::string &clientId = clientPair.first;
        const auto &entityMap = clientPair.second;

        json clientData;
        clientData["clientID"] = clientId;
        clientData["entities"] = json::array();

        for (const auto &entityPair : entityMap)
        {
            int entityId = entityPair.first;
            float x = entityPair.second.first;
            float y = entityPair.second.second;

            json entityJson;
            entityJson["entityID"] = entityId;
            entityJson["x"] = x;
            entityJson["y"] = y;

            clientData["entities"].push_back(entityJson);
        }

        j.push_back(clientData);
    }

    return j.dump();
}