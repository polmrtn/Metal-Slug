#include "SoldierAnim.hpp"
enum class Animations{IDLE, WALKING, JUMPING};
SoldierAnim::SoldierAnim()
{
	Image img = LoadImage("Graphics/NEW/Sprite-0002.png");
	spriteSheet = LoadTextureFromImage(img);
	UnloadImage(img);
	SetTextureFilter(spriteSheet, TEXTURE_FILTER_POINT);
}

SoldierAnim::~SoldierAnim()
{
}

void SoldierAnim::LoadTexture()
{
	
}

void SoldierAnim::UnloadTextures()
{
}
