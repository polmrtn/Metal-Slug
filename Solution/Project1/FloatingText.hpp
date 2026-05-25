#pragma once
#include <raylib.h>
#include <string>

class FloatingText {
public:
    FloatingText(Vector2 position, const std::string& text);
    void Update(float dt);
    void Draw() const;
    bool IsActive() const { return active; }

private:
    Vector2     position;
    std::string text;
    bool        active = true;
    float       timer = 0.0f;
    bool        blinkOn = true;
    float       blinkTimer = 0.0f;

    static constexpr float TOTAL_DURATION = 2.0f;
    static constexpr float CHAR_DELAY = 0.06f;
    static constexpr float BLINK_RATE = 0.05f;
    static constexpr float CHAR_W = 9.0f;
    static constexpr float CHAR_H = 9.0f;
    static constexpr float SCALE = 1.5f;
    static constexpr float GRAVITY_FT = 400.0f;

    float originY = 0.0f;

    struct CharAnim {
        float y = 0.0f;
        float velY = 0.0f;
        bool  bounced = false;
        bool  active = false;
    };
    static constexpr int MAX_CHARS = 32;
    CharAnim charAnims[MAX_CHARS];
    int   charsSpawned = 0;
    float spawnTimer = 0.0f;

    static Texture2D texture;
    static bool      textureLoaded;

    int GetCharFrame(char c) const;
};