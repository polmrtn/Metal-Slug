#pragma once

#include "raylib.h"

enum class PlayerDirection { LEFT, RIGHT, UP, DOWN };

class Player {
public:
    Player();
    ~Player();

    void Update(float CameraLeftLimit);
    void Draw();
    void DrawHitBox();

    // Movimiento e Input
    void MoveLeft();
    void MoveRight();
    void StopMovingHorizontal();
    void Jump();
    void StartAimingUp();
    void StopAimingUp();
    void StartCrouching();
    void StopCrouching();

    // Getters
    Rectangle GetHitBox();
    Vector2 GetPosition();
    PlayerDirection GetAimDirection() const;
    float GetWidth() const { return (float)image.width * scale; }
    float GetHeight() const { return (isCrouching ? crouchHeight : normalHeight) * scale; }
    bool GetisGrounded() { return isGrounded; }
    float GetY() const { return position.y; }
    void SetY(float newY) { position.y = newY; }
    void SetVelocityY(float newVelY) { velocity.y = newVelY; }
    void SetGrounded(bool grounded) { isGrounded = grounded; }
    float GetVelocityY() const { return velocity.y; }
    
private:
    Texture2D image;
    Vector2 position;
    Vector2 velocity;

    float scale;
    float gravity;
    float jumpForce;
    float moveSpeed;
    float crouchSpeed; // Añadida para coherencia
    float groundLevel;

    float normalHeight;
    float crouchHeight;

    bool isGrounded;
    bool aimingUp;
    bool isCrouching;

    PlayerDirection direction;
};



