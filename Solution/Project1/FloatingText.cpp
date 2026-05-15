#include "FloatingText.hpp"

Texture2D FloatingText::texture = { 0 };
bool      FloatingText::textureLoaded = false;

FloatingText::FloatingText(Vector2 position, const std::string& text)
    : position(position), text(text)
{
    originY = position.y;
    velY = -80.0f;
    if (!textureLoaded) {
        texture = LoadTexture("Graphics/new fonts and HUDs/points.png");
        SetTextureFilter(texture, TEXTURE_FILTER_POINT);
        textureLoaded = true;
    }
}

int FloatingText::GetCharFrame(char c) const {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a';
    if (c >= '1' && c <= '9') return 26 + (c - '1');
    if (c == '0') return 35;
    if (c == ' ') return -1;
    return -1;
}

void FloatingText::Update(float dt) {
    if (!active) return;
    timer += dt;
    // Física de rebote
    velY += GRAVITY_FT;
    position.y += velY * dt;

    // Rebote cuando vuelve al origen
    if (!bounced && position.y >= originY) {
        position.y = originY;
        velY = -velY * 0.4f;  // rebota con menos fuerza
        bounced = true;
    }
    charTimer += dt;
    if (charsVisible < (int)text.size() && charTimer >= CHAR_DELAY) {
        charTimer = 0.0f;
        charsVisible++;
    }
    blinkTimer += dt;
    if (blinkTimer >= BLINK_RATE) {
        blinkTimer = 0.0f;
        blinkOn = !blinkOn;
    }
    if (timer >= TOTAL_DURATION)
        active = false;
}

void FloatingText::Draw() const {
    if (!active) return;
    TraceLog(LOG_INFO, "FloatingText Update: timer=%.2f charsVisible=%d", timer, charsVisible);

    float totalW = 0;
    for (int i = 0; i < (int)text.size(); ++i)
        totalW += (text[i] == ' ') ? CHAR_W * SCALE * 0.5f : CHAR_W * SCALE;

    float startX = position.x - totalW / 2.0f;
    float x = startX;

    // blinkOn alterna fila roja/amarilla
    float rowY = blinkOn ? 0.0f : CHAR_H;

    for (int i = 0; i < charsVisible && i < (int)text.size(); ++i) {
        char c = text[i];
        if (c == ' ') { x += CHAR_W * SCALE * 0.5f; continue; }

        int frame = GetCharFrame(c);
        if (frame < 0) { x += CHAR_W * SCALE; continue; }

        Rectangle src = { frame * CHAR_W, rowY, CHAR_W, CHAR_H };
        Rectangle dst = { x, position.y, CHAR_W * SCALE, CHAR_H * SCALE };
        DrawTexturePro(texture, src, dst, { 0,0 }, 0, WHITE);
        x += CHAR_W * SCALE;
    }
}