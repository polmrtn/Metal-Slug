#include "Item.hpp"

Texture2D Item::texture = { 0 };
bool Item::textureLoaded = false;

Item::Item(Vector2 position, ItemType type) {
    this->position = position;
    this->type = type;

    if (!textureLoaded) {
        texture = LoadTexture("Graphics/items.png");
        SetTextureFilter(texture, TEXTURE_FILTER_POINT);
        textureLoaded = true;
    }
}

Item::~Item() {
    // No descargar aquí
}

void Item::Draw() {
    if (!isActive) return;
    if (!textureLoaded) return;

    float frameWidth = 32.0f;
    float frameHeight = 32.0f;
    int column = 3;

    Rectangle sourceRect = {
        column* frameWidth,  
        0,                    
        frameWidth,
        frameHeight
    };

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