#include "LevelMap.hpp"
#include "Player.hpp"

Block::Block(float x, float y, float width, float height) {
    rect = { x, y, width, height };
}

void Block::Draw() {
    DrawRectangleRec(rect, GRAY);
}

Rectangle Block::GetRect() const {
    return rect;
}

