#pragma once
#include "entity_manager.hpp"
#include "event.hpp"
#include "event_manager.hpp"
#include "timeline.hpp"
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <unordered_set>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <zmq.hpp>

class Server {
public:
  Server(EventManager *em, Timeline *timeline);
  void start(); // Main server loop
  void bindResponder(const std::string &address, int port);
  void bindPuller(const std::string &address, int port);
  void bindPublisher(const std::string &address, int port);
  void updateClientEntityMap(EntityManager &serverEntityManager);
  void broadcastMsg();

private:
  std::mutex clientMutex;
  zmq::context_t context;
  zmq::socket_t responder;
  zmq::socket_t puller;
  zmq::socket_t publisher;
  EventManager *em;
  Timeline *timeline;
  std::shared_ptr<std::unordered_set<std::string>> connectedClientIDs;
  std::shared_ptr<std::unordered_map<std::string, std::unordered_map<int, std::pair<float, float>>>> clientEntityMap;
  std::string generateUniqueClientID(void);
  std::string generatePubMsg();
  void handle_client_thread(const std::string &clientID);
  bool parseString(
      const std::string &input, const std::string &clientID,
      std::unordered_map<int, std::pair<float, float>> &entityPositionMap);
  void printEntityMap();
  void raiseClientDisconnectEvent(std::string clientID);
};
