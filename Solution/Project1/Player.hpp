#pragma once
#include "raylib.h"
#include "playerAnim.hpp"

// Direcciones de apuntado del jugador
enum class PlayerDirection { LEFT, RIGHT, UP, DOWN };

// Animaciones especiales de cuerpo completo
enum class SpecialAnim { NONE, FALLING_START, CROUCH, CROUCH_SHOOT, DEATH, RESPAWN };

// Tipos de arma
enum class WeaponType { PISTOL, MACHINEGUN };

// Datos para lanzar una granada
struct GrenadeThrowData {
    Vector2 startPos;
    Vector2 initialVelocity;  // velocidad inicial directa, sin target hardcodeado
    bool valid;
};

class Player {
public:
    Player();
    ~Player();
    void Init();

    // ========== ACTUALIZACIÓN Y DIBUJO ==========
    void Update(float CameraLeftLimit, float CameraTop = -99999.0f);
    void Draw();
    void DrawHitBox();

    // ========== MOVIMIENTO E INPUT ==========
    void MoveLeft();
    void MoveRight();
    void StopMovingHorizontal();
    void Jump();
    void StartAimingUp();
    void StopAimingUp();
    void StartCrouching();
    void StopCrouching();
    void Shoot();

    // ========== GETTERS ==========
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

    // ========== SETTERS ==========
    void SetY(float newY) { pos.y = newY; }
    void SetX(float newX) { pos.x = newX; }
    void SetVelocityY(float newVelY) { vel.y = newVelY; }
    void SetVelocityX(float newVelX) { vel.x = newVelX; }
    void SetGrounded(bool g) { grounded = g; }
    void SetInputVelX(float val) { inputVelX = val; }
    void SavePreviousPosition() { previousY = pos.y; }

    // ========== HITBOX ==========
    void SetNormalHitbox();
    void SetCrouchHitbox();
    Rectangle GetLeftHitBox();
    Rectangle GetRightHitBox();

    // ========== COLISIONES LATERALES ==========
    void SetLeftCollision(bool colliding) { leftCollision = colliding; }
    void SetRightCollision(bool colliding) { rightCollision = colliding; }
    bool GetLeftCollision() const { return leftCollision; }
    bool GetRightCollision() const { return rightCollision; }

    // ========== MUERTE Y RESPAWN ==========
    void TakeDamage();
    bool IsAlive() const { return isAlive; }
    bool IsDisappeared() const { return isDisappeared; }
    void Respawn();
    Vector2 GetDeathPosition() const { return deathPosition; }
    bool IsInvincible() const { return invincibilityTimer > 0.0f; }

    // ========== ARMAS ==========
    GrenadeThrowData ThrowGrenade();
    void EquipMachinegun();
    void UseAmmo();
    int GetAmmo() const { return machinegunAmmo; }
    WeaponType GetCurrentWeapon() const { return currentWeapon; }

    bool IsFalling() const { return isFalling; }
    PlayerAnim& GetAnim() { return anim; }
    bool isLanding = false;
    void ResetToStart();

    void SetWasOnRamp(bool val) { wasOnRamp = val; }
    bool GetWasOnRamp() const { return wasOnRamp; }
    void SetRampGroundedFrames(int f) { rampGroundedFrames = f; }
    int GetRampGroundedFrames() const { return rampGroundedFrames; }

    void SetDeathPosition(Vector2 pos) { deathPosition = pos; }
    void SetDyingInAir(bool val) { dyingInAir = val; }
    bool IsDyingInAir() const { return dyingInAir; }
    float GetNormalHeight() const { return 40.0f * SCALE; }
    // ========== MELEE ==========
    void StartMelee();
    bool IsMeleeAttacking() const { return meleeAttacking; }
    Rectangle GetMeleeHitBox() const;

    // ========== JETPACK ==========
    void EquipJetpack();
    bool HasJetpack() const { return hasJetpack; }
    void JetpackThrust();
    float GetJetpackFuel() const { return jetpackFuel; }
    float GetJetpackMaxFuel() const { return JETPACK_MAX_FUEL; }

