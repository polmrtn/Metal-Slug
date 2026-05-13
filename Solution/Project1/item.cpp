#include "item.hpp"

Texture2D Item::textures[6] = {};
bool Item::loaded[6] = { false, false, false, false, false, false };   

Item::Item(Vector2 position, ItemType type) {
    this->position = position;
    this->type = type;

    int idx = (int)type;
    if (!loaded[idx]) {
        textures[idx] = LoadTexture(Cfg().texturePath);
        SetTextureFilter(textures[idx], TEXTURE_FILTER_POINT);
        loaded[idx] = true;
    }

    // initialize runtime state
    currentFrame = 0;
    animTimer = 0.0f;
    isDestroyed = false;
    spawnItem = false;
    destroyFrame = 0;
    destroyTimer = 0.0f;
    hasGravity = false;
    velY = 0.0f;
    grounded = false;
    isActive = true;
}

Item::~Item() {}

void Item::Destroy() {
    // Only BOX can be destroyed and spawn another item
    if (type != ItemType::BOX || isDestroyed) return;

    isDestroyed = true;
    destroyFrame = 0;
    destroyTimer = 0.0f;
    spawnItem = true; // system collision will create the spawn item
}

void Item::Update() {
    if (!isActive) return;

    // Physics for falling items
    if (hasGravity && !grounded) {
        velY += GRAVITY * GetFrameTime();
        position.y += velY * GetFrameTime();
    }

    const ItemConfig& cfg = Cfg();

    // Auto animation (non-destroy)
    if (cfg.animFps > 0 && !isDestroyed) {
        animTimer += GetFrameTime();
        if (animTimer >= 1.0f / cfg.animFps) {
            animTimer = 0.0f;
            currentFrame = (currentFrame + 1) % cfg.totalFrames;
        }
    }

    // Destruction animation for BOX
    if (type == ItemType::BOX && isDestroyed) {
        destroyTimer += GetFrameTime();
        if (destroyTimer >= DESTROY_DELAY) {
            destroyTimer = 0.0f;
            destroyFrame++;
            if (destroyFrame >= DESTROY_FRAMES) {
                // mark inactive at end of destruction animation
                isActive = false;
            }
        }
    }
}

void Item::Draw() {
    if (!isActive) return;

    const ItemConfig& cfg = Cfg();
    Texture2D& tex = textures[(int)type];

    // BOX: frame 0 = intact, frames 1.. = destruction
    if (type == ItemType::BOX) {
        if (!loaded[(int)ItemType::BOX]) return;

        int frame = isDestroyed ? (destroyFrame + 1) : 0;
        Rectangle src = { cfg.startX + frame * cfg.frameW, cfg.startY, cfg.frameW, cfg.frameH };
        Rectangle dst = { position.x, position.y, cfg.frameW * cfg.scale, cfg.frameH * cfg.scale };
        DrawTexturePro(tex, src, dst, { 0, 0 }, 0, WHITE);
        return;
    }

    // Other items: animation frames arranged according to config
    Rectangle src;
    if (cfg.framesVertical) {
        src = { cfg.startX, cfg.startY + currentFrame * (cfg.frameH + cfg.spacing), cfg.frameW, cfg.frameH };
    } else {
        src = { cfg.startX + currentFrame * (cfg.frameW + cfg.spacing), cfg.startY, cfg.frameW, cfg.frameH };
    }

    float scale = cfg.scale;
    Rectangle dst = {
        position.x - (cfg.frameW * scale) / 2.0f,
        position.y - (cfg.frameH * scale) / 2.0f,
        cfg.frameW * scale,
        cfg.frameH * scale
    };

    DrawTexturePro(tex, src, dst, { 0, 0 }, 0, WHITE);
}

Rectangle Item::GetHitBox() const {
    const ItemConfig& cfg = Cfg();

    if (type == ItemType::BOX) {
        // Use a smaller bottom-aligned hitbox for the box (ground contact area)
        float boxW = cfg.frameW * cfg.scale;
        float boxH = 23.0f * cfg.scale; // approximate foot/hit area from sprite
        float hbY = position.y + cfg.frameH * cfg.scale - boxH;
        return Rectangle{ position.x, hbY, boxW, boxH };
    }

    // Default centered hitbox for other items
    float w = cfg.frameW * cfg.scale;
    float h = cfg.frameH * cfg.scale;
    return Rectangle{
        position.x - w / 2.0f,
        position.y - h / 2.0f,
        w, h
    };
}

void Item::Collect() {
    // If an item is collectible by player, deactivate it on collect
    if (Cfg().collectedByPlayer) isActive = false;
}