#pragma once
#include <SDL2/SDL.h>
#include "entity.hpp"

class BoundingBox {
public:
    BoundingBox(int x, int y, int width, int height);

    bool isInside(const std::shared_ptr<Entity>& entity);
    SDL_Rect getRect() const;
    void enforceBoundaries(const std::shared_ptr<Entity>& entity);

private:
    SDL_Rect rect; // The SDL rectangle representing the bounding box
};
