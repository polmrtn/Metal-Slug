#include "BloodEffect.hpp"

Texture2D BloodEffect::texture = { 0 };
bool BloodEffect::textureLoaded = false;

BloodEffect::BloodEffect(Vector2 position) : position(position) {
    if (!textureLoaded) {
        texture = LoadTexture("Graphics/blood40x40.png");
        SetTextureFilter(texture, TEXTURE_FILTER_POINT);
        textureLoaded = true;
    }
}

BloodEffect::~BloodEffect() {}

void BloodEffect::Update() {
    if (!active) return;
    timer += GetFrameTime();
    if (timer >= frameDelay) {
        timer = 0.0f;
        frame++;
        if (frame >= FRAMES) active = false;
    }
}

void BloodEffect::Draw() const {
    if (!active) return;
    Rectangle src = { frame * W, 0, W, H };
    Rectangle dst = { position.x - W * SCALE / 2.0f, position.y - H * SCALE / 2.0f, W * SCALE, H * SCALE };
    DrawTexturePro(texture, src, dst, { 0,0 }, 0, WHITE);
}