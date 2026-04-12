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
    Texture2D background;
    Vector2 origin;
    Rectangle source;
    Rectangle destination;
    float xposSprite;
    float yposSprite;
    float scale;
};