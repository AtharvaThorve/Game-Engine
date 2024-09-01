#include "entity.hpp"
#include <iostream>

Entity* createRectEntity(Vector2 position, Vector2 velocity, SDL_Color color, float mass, int height, int width, bool isMovable, bool isAffectedByGravity, bool isHittable)
{
    Entity* entity = new Entity();
    entity->color = color;
    entity->position = position;
    entity->velocity = velocity;
    entity->mass = mass;
    entity->isMovable = isMovable;
    entity->isAffectedByGravity = isAffectedByGravity;
    entity->isHittable = isHittable;
    entity->rect = new Rectangle(app->renderer);
    entity->rect->Draw(static_cast<int>(position.x), static_cast<int>(position.y), width, height);
    return entity;
}

void EntityManager::addEntity(Entity* entity)
{
    entities.push_back(entity);
}

void EntityManager::updateEntities(float deltaTime)
{
    for (auto& entity : entities) {
        if (entity->isMovable) {
            apply_gravity(entity, deltaTime);
            update_entity(entity, deltaTime);
        }
    }
}

void EntityManager::renderEntities()
{
    for (auto& entity : entities) {
        SDL_SetRenderDrawColor(app->renderer, entity->color.r, entity->color.g, entity->color.b, entity->color.a);
        // SDL_RenderFillRect(app->renderer, &entity->rect);
    }
}

void EntityManager::cleanUp() {
    for (auto& entity : entities) {
        delete entity;
    }
    entities.clear();
}