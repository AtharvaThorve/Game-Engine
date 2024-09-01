#include "cleanup.hpp"

void clean_up_sdl() {
    if (app->renderer) {
        SDL_DestroyRenderer(app->renderer);
        app->renderer = nullptr;
    }
    if (app->window) {
        SDL_DestroyWindow(app->window);
        app->window = nullptr;
    }
    SDL_Quit();
}

void clean_up_physics_system() {
    delete physicsSystem;
    physicsSystem = nullptr;
}

//void clean_up_entities(EntityManager* entityManager) {
//    entityManager->cleanUp();
//}
