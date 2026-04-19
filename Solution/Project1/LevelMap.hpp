#pragma once
#include <raylib.h>
#include <vector>
#include "Player.hpp"

class Block {

public:
    Block(float x, float y, float width, float height, bool ground = true);
    void Draw();
    Rectangle GetRect() const;
    bool IsGround() const { return isGround; }
  
private:
    Rectangle rect;
    bool isGround;
};