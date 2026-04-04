#include "BackgroundManager.hpp"
#include "raymath.h"

BackgroundManager::BackgroundManager()
{
	yposSprite = 150;
	xposSprite = 0;
	destHeight = 1280;
	destWidth = 896;
	background = LoadTexture("Graphics/background.png");
	source = {xposSprite,yposSprite ,570, 405};
	destination = { 0,0 ,destHeight, destWidth };
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
