#include "main.hpp"
#include <memory>
#include <iostream>

int obj2_X = 100;
int obj2_Y = 100;

int main(int argc, char* argv[]) 
{
    initSDL();

	initPhysics(0.0f, 9.8f);

	EntityManager entityManager;

	auto rectEntity1 = createRectEntity(Vector2{ 100, 100 }, Vector2{ 0, 0 }, SDL_Color{ 255, 0, 0, 0 }, 1.0f, 50, 100, true, true, true);
	auto rectEntity2 = createRectEntity(Vector2{ 200, 200 }, Vector2{ 0, 0 }, SDL_Color{ 0, 255, 0, 0 }, 1.0f, 100, 50, true, true, true);

	entityManager.addEntity(rectEntity1);
	entityManager.addEntity(rectEntity2);

	Uint32 lastTime = SDL_GetTicks();
	//SDL_Color rectColor;
	//rectColor.r = 0xFF;
	//rectColor.g = 0x00;
	//rectColor.b = 0x00;
	//rectColor.a = 0x00;

	Rectangle obj1(app->renderer);
	Rectangle obj2(app->renderer);

	while (1)
	{
		doInput();

		// Uint32 currentTime = SDL_GetTicks();
		// float deltaTime = (currentTime - lastTime) / 1000.0f;
		// lastTime = currentTime;

		// entityManager.updateEntities(deltaTime);

		// prepareScene(SDL_Color{0, 0, 255, 0});
		// entityManager.renderEntities();
		// presentScene();

		obj1.Draw(0, 0, 100, 100);
		obj2.Draw(obj2_X, obj2_Y, 100, 100);

		obj1.IsColliding(obj2);

		obj1.Render(app->renderer);
		obj2.Render(app->renderer);

		SDL_RenderPresent(app->renderer);
	}

	clean_up_entities(&entityManager);
	clean_up_physics_system();
	clean_up_sdl();
	
	return 0;
}