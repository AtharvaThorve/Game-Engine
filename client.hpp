#pragma once
#include "entity_manager.hpp"
#include "main.hpp"
#include <atomic>
#include <string>
#include <zmq.hpp>

class Client {
public:
  Client(EntityManager &entityManager, EntityManager &clientEntityManager);
  static std::atomic<bool> disconnectRequested;
  void start(bool isP2P = false); // Main client loop
  void connectRequester(const std::string &address, int port);
  void connectPusher(const std::string &address, int port);
  void connectSubscriber(const std::string &address, int port);
  void bindPeerPublisher(const std::string &address, int port);
  void connectPeerSubscriber1(const std::string &address, int port);
  void connectPeerSubscriber2(const std::string &address, int port);
  bool connectServer(bool isP2P = false);
  void deserializeClientEntityMap(const std::string &pubMsg);
  void updateOtherEntities();
  void printEntityMap();
  void cleanup();

private:
  zmq::context_t context;
  zmq::socket_t requester;
  zmq::socket_t pusher;
  zmq::socket_t subscriber;
  zmq::socket_t peerPublisher;
  zmq::socket_t peerSubscriber1;
  zmq::socket_t peerSubscriber2;
  std::string clientID;
  EntityManager &entityManager;
  EntityManager &clientEntityManager;
  std::unordered_map<std::string,
                     std::unordered_map<int, std::pair<float, float>>>
      clientEntityMap;
  std::unordered_map<std::string, std::shared_ptr<Entity>> dict;
  std::unordered_set<std::string> activeEntities;
  void receiveSubMsg();
  void receivePeerMsg();
};