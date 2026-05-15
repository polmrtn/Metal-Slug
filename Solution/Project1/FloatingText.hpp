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
    float       charTimer = 0.0f;
    int         charsVisible = 0;
    bool        blinkOn = true;
    float       blinkTimer = 0.0f;

    static constexpr float TOTAL_DURATION = 2.0f;
    static constexpr float CHAR_DELAY = 0.06f;
    static constexpr float BLINK_RATE = 0.05f;
    static constexpr float CHAR_W = 9.0f;
    static constexpr float CHAR_H = 9.0f;
    static constexpr float SCALE = 1.5f;
    static constexpr float FLOAT_SPEED = 20.0f;  // sube lentamente

    static Texture2D texture;
    static bool      textureLoaded;

    int GetCharFrame(char c) const;

    float velY = -3.0f;      // velocidad inicial hacia arriba
    float bounceY = 0.0f;   // posición relativa al origen
    float originY = 0.0f;   // posición Y inicial
    bool bounced = false;
    static constexpr float GRAVITY_FT = 8.0f;  // gravedad del floating text

};