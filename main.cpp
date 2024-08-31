#include "main.hpp"
#include <memory>

int main(int argc, char* argv[]) 
{
	initSDL();

	SDL_Color screenColor;
	screenColor.r = 0x00;
	screenColor.g = 0x00;
	screenColor.b = 0xFF;
	screenColor.a = 0x00;

	//SDL_Color rectColor;
	//rectColor.r = 0xFF;
	//rectColor.g = 0x00;
	//rectColor.b = 0x00;
	//rectColor.a = 0x00;

	Rectangle obj1(app->renderer);
	Rectangle obj2(app->renderer);
	

	while (1)
	{
		prepareScene(screenColor);

		doInput();

		//drawRect((SCREEN_WIDTH / 2) - 100, (SCREEN_HEIGHT / 2) - 200, 200, 400);
		//colorRect(rectColor);
		//presentScene();

		obj1.Draw(0, 0, 100, 100);
		obj2.Draw(10, 10, 100, 100);

		/*obj1.IsColliding(obj2);*/

		obj1.Render(app->renderer);
		obj2.Render(app->renderer);

		SDL_RenderPresent(app->renderer);
	}
	
	return 0;
}