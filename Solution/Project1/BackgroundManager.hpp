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
    float parallaxFactor = 0.1f; // Lo añadimos aquí para que cada anim tenga su peso

    void Update(float dt) {
        if (!active) return;
        timer += dt;
        if (timer >= 1.0f / fps) {
            timer = 0.0f;
            currentFrame = (currentFrame + 1) % totalFrames;
            // Cálculo preciso con el espaciado
            source.x = (float)(currentFrame * ((int)source.width + (int)spacing));
        }
    }
};

class BackgroundManager {
public:

    BackgroundManager();
    ~BackgroundManager();
    void FollowPlayer(Vector2 cameraPos);
    void AddAnimation(const char* path, Vector2 pos, int frames, float fps, float scale, float spacing, float pFactor);
    void Update(float dt);
    void Draw();
    void Drawfrontground();
    float GetWidth() { return destination.width; }
    float GetHeight() { return destination.height; }

private:
    Texture2D parallax1, background, parallax2;
    Vector2 origin;
    Rectangle source, source1, source2;
    Rectangle destination, destination1, destination2;

    float camX = 0.0f; // DECLARADA CORRECTAMENTE
    float parallaxOffset1 = 0.0f;
    float scale;

    std::vector<BackgroundAnim> animations;
};