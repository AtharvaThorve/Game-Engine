#include "entity.hpp"
#include <memory>
#include <string>

namespace collision_utils {
    bool isColliding(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB);
    std::string checkCollisionDirection(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB);
    void handlePlatformCollision(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB);
}