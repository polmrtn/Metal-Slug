#include <raylib.h>
#include "game.hpp"

int main() 
{

	InitWindow(1280, 896, "Metal Slug");
	SetTargetFPS(30);

	Game game;

	while (WindowShouldClose() == false)
	{
		game.HandleInput();
		game.Update();

		BeginDrawing();
		ClearBackground(BLACK);
		game.Draw();

		EndDrawing();
	
	}
}
