#pragma once
#include <raylib.h>
#include <vector>

struct BackgroundAnim {
    Texture2D texture;
    Rectangle source;
    Rectangle dest;
    int totalFrames;
    int currentFrame;
    float fps;
    float timer;
    float spacing;
    bool active;
    float parallaxFactor = 0.1f;
    bool isFrontground = false;
    int layer = 0;
    int repeatCount = 1;

    void Update(float dt) {
        if (!active) return;
        timer += dt;
        if (timer >= 1.0f / fps) {
            timer = 0.0f;
            currentFrame = (currentFrame + 1) % totalFrames;
            source.x = (float)(currentFrame * ((int)source.width + (int)spacing));
        }
    }
};

struct BackgroundSprite {
    Texture2D texture;
    Rectangle source;
    Rectangle dest;
    float parallaxFactor;
    bool isFrontground = false;
    int layer = 0;
};

struct BackgroundEventSprite {
    Texture2D texture;
    Rectangle source;
    Rectangle dest;
    float parallaxFactor;
    int totalFrames;
    int currentFrame;
    float frameWidth;
    float spacing;
    bool isFrontground = false;
    int layer = 0;

    void SetFrame(int frame) {
        if (frame >= 0 && frame < totalFrames) {
            currentFrame = frame;
            source.x = currentFrame * (frameWidth + spacing);
        }
    }
};

class BackgroundManager {
public:
    BackgroundManager();
    ~BackgroundManager();
    void Init();
    void FollowPlayer(Vector2 cameraPos);
    void AddAnimation(const char* path, Vector2 pos, int frames, float fps, float scale, float spacing, float pFactor, float anchoExtra = 1.0f, bool isFrontground = false, int layer = 0, int repeatcount = 1);
    void AddSprite(const char* path, Vector2 pos, float scale, float parallaxFactor = 0.0f, float anchoExtra = 1.0f, bool isFrontground = false, int layer = 0);
    void AddEventSprite(const char* path, Vector2 pos, int frames, float scale, float spacing, float parallaxFactor = 0.0f, bool isFrontground = false, int layer = 0);
    void SetEventSpriteFrame(int index, int frame);
    void Update(float dt);
    void Draw();
    void Drawfrontground();
    float GetWidth() const { return totalWidth; }
    float GetHeight() const { return totalHeight; }

private:
    Vector2 origin;
    float totalWidth = 0.0f;
    float totalHeight = 0.0f;
    float camX = 0.0f;
    float scale;
    std::vector<BackgroundAnim> animations;
    std::vector<BackgroundSprite> sprites;
    std::vector<BackgroundEventSprite> eventSprites;
};