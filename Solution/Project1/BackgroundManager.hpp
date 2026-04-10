#pragma once
#include <raylib.h>

class BackgroundManager {
public:
	BackgroundManager();
	~BackgroundManager();
	void FollowPlayer(Vector2 playerpos);
	void Draw();
	float destHeight, destWidth, xposSprite, yposSprite;
	float GetWidth() { return (float)background.width * widthScale; }
	float GetHeight() { return 1280; }
private:
	Texture2D background;
	Vector2 origin;
	Rectangle source;
	Rectangle destination;
	
	float widthScale = 2.3;
};