    void FullReset();
    void ResetWeapon();
    void EquipMachinegunFresh();

private:
    // ========== ANIMACIÓN ==========
    PlayerAnim anim;

    // ========== POSICIÓN Y FÍSICA ==========
    Vector2 pos = { 300.0f, -50.0f };
    Vector2 vel = { 0.0f, 0.0f };
    float inputVelX = 0.0f;     // Velocidad deseada por input (para animación)
    float previousY = 0.0f;     // Posición Y del frame anterior

    // ========== ESTADOS ==========
    bool grounded = false;
    bool aimingUp = false;
    bool crouching = false;
    PlayerDirection dir = PlayerDirection::RIGHT;

    // ========== COLISIONES LATERALES ==========
    bool leftCollision = false;
    bool rightCollision = false;

    // ========== CONSTANTES FÍSICAS ==========
    static constexpr float SCALE = 4.0f;
    static constexpr float GRAVITY = 2.5f;
    static constexpr float JUMP_FORCE = -34.0f;
    static constexpr float MOVE_SPEED = 12.0f;
    static constexpr float CROUCH_SPEED = 6.0f;

    // ========== HITBOX ==========
    float hitboxWidth;
    float hitboxHeight;
    float hitboxOffsetX;
    float hitboxOffsetY;

    // ========== MODOS DE ANIMACIÓN ==========
    enum class Mode { SEPARATED, FULL_BODY };
    Mode mode = Mode::SEPARATED;
    SpecialAnim special = SpecialAnim::NONE;
    float specialTimer = 0.0f;
    float specialDuration = 0.0f;

    // ========== MUERTE ==========
    bool isAlive = true;
    Vector2 deathPosition = { 0.0f, 0.0f };
    float invincibilityTimer = 0.0f;
    float invincibilityDuration = 2.0f;     // Duración de invencibilidad tras respawn
    float deathTimer = 0.0f;
    float disappearDelay = 1.0f;            // Tiempo hasta desaparecer tras morir
    bool isDisappeared = false;

    // ========== ARMAS ==========
    WeaponType currentWeapon = WeaponType::PISTOL;
    int machinegunAmmo = 0;
    static constexpr int MACHINEGUN_MAX_AMMO = 200;
    bool pendingStopAimingUp = false;
    bool isShootingUpBlocked = false;

    // ========== MÉTODOS DE DIBUJO PRIVADOS ==========
    void DrawSeparated();
    void DrawFullBody();
    void DrawCrouch();
    Rectangle GetFullBodyRect();
    float GetFullBodyH() const;

    // ========== CAÍDA INICIAL ==========
    bool isFalling = true;
    float fallSpeed = 6.5f;  // más lento que la gravedad normal
    float blinkTimer = 0.0f;
    float blinkDelay = 0.1f;
    bool blinkVisible = true;
    bool wasOnRamp = false;
    int rampGroundedFrames = 0;

    bool dyingInAir = false;
    float deathFallGravity = 2.5f;

    // ========== MELEE ==========
    bool  meleeAttacking = false;
    float meleeTimer = 0.0f;
    static constexpr float MELEE_DURATION = 0.3f;

    // ========== JETPACK ==========
    bool  hasJetpack = false;
    float jetpackFuel = 0.0f;
    static constexpr float JETPACK_MAX_FUEL = 100.0f;
    static constexpr float JETPACK_FUEL_DRAIN = 25.0f;  // por segundo
    static constexpr float JETPACK_FORCE = -3.5f;
    static constexpr float JETPACK_MAX_VEL = -18.0f;

    float fallingTimer = 0.0f;
    static constexpr float PARACHUTE_RELEASE_TIME = 2.0f;
    static constexpr float PARACHUTE_FALL_GRAVITY = 2.0f;  // ← ajusta, más bajo que GRAVITY=2.5
};