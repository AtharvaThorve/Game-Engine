#include "entity.hpp"
#include "event.hpp"
#include "event_manager.hpp"
#include "main.hpp"
#include "timeline.hpp"
#include <cmath>
#include <memory>
#include <string>

extern Timeline globalTimeline;

namespace collision_utils {
std::string checkCollisionDirection(std::shared_ptr<Entity> entityA,
                                    std::shared_ptr<Entity> entityB);
void handlePlatformCollision(std::shared_ptr<Entity> entityA,
                             std::shared_ptr<Entity> entityB);
void handleDeathZoneCollision(std::shared_ptr<Entity> entityA,
                              std::shared_ptr<Entity> entityB);
void handleBallPaddle(std::shared_ptr<Entity> ball,
                      std::shared_ptr<Entity> paddle);
void handleBallBrick(std::shared_ptr<Entity> ball,
                     std::shared_ptr<Entity> brick);
void handleBallWall(std::shared_ptr<Entity> ball, int sW, int sH);
} // namespace collision_utils