#include "BackgroundManager.hpp"

BackgroundManager::BackgroundManager()
{
	background = LoadTexture("Graphics/backgrond.png");

}

BackgroundManager::~BackgroundManager()
{

}

void BackgroundManager::FollowPlayer()
{

}

void BackgroundManager::Draw(Rectangle screen, Vector2 origin, Rectangle source)
{
	DrawTexturePro(background, source, screen, { 0,0 }, 0, WHITE);
}
