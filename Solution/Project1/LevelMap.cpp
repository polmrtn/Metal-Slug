#include "LevelMap.hpp"
#include "Player.hpp"

Block::Block(float x, float y, float width, float height, bool ground) {
    rect = { x, y, width, height };
    this->isGround = ground;
}

void Block::Draw() {
    if (isGround) {
        DrawRectangle(rect.x, rect.y, rect.width, rect.height, ColorAlpha(BLUE, 0.3f));
    }
    else {
        DrawRectangle(rect.x, rect.y, rect.width, rect.height, ColorAlpha(GREEN, 0.3f));
    }
}

Rectangle Block::GetRect() const {
    return rect;
}

