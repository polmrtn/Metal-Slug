#pragma once
#include "raylib.h"

enum class SoldierState {
    IDLE,
    WALKING,
    ATTACKING,
    HURT,
    DYING
};

class Soldier {
public:
    Soldier(int type, Vector2 position);
    Soldier(const Soldier& other);
    ~Soldier();

    void Update();
    void UpdateAI(class Player& player);
    void Draw();
    void DrawHitBox();

    Rectangle GetHitBox();
    int GetType();

    float GetWidth() { return (float)image.width * scale; }
    float GetHeight() { return (float)image.height * scale; }
    bool GetIsGrounded() { return isGrounded; }
    float GetY() const { return position.y; }
    float GetX() const { return position.x; }
    Vector2 GetPosition() const { return position; }

    void SetY(float newY) { position.y = newY; }
    void SetVelocityY(float newVelY) { velocity.y = newVelY; }
    void SetVelocityX(float newVelX) { velocity.x = newVelX; }
    void SetGrounded(bool grounded) { isGrounded = grounded; }
    float GetVelocityY() const { return velocity.y; }

private:
    Texture2D image;
    Vector2 position;
    Vector2 velocity;
    int type;

    float scale;
    float gravity;
    float groundLevel;
    bool isGrounded;

    // Variables de IA (cada soldado tiene las suyas)
    SoldierState currentState;
    float stateTimer;
};