#include "main.hpp"
#include <memory>
#include <iostream>

int obj2_X = 100;
int obj2_Y = 100;

int main(int argc, char* argv[]) 
{
    initSDL();

	//initPhysics(0.0f, 9.8f);

	// Define entity's initial properties
	Vector2 initialPosition{ 100, 100 };  // Starting position of the entity
	Vector2 initialVelocity{ 0, 0 };      // Starting velocity of the entity
	float mass = 1.0f;                  // Mass of the entity
	bool isAffectedByGravity = false;   // Assuming gravity is not applied
	bool isMovable = true;              // Entity can move
	bool isHittable = true;             // Entity can be hit
	ShapeType shapeType = ShapeType::RECTANGLE;  // Choose RECTANGLE or CIRCLE
	SDL_Color color = { 255, 0, 0, 255 }; // Red color
	SDL_Rect rect = { static_cast<int>(initialPosition.x), static_cast<int>(initialPosition.y), 50, 50 }; // Size of rectangle
	SDL_Point center = { 0, 0 };
	int radius = 0;

	Entity entity(initialPosition, initialVelocity, mass, isAffectedByGravity, isMovable, isHittable, shapeType, color, rect, center, radius);

	Uint32 lastTime = SDL_GetTicks();
	

	while (1)
	{
		doInput(&entity, 50.0f);

		Uint32 currentTime = SDL_GetTicks();
		float deltaTime = (currentTime - lastTime) / 1000.0f;
		lastTime = currentTime;

		entity.updatePosition(deltaTime);

		// entityManager.updateEntities(deltaTime);

		prepareScene(SDL_Color{0, 0, 255, 255});

		entity.draw();
		presentScene();

		SDL_RenderPresent(app->renderer);
	}

	//clean_up_physics_system();
	clean_up_sdl();
	
	return 0;
}