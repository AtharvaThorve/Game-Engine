#include "entity.hpp"
#include "event.hpp"
#include "event_manager.hpp"
#include "timeline.hpp"
#include "main.hpp"
#include <memory>
#include <cmath>
#include <string>

extern Timeline globalTimeline;

namespace collision_utils {
    std::string checkCollisionDirection(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB);
    void handlePlatformCollision(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB);
    void handleDeathZoneCollision(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB);
}