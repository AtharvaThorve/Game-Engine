#include "bullet_handler.hpp"

BulletHandler::BulletHandler(Timeline *timeline) : timeline(timeline) {}

void BulletHandler::on_event(const Event &event) {
  if (event.type == bullet_event_hash) {
    std::shared_ptr<Entity> shooter =
        std::get<std::shared_ptr<Entity>>(event.parameters.at("shooter"));
    
    std::shared_ptr<std::unordered_set<std::shared_ptr<Entity>>> bullets =
        std::get<std::shared_ptr<std::unordered_set<std::shared_ptr<Entity>>>>(
            event.parameters.at("bullets"));

    std::vector<std::shared_ptr<EntityManager>> entityManagers =
        std::get<std::vector<std::shared_ptr<EntityManager>>>(
            event.parameters.at("entityManagers"));

    Vector2 shooterPos = shooter->getPosition();
    auto bullet = std::make_shared<Entity>(
        Vector2{shooterPos.x + shooter->dimensions.x / 2 - 5,
                shooterPos.y + shooter->dimensions.y},
        Vector2{10, 20}, SDL_Color{255, 0, 0, 255}, &globalTimeline, 1);
    bullet->isMovable = true;
    if (shooter->dimensions.x == 50) {
      bullet->velocity = {0, -100};
      bullet->color = {0, 0, 0, 255};
      Vector2 bulletPos = bullet->getPosition();
      bullet->setPosition({bulletPos.x, shooterPos.y - 20});
    } else {
      bullet->velocity = {0, 100};
    }
    bullet->maxVelocity = {0, 100};
    bullets->insert(bullet);
    entityManagers[0]->addEntity(bullet);
  }
}
