#pragma once

#include "raylib.h"

enum class PlayerDirection { LEFT, RIGHT, UP, DOWN };

class Player {
public:
    Player();
    ~Player();

    void Update();
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
    void Shoot();

    // Getters
    Rectangle GetHitBox();
    Vector2 GetPosition() const { return position; }
    PlayerDirection GetAimDirection() const;
    float GetWidth() const { return (float)image.width * scale; }
    float GetHeight() const { return (isCrouching ? crouchHeight : normalHeight) * scale; }

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



