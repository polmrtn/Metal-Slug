#include "Item.hpp"

Texture2D Item::texture = { 0 };
bool Item::textureLoaded = false;
Texture2D Item::boxTexture = { 0 };
bool Item::boxTextureLoaded = false;

Item::Item(Vector2 position, ItemType type) {
    this->position = position;
    this->type = type;

    if (!textureLoaded) {
        texture = LoadTexture("Graphics/items.png");
        SetTextureFilter(texture, TEXTURE_FILTER_POINT);
        textureLoaded = true;
    }

    if (!boxTextureLoaded && type == ItemType::BOX) {
        boxTexture = LoadTexture("Graphics/ammocratedestoyed.png");
        SetTextureFilter(boxTexture, TEXTURE_FILTER_POINT);
        boxTextureLoaded = true;
    }
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
    // Física de caída para items con gravedad
    if (hasGravity && !grounded) {
        velY += GRAVITY * GetFrameTime();
        position.y += velY * GetFrameTime();
    }

    if (type != ItemType::BOX || !isDestroyed) return;
    if (destroyFrame >= DESTROY_FRAMES - 1) return;

    destroyTimer += GetFrameTime();
    if (destroyTimer >= DESTROY_DELAY) {
        destroyTimer = 0.0f;
        destroyFrame++;
    }
    if (destroyFrame >= DESTROY_FRAMES - 1) {
        isActive = false;
    }
}

void Item::Draw() {
    if (!isActive) return;

    if (type == ItemType::BOX) {
        if (!boxTextureLoaded) return;

        // JETPACK — cuadrado verde temporal
        if (type == ItemType::JETPACK) {
            DrawRectangleRec(GetHitBox(), GREEN);
            DrawRectangleLinesEx(GetHitBox(), 2.0f, DARKGREEN);
            return;
        }

        // Frame 0 = caja intacta, frames 1-4 = destrucción
        int frame = isDestroyed ? (destroyFrame + 1) : 0;

        Rectangle src = {
            frame * BOX_W,
            0.0f,
            BOX_W,
            BOX_H
        };
        Rectangle dst = {
            position.x,
            position.y,
            BOX_W * BOX_SCALE,
            BOX_H * BOX_SCALE
        };
        DrawTexturePro(boxTexture, src, dst, { 0, 0 }, 0, WHITE);
        return;
    }

    // Item normal (machinegun)
    if (!textureLoaded) return;

    float frameWidth = 32.0f;
    float frameHeight = 32.0f;
    int column = 3;

    Rectangle sourceRect = { column * frameWidth, 0, frameWidth, frameHeight };
    float scale = 3.0f;
    Rectangle destRect = {
        position.x - (frameWidth * scale) / 2,
        position.y - (frameHeight * scale) / 2,
        frameWidth * scale,
        frameHeight * scale
    };
    DrawTexturePro(texture, sourceRect, destRect, { 0, 0 }, 0, WHITE);
}

Rectangle Item::GetHitBox() const {
    if (type == ItemType::BOX) {
        float hbH = 23.0f * BOX_SCALE;
        float hbY = position.y + BOX_H * BOX_SCALE - hbH;  // ← empieza desde abajo
        return Rectangle{ position.x, hbY, BOX_W * BOX_SCALE, hbH };
    }
    if (type == ItemType::JETPACK) {
        return Rectangle{ position.x, position.y, 40.0f, 40.0f };
    }
    float width = 22.0f * 3.0f;
    float height = 22.0f * 3.0f;
    return Rectangle{
        position.x - width / 2,
        position.y - height / 2,
        width, 
        height
    };
}

void Item::Collect() {
    isActive = false;
}