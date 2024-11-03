#include "disconnectHandler.hpp"

DisconnectHandler::DisconnectHandler(EventManager *em, Timeline *timeline)
    : em(em), timeline(timeline) {}

void DisconnectHandler::on_event(const Event &event) {
  if (event.type == disconnect_event_hash) {
    std::shared_ptr<std::unordered_map<
        std::string, std::unordered_map<int, std::pair<float, float>>>>
        clientEntityMap = std::get<std::shared_ptr<std::unordered_map<
            std::string, std::unordered_map<int, std::pair<float, float>>>>>(
            event.parameters.at("clientEntityMap"));
    std::string clientID =
        std::get<std::string>(event.parameters.at("clientID"));

    std::shared_ptr<std::set<std::string>> connectedClientIDs =
        std::get<std::shared_ptr<std::set<std::string>>>(
            event.parameters.at("connectedClientIDs"));

    if (connectedClientIDs->find(clientID) != connectedClientIDs->end()) {
      connectedClientIDs->erase(clientID);
    }

    if (clientEntityMap->find(clientID) != clientEntityMap->end()) {
      clientEntityMap->erase(clientID);
      std::cout << "Removed client " << clientID << " from clientEntityMap"
                << std::endl; // Debug info
    }
  }
}
