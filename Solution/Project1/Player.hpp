#pragma once
#include "raylib.h"
#include "playerAnim.hpp"

enum class PlayerDirection { LEFT, RIGHT, UP, DOWN };
enum class SpecialAnim { NONE, FALLING_START, CROUCH, CROUCH_SHOOT, DEATH, RESPAWN };

struct GrenadeThrowData {
    Vector2 startPos;
    Vector2 targetPos;
    float power;
    bool valid;
};

class Player {
public:
    Player();
    ~Player();

    // Actualización y dibujo
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
    float GetX() const { return pos.x; }
    float GetY() const { return pos.y; }
    float GetVelocityX() const { return vel.x; }
    float GetVelocityY() const { return vel.y; }
    bool IsCrouching() const { return crouching; }
    float GetPreviousY() const { return previousY; }

    // Setters
    void SetY(float newY) { pos.y = newY; }
    void SetX(float newX) { pos.x = newX; }
    void SetVelocityY(float newVelY) { vel.y = newVelY; }
    void SetVelocityX(float newVelX) { vel.x = newVelX; }
    void SetGrounded(bool g) { grounded = g; }
    void SetInputVelX(float val) { inputVelX = val; }
    void SavePreviousPosition() { previousY = pos.y; }

    // Hitbox
    void SetNormalHitbox();
    void SetCrouchHitbox();

    // Hitboxes laterales
    Rectangle GetLeftHitBox();
    Rectangle GetRightHitBox();

    // Colisiones laterales
    void SetLeftCollision(bool colliding) { leftCollision = colliding; }
    void SetRightCollision(bool colliding) { rightCollision = colliding; }
    bool GetLeftCollision() const { return leftCollision; }
    bool GetRightCollision() const { return rightCollision; }

    //Muerte
    void TakeDamage();           
    bool IsAlive() const { return isAlive; }  
    void Respawn();              
    Vector2 GetDeathPosition() const { return deathPosition; }
    bool IsInvincible() const { return invincibilityTimer > 0.0f; }

    GrenadeThrowData ThrowGrenade();

private:
    // Animación
    PlayerAnim anim;

    // Posición y física
    Vector2 pos = { 0.0f, 100.0f };
    Vector2 vel = { 0.0f, 0.0f };
    float inputVelX = 0.0f;  // Velocidad deseada por input (para animación)
    float previousY = 0.0f;

    // Estados
    bool grounded = false;
    bool aimingUp = false;
    bool crouching = false;
    PlayerDirection dir = PlayerDirection::RIGHT;

    // Colisiones laterales
    bool leftCollision = false;
    bool rightCollision = false;

    // Constantes físicas
    static constexpr float SCALE = 4.0f;
    static constexpr float GRAVITY = 2.5f;
    static constexpr float JUMP_FORCE = -42.0f;
    static constexpr float MOVE_SPEED = 10.0f;
    static constexpr float CROUCH_SPEED = 4.0f;
    static constexpr float NORMAL_H = 34.0f;

    // Hitbox
    float hitboxWidth;
    float hitboxHeight;
    float hitboxOffsetX;
    float hitboxOffsetY;

    // Modos de animación
    enum class Mode { SEPARATED, FULL_BODY };
    Mode mode = Mode::SEPARATED;
    SpecialAnim special = SpecialAnim::NONE;
    float specialTimer = 0.0f;
    float specialDuration = 0.0f;

    // Añade esto para la animación de muerte
    float deathAnimTimer = 0.0f;
    float deathAnimDuration = 1.0f;  // Duración de la animación de muerte
    bool isDying = false;  // para saber si está en animación de muerte

    // Métodos de dibujo
    void DrawSeparated();
    void DrawFullBody();
    void DrawCrouch();
    void DrawDeathAnimation();
    Rectangle GetFullBodyRect();
    float GetFullBodyH() const;

    bool isAlive = true;        
    Vector2 deathPosition = { 0.0f, 0.0f };
    float invincibilityTimer = 0.0f; 
    float invincibilityDuration = 2.0f;
    float deathTimer = 0.0f;
    float disappearDelay = 1.0f;
    bool isDisappeared = false; 
};