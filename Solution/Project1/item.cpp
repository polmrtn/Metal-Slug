#include "item.hpp"

Texture2D Item::textures[8] = {};
bool Item::loaded[8] = { false, false, false, false, false, false, false, false };

Item::Item(Vector2 position, ItemType type) {
    this->position = position;
    this->type = type;

if (type == ItemType::BOX || type == ItemType::JETPACK||
    type == ItemType::PLUSHY ||
    type == ItemType::MEDAL || type == ItemType::BOMBS) {
    hasGravity = true;
}

    int idx = (int)type;
    if (!loaded[idx]) {
        textures[idx] = LoadTexture(Cfg().texturePath);
        SetTextureFilter(textures[idx], TEXTURE_FILTER_POINT);
        loaded[idx] = true;
    }

    currentFrame = 0;
    animTimer = 0.0f;
    isDestroyed = false;
    spawnMachinegun = false;
    destroyFrame = 0;
    destroyTimer = 0.0f;
    velY = 0.0f;
    grounded = false;
    isActive = true;
}

Item::~Item() {}

void Item::Destroy() {
    if (type != ItemType::BOX || isDestroyed) return;
    isDestroyed = true;
    destroyFrame = 0;
    destroyTimer = 0.0f;
    spawnMachinegun = true;
}

void Item::Update() {
    if (!isActive) return;

    if (hasGravity && !grounded) {
        velY += GRAVITY * GetFrameTime();
        position.y += velY * GetFrameTime();
    }

    const ItemConfig& cfg = Cfg();

    if (cfg.animFps > 0 && !isDestroyed) {
        animTimer += GetFrameTime();
        if (animTimer >= 1.0f / cfg.animFps) {
            animTimer = 0.0f;
            currentFrame = (currentFrame + 1) % cfg.totalFrames;
        }
    }

    if (type == ItemType::BOX && isDestroyed) {
        destroyTimer += GetFrameTime();
        if (destroyTimer >= DESTROY_DELAY) {
            destroyTimer = 0.0f;
            destroyFrame++;
            if (destroyFrame >= DESTROY_FRAMES)
                isActive = false;
        }
    }
}

void Item::Draw() {
    if (!isActive) return;

    const ItemConfig& cfg = Cfg();
    Texture2D& tex = textures[(int)type];

    if (type == ItemType::BOX) {
        int frame = isDestroyed ? (destroyFrame + 1) : 0;
        Rectangle src = { cfg.startX + frame * cfg.frameW, cfg.startY, cfg.frameW, cfg.frameH };
        Rectangle dst = { position.x, position.y, cfg.frameW * cfg.scale, cfg.frameH * cfg.scale };
        DrawTexturePro(tex, src, dst, { 0, 0 }, 0, WHITE);
        DrawRectangleLinesEx(GetHitBox(), 2.0f, RED);
        return;
    }

    Rectangle src;
    if (cfg.framesVertical)
        src = { cfg.startX, cfg.startY + currentFrame * (cfg.frameH + cfg.spacing), cfg.frameW, cfg.frameH };
    else
        src = { cfg.startX + currentFrame * (cfg.frameW + cfg.spacing), cfg.startY, cfg.frameW, cfg.frameH };

    float scale = cfg.scale;
    float height = cfg.frameH * scale;
    Rectangle dst = {
        position.x - (cfg.frameW * scale) / 2.0f,
        position.y - (cfg.frameH * scale) + height,
        cfg.frameW * scale,
        cfg.frameH * scale
    };
    DrawTexturePro(tex, src, dst, { 0, 0 }, 0, WHITE);
    DrawRectangleLinesEx(GetHitBox(), 2.0f, YELLOW);
}

Rectangle Item::GetHitBox() const {
    const ItemConfig& cfg = Cfg();

    if (type == ItemType::BOX) {
        float boxW = cfg.frameW * cfg.scale;
        float boxH = 23.0f * cfg.scale;
        float hbY = position.y + cfg.frameH * cfg.scale - boxH;
        return Rectangle{ position.x, hbY, boxW, boxH };
    }

    float w = cfg.frameW * cfg.scale;
    float h = cfg.frameH * cfg.scale;
    return Rectangle{
        position.x - w / 2.0f,
        position.y,
        w, h
    };
}

void Item::Collect() {
    isActive = false;
}