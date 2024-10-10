#include "collision_utils.hpp"

namespace collision_utils
{
    std::string checkCollisionDirection(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB)
    {
        // Use the isColliding function to check for a collision first
        if (entityA->isColliding(*entityB))
        {
            RectangleShape *rectA = dynamic_cast<RectangleShape *>(entityA->shape.get());
            RectangleShape *rectB = dynamic_cast<RectangleShape *>(entityB->shape.get());

            if (rectA && rectB)
            {
                // Determine the direction of the collision
                int deltaX = (rectA->rect.x + rectA->rect.w / 2) - (rectB->rect.x + rectB->rect.w / 2);
                int deltaY = (rectA->rect.y + rectA->rect.h / 2) - (rectB->rect.y + rectB->rect.h / 2);

                // Collision direction determination
                if (std::abs(deltaX) > std::abs(deltaY))
                {
                    // Collision is more horizontal
                    return (deltaX > 0) ? "left" : "right";
                }
                else
                {
                    // Collision is more vertical
                    return (deltaY > 0) ? "up" : "down";
                }
            }
        }

        // Default case: no collision or unsupported shapes
        return "none";
    }

    void handlePlatformCollision(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB)
    {
        std::string direction = checkCollisionDirection(entityA, entityB);
        RectangleShape *rectA = dynamic_cast<RectangleShape *>(entityA->shape.get());
        RectangleShape *rectB = dynamic_cast<RectangleShape *>(entityB->shape.get());

        if (direction == "up")
        {
            entityA->position.y = entityB->position.y + rectB->rect.h - 1;
            if(entityA->velocity.y < 0)
                entityA->velocity.y = 0;
        }
        else if (direction == "down")
        {
            entityA->position.y = entityB->position.y - rectA->rect.h + 1;
            if(!entityA->standingPlatform)
                entityA->velocity.x = 0;
            
            entityA->standingPlatform = entityB;
            if(entityA->velocity.y > 0)
                entityA->velocity.y = 0;
        }
        else if (direction == "left")
        {
            entityA->position.x = entityB->position.x + rectB->rect.w - 1;
            if(entityA->velocity.x < 0)
                entityA->velocity.x = 0;
        }
        else if (direction == "right")
        {
            entityA->position.x = entityB->position.x - rectA->rect.w + 1;
            if(entityA->velocity.x > 0)
                entityA->velocity.x = 0;
        }
    }

} // namespace collision_utils
