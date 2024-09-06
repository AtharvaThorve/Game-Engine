#include "scaling.hpp"

bool allowScaling = false;

void setRenderScale(float scaleX, float scaleY) {
    SDL_RenderSetScale(app->renderer, scaleX, scaleY);
}

void updateScaleFactor(float& scale) {
    int currentScreenWidth, currentScreenHeight;
    SDL_GetWindowSize(app->window, &currentScreenWidth, &currentScreenHeight);
    float widthScale = static_cast<float>(currentScreenWidth) / SCREEN_WIDTH;
    float heightScale = static_cast<float>(currentScreenHeight) / SCREEN_HEIGHT;

    scale = std::min(widthScale, heightScale);
}