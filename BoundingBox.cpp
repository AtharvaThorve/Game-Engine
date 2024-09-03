#include "BoundingBox.hpp"

BoundingBox::BoundingBox(int x, int y, int w, int h) {

	rect = { x, y, w, h };
}

bool BoundingBox::isInside(const std::shared_ptr<Entity>& entity) {
    if (entity->shape->type == ShapeType::RECTANGLE) {
        auto rectShape = dynamic_cast<RectangleShape*>(entity->shape.get());
        SDL_Rect& entityRect = rectShape->rect;

        return (entityRect.x >= rect.x &&
            entityRect.y >= rect.y &&
            entityRect.x + entityRect.w <= rect.x + rect.w &&
            entityRect.y + entityRect.h <= rect.y + rect.h);
    }

}
SDL_Rect BoundingBox::getRect() const {
    return rect;
}


void BoundingBox::enforceBoundaries(const std::shared_ptr<Entity>& entity) {
    if (entity->shape->type == ShapeType::RECTANGLE) {
        auto rectShape = dynamic_cast<RectangleShape*>(entity->shape.get());
        SDL_Rect& entityRect = rectShape->rect;

        if (entityRect.x < rect.x) {
            entityRect.x = rect.x; 
            entity->velocity.x = 0; 
        }

        if (entityRect.x + entityRect.w > rect.x + rect.w) {
            entityRect.x = rect.x + rect.w - entityRect.w; 
            entity->velocity.x = 0; 
        }

        if (entityRect.y < rect.y) {
            entityRect.y = rect.y; 
            entity->velocity.y = 0;
        }
        if (entityRect.y + entityRect.h > rect.y + rect.h) {
            entityRect.y = rect.y + rect.h - entityRect.h; 
            entity->velocity.y = 0; 
        }

        if (entityRect.x + entityRect.w < rect.x) {
            entityRect.x = rect.x - entityRect.w; // Keep it outside on the left
            entity->velocity.x = 0;
        }
        if (entityRect.x > rect.x + rect.w) {
            entityRect.x = rect.x + rect.w; // Keep it outside on the right
            entity->velocity.x = 0;
        }
        if (entityRect.y + entityRect.h < rect.y) {
            entityRect.y = rect.y - entityRect.h; // Keep it outside on the top
            entity->velocity.y = 0;
        }
        if (entityRect.y > rect.y + rect.h) {
            entityRect.y = rect.y + rect.h; // Keep it outside on the bottom
            entity->velocity.y = 0;
        }

    }
}
