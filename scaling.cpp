#include "scaling.hpp"

bool allowScaling = true;

void setRenderScale(float scaleX, float scaleY) {
    SDL_RenderSetScale(app->renderer, scaleX, scaleY);
}

void updateScaleFactor(float& scale) {
    int currentScreenWidth, currentScreenHeight;
    SDL_GetWindowSize(app->window, &currentScreenWidth, &currentScreenHeight);
    // Calculate the scale factor based on the smaller dimension
    float widthScale = static_cast<float>(currentScreenWidth) / SCREEN_WIDTH;
    float heightScale = static_cast<float>(currentScreenHeight) / SCREEN_HEIGHT;
    // Use the smaller scale to maintain the aspect ratio
    scale = std::min(widthScale, heightScale);
}