#pragma once
#include <raylib.h>

class SoldierAnim
{
public:
	SoldierAnim();
	~SoldierAnim();
	Texture2D GetSheet() const { return spriteSheet; }
	bool IsShooting() const { return isShooting; }
	void LoadTexture();
	void UnloadTextures();

private:
	Texture2D spriteSheet;
	bool isShooting;
};

