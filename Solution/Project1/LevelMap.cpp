#include "LevelMap.hpp"
#include "Player.hpp"

Block::Block(float x, float y, float width, float height, bool ground) {
    rect = { x, y, width, height };
    this->isGround = ground;
}

void Block::Draw() {
    if (isGround) {
        DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, BLUE);
    }
    else {
        DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, GREEN);
    }
}

Rectangle Block::GetRect() const {
    return rect;
}

