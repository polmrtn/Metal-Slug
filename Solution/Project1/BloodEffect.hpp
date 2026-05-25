#pragma once
#include <raylib.h>

class BloodEffect {
public:
    BloodEffect(Vector2 position);
    ~BloodEffect();
    void Update();
    void Draw() const;
    bool IsActive() const { return active; }

private:
    Vector2 position;
    int     frame = 0;
    float   timer = 0.0f;
    float   frameDelay = 0.1f;
    bool    active = true;
    static constexpr int   FRAMES = 8;
    static constexpr float W = 40.0f;
    static constexpr float H = 40.0f;
    static constexpr float SCALE = 3.0f;

    static Texture2D texture;
    static bool textureLoaded;
};