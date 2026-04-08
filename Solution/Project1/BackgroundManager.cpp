	#include "BackgroundManager.hpp"
	#include "raymath.h"

	BackgroundManager::BackgroundManager()
	{
		yposSprite = -342;
		xposSprite = 0;
		destHeight = 1280;
		destWidth = 896;
		background = LoadTexture("Graphics/background.png");
		source = {0,0 ,(float)background.width, (float)background.height *(float)0.8}; //if player isground deactivate camera follow y; fit background; make floor collision
		destination = { 0,yposSprite ,(float)background.width * (float)2.3, 1280 };
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
