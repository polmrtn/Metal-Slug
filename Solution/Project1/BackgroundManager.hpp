#pragma once
#include <raylib.h>

class BackgroundManager {
public:
    BackgroundManager();
    ~BackgroundManager();
    void FollowPlayer(Vector2 cameraPos);
    void Draw();
    float GetWidth() { return destination.width; }
    float GetHeight() { return destination.height; }
 

private:
	Texture2D parallax1, background, parallax2;
    Vector2 origin;
    Rectangle source , source1, source2;
    Rectangle destination,destination1, destination2; 
    float xposSprite;
    float yposSprite;
    float scale;
    float parallaxOffset1 = 0.0f;
    float parallaxOffset2 = 0.0f;
    
};