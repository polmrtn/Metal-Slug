#include "LevelMap.hpp"
#include "Player.hpp"

Block::Block(float x, float y, float width, float height) {
    rect = { x, y, width, height };
}

void Block::Draw() {
    DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, BLUE);
}

Rectangle Block::GetRect() const {
    return rect;
}

