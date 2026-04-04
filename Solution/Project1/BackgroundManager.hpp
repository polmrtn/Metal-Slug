#pragma once
#include <raylib.h>

class BackgroundManager {
public:
	BackgroundManager();
	~BackgroundManager();
	void FollowPlayer(Vector2 playerpos);
	void Draw();
	float destHeight,destWidth, xposSprite, yposSprite;
	
	
	
private:
	Texture2D background;
	Vector2 origin;
	Rectangle source;
	Rectangle destination;


};
