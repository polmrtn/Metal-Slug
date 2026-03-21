
#include <raylib.h>

#include "Player.hpp"

int main() 
{
	InitWindow(1280, 896, "Metal Slug");
	SetTargetFPS(30);

	Player player;
	while (WindowShouldClose() == false)
	{
		BeginDrawing();
		ClearBackground(BLACK);
		player.Draw();
		EndDrawing();
	
	}
}
