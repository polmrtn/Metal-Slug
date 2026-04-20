#include "LevelMap.hpp"
#include "Player.hpp"

Block::Block(float x, float y, float width, float height, bool ground) {
    rect = { x, y, width, height };
    this->isGround = ground;
    this->rotation = 0.0f;
}

void Block::Draw() {
    // Dibujo normal sin rotación
    if (isGround) {
        DrawRectangle(rect.x, rect.y, rect.width, rect.height, ColorAlpha(BLUE, 0.3f));
    }
    else {
        DrawRectangle(rect.x, rect.y, rect.width, rect.height, ColorAlpha(GREEN, 0.3f));
    }
}

void Block::Draw(float rotation) {
    TraceLog(LOG_INFO, "Drawing block with rotation: %.0f", rotation);  // ← Debug

    Vector2 origin = { rect.width / 2, rect.height / 2 };
    Rectangle destRect = { rect.x + rect.width / 2, rect.y + rect.height / 2, rect.width, rect.height };

    if (isGround) {
        DrawRectanglePro(destRect, origin, rotation, ColorAlpha(BLUE, 0.3f));
    }
    else {
        DrawRectanglePro(destRect, origin, rotation, ColorAlpha(GREEN, 0.3f));
    }
}

Rectangle Block::GetRect() const {
    return rect;
}