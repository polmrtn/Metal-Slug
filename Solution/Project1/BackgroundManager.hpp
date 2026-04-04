#pragma once
#include <raylib.h>

class BackgroundManager {
public:
	BackgroundManager();
	~BackgroundManager();
	void FollowPlayer();
	void Draw(Rectangle screen, Vector2 origin, Rectangle source);



	
private:
	Texture2D background;



};
