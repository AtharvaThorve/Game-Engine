#include "entity.hpp"
#include <memory>
#include <cmath>
#include <string>

namespace collision_utils {
    std::string checkCollisionDirection(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB);
    void handlePlatformCollision(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB);
}