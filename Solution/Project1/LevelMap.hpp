#pragma once
#include <raylib.h>
#include <vector>
#include "Player.hpp"

class Block {

public:
    Block(float x, float y, float width, float height, bool ground = true);
    void Draw();
    void Draw(float rotation); 
    Rectangle GetRect() const;
    bool IsGround() const { return isGround; }

    void SetRotation(float rot) { rotation = rot; }
    float GetRotation() const { return rotation; }

private:
    Rectangle rect;
    bool isGround;
    float rotation = 0.0f;  
};