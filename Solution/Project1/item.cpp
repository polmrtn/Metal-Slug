#include "Item.hpp"

Texture2D Item::textures[5] = {};
bool Item::loaded[5] = { false, false, false, false, false };

Item::Item(Vector2 position, ItemType type) {
    this->position = position;
    this->type = type;

    int idx = (int)type;
    if (!loaded[idx]) {
        textures[idx] = LoadTexture(Cfg().texturePath);
        SetTextureFilter(textures[idx], TEXTURE_FILTER_POINT);
        loaded[idx] = true;
    }
}

Item::~Item() {}

void Item::Collect() {
    if (!Cfg().collectedByPlayer) return;
    isActive = false;
}

void Item::Destroy() {
    if (!Cfg().destroyedByBullet || isDestroyed) return;
    isDestroyed = true;
    destroyFrame = 0;
    destroyTimer = 0.0f;
    spawnItem = true;
}

void Item::Update() {
    if (!isActive) return;

    // Física de caída
    if (hasGravity && !isGrounded) {
        velY += GRAVITY * GetFrameTime();
        position.y += velY * GetFrameTime();
    }

    const ItemConfig& cfg = Cfg();

    // Animación automática
    if (cfg.animFps > 0 && !isDestroyed) {
        animTimer += GetFrameTime();
        if (animTimer >= 1.0f / cfg.animFps) {
            animTimer = 0.0f;
            currentFrame = (currentFrame + 1) % cfg.totalFrames;
        }
    }

    // Animación de destrucción (BOX)
    if (isDestroyed) {
        destroyTimer += GetFrameTime();
        if (destroyTimer >= DESTROY_DELAY) {
            destroyTimer = 0.0f;
            destroyFrame++;
            if (destroyFrame >= DESTROY_FRAMES) {
                isActive = false;
            }
        }
    }
}

void Item::Draw() {
    if (!isActive) return;

    const ItemConfig& cfg = Cfg();
    Texture2D& tex = textures[(int)type];

    // BOX — frame 0 intacta, frames 1-4 destrucción
    if (type == ItemType::BOX) {
        int frame = isDestroyed ? (destroyFrame + 1) : 0;
        Rectangle src = { frame * cfg.frameW, 0.0f, cfg.frameW, cfg.frameH };
        Rectangle dst = { position.x, position.y, cfg.frameW * cfg.scale, cfg.frameH * cfg.scale };
        DrawTexturePro(tex, src, dst, { 0, 0 }, 0, WHITE);
        return;
    }

    // Items con animación o estáticos
    Rectangle src;
    if (cfg.framesVertical) {
        src = { cfg.startX, cfg.startY + currentFrame * (cfg.frameH + cfg.spacing), cfg.frameW, cfg.frameH };
    }
    else {
        src = { cfg.startX + currentFrame * (cfg.frameW + cfg.spacing), cfg.startY, cfg.frameW, cfg.frameH };
    }

    Rectangle dst = {
        position.x - (cfg.frameW * cfg.scale) / 2.0f,
        position.y - (cfg.frameH * cfg.scale) / 2.0f,
        cfg.frameW * cfg.scale,
        cfg.frameH * cfg.scale
    };

    DrawTexturePro(tex, src, dst, { 0, 0 }, 0, WHITE);
}

Rectangle Item::GetHitBox() const {
    const ItemConfig& cfg = Cfg();
    float w = cfg.frameW * cfg.scale;
    float h = cfg.frameH * cfg.scale;

    if (type == ItemType::BOX) {
        return Rectangle{ position.x, position.y, w, h };
    }
    return Rectangle{
        position.x - w / 2.0f,
        position.y - h / 2.0f,
        w, h
    };
}
