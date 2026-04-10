#pragma once
#include <raylib.h>
#include <vector>
#include "Player.hpp"

class Block {

public:
    Block(float x, float y, float width, float height);
    void Draw();
    Rectangle GetRect() const;
  
private:
    Rectangle rect;


	
};