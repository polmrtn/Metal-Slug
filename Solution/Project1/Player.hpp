#pragma once
#include "raylib.h"

class Player {
public:
    Player();
    ~Player();

    void Update(); 
    void Draw();
    void StopMoving();
    void MoveLeft();
    void MoveRight();
    void Jump();

private:
    Texture2D image;
    Vector2 position;
    Vector2 velocity; 
};