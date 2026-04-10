	#include "BackgroundManager.hpp"
	#include "raymath.h"

	BackgroundManager::BackgroundManager()
	{
		yposSprite = -174;
		xposSprite = 0;
		destHeight = 1280;
		destWidth = 896;
		background = LoadTexture("Graphics/background.png");
		source = {0,0 ,(float)background.width, (float)background.height - 50};
		destination = { 0,yposSprite ,(float)background.width * (float)2.3, 1280.0f };
		origin = { 0 ,0 };
	}

	BackgroundManager::~BackgroundManager()
	{
	}

	void BackgroundManager::FollowPlayer(Vector2 playerpos)
	{
		xposSprite = playerpos.x;
		source.x = xposSprite;
	}

	void BackgroundManager::Draw()
	{
		DrawTexturePro(background, source, destination, origin, 0, WHITE);
	}
