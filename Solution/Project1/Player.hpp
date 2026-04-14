#pragma once
#include "raylib.h"
#include "playerAnim.hpp"

enum class PlayerDirection { LEFT, RIGHT, UP, DOWN };
enum class SpecialAnim { NONE, FALLING_START, CROUCH, CROUCH_SHOOT };

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
    void Shoot();

    // Getters
    Rectangle GetHitBox();
    Vector2 GetPosition();
    PlayerDirection GetAimDirection() const;
    float GetWidth() const { return hitboxWidth; }
    float GetHeight() const;
    bool GetIsGrounded() const { return grounded; }
    float GetY() const { return pos.y; }
    float GetVelocityX() const { return vel.x; }
    float GetVelocityY() const { return vel.y; }
    bool IsCrouching() const { return crouching; }

    // Setters
    void SetY(float newY) { pos.y = newY; }
    void SetX(float newX) { pos.x = newX; }
    void SetVelocityY(float newVelY) { vel.y = newVelY; }
    void SetVelocityX(float newVelX) { vel.x = newVelX; }  // ← AÑADIR
    void SetGrounded(bool g) { grounded = g; }

    // Hitbox
    void SetNormalHitbox();
    void SetCrouchHitbox();

private:
    PlayerAnim anim;

    Vector2 pos = { 0, 100 };
    Vector2 vel = { 0, 0 };
    bool grounded = false;
    bool aimingUp = false;
    bool crouching = false;

    PlayerDirection dir = PlayerDirection::RIGHT;

    // Constantes físicas
    static constexpr float SCALE = 4.0f;
    static constexpr float GRAVITY = 2.5f;
    static constexpr float JUMP_FORCE = -42.0f;
    static constexpr float MOVE_SPEED = 10.0f;
    static constexpr float CROUCH_SPEED = 4.0f;
    static constexpr float NORMAL_H = 34.0f;

    // ========== HITBOX ==========
    float hitboxWidth;
    float hitboxHeight;
    float hitboxOffsetX;
    float hitboxOffsetY;

    enum class Mode { SEPARATED, FULL_BODY };
    Mode mode = Mode::SEPARATED;
    SpecialAnim special = SpecialAnim::NONE;
    float specialTimer = 0;
    float specialDuration = 0;

    void DrawSeparated();
    void DrawFullBody();
    void DrawCrouch();
    Rectangle GetFullBodyRect();
    float GetFullBodyH() const;
};