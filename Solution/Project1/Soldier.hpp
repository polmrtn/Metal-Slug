#pragma once

#include "raylib.h"

class Soldier {
public:
    Soldier(int type, Vector2 position);
    Soldier(const Soldier& other); // Constructor de copia
    ~Soldier();

    void Update();
    void Draw();
    void DrawHitBox();

    Rectangle GetHitBox();
    int GetType();

    // Getters de dimensiones escaladas
    float GetWidth()  { return (float)image.width * scale; }
    float GetHeight()  { return (float)image.height * scale; }

private:
    Texture2D image;
    Vector2 position;
    Vector2 velocity;
    int type;

    float scale;
    float gravity;
    float groundLevel;
    bool isGrounded;
};



