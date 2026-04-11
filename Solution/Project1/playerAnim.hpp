#pragma once
#include "raylib.h"

// Estados de animación para piernas y torso
enum class LegsAnim { IDLE, WALKING, JUMPING };
enum class TorsoAnim { IDLE, WALKING, JUMPING, SHOOTING };

// Offsets visuales SOLO para piernas
struct VisualOffsets { float legsX, legsY; };

class PlayerAnim {
public:
    PlayerAnim();
    ~PlayerAnim();

    void LoadTextures();
    void UnloadTextures();

    // Actualizar todas las animaciones
    void Update(bool grounded, float velX, bool crouchingInput, bool aimingUpInput, float dt);

    // Getters principales
    Texture2D GetSheet() const { return spriteSheet; }
    VisualOffsets GetOffsets() const;
    LegsAnim GetLegsAnim() const { return legsAnim; }
    TorsoAnim GetTorsoAnim() const { return torsoAnim; }
    bool IsShooting() const { return shooting; }

    // Frames actuales
    int GetIdleFrame() const { return idleFrame; }
    int GetWalkLegsFrame() const { return walkLegsFrame; }
    int GetWalkTorsoFrame() const { return walkTorsoFrame; }
    int GetJumpLegsFrame() const { return jumpLegsFrame; }
    int GetJumpTorsoFrame() const { return jumpTorsoFrame; }
    int GetShootFrame() const { return shootFrame; }

    // Dimensiones y filas del sprite sheet
    float GetW() const { return 34.0f; }
    float GetH() const { return 34.0f; }
    float GetShootW() const { return 68.0f; }
    float GetRowIdle() const { return 7 * 34.0f; }
    float GetRowWalkTorso() const { return 8 * 34.0f; }
    float GetRowWalkLegs() const { return 9 * 34.0f; }
    float GetRowJumpTorso() const { return 10 * 34.0f; }
    float GetRowJumpLegs() const { return 11 * 34.0f; }
    float GetRowShoot() const { return 12 * 34.0f; }

    // Getters para agachado
    int GetCrouchFrame() const { return crouchFrame; }
    bool IsCrouching() const { return crouching; }
    bool IsCrouchTransition() const { return crouchTransition; }
    float GetCrouchTransitionRowY() const { return 18 * 34.0f; }
    float GetCrouchIdleRowY() const { return 20 * 34.0f; }
    float GetCrouchTransitionH() const { return 68.0f; }
    float GetCrouchIdleH() const { return 34.0f; }

    // Getters para aiming up
    int GetAimingFrame() const { return aimingFrame; }
    bool IsAimingUp() const { return aimingUp; }
    bool IsAimingTransition() const { return aimingTransition; }
    float GetAimingTransitionRowY() const { return 14 * 34.0f; }
    float GetAimingIdleRowY() const { return 15 * 34.0f; }

    // Getters para shooting up
    int GetShootUpFrame() const { return shootUpFrame; }
    bool IsShootingUp() const { return shootingUp; }
    float GetShootUpRowY() const { return 16 * 34.0f; }
    float GetShootUpH() const { return 68.0f; }

    // Getters para caminar agachado
    int GetCrouchWalkFrame() const { return crouchWalkFrame; }
    bool IsCrouchWalking() const { return crouchWalking; }
    float GetCrouchWalkRowY() const { return 21 * 34.0f; }  // Fila 21

    // Getters para disparo agachado
    int GetCrouchShootFrame() const { return crouchShootFrame; }
    bool IsCrouchShooting() const { return crouchShooting; }
    float GetCrouchShootRowY() const { return 22 * 34.0f; }  // Fila 22
    float GetCrouchShootW() const { return 68.0f; }          // Ancho 68px


    // Control de animaciones
    void StartShoot();
    void StartShootUp();
    void ForceStopShoot();
    void ResetJump();
    void ForceCrouch();
    void StartCrouchShoot();

private:
    Texture2D spriteSheet;

    // Estados de animación
    LegsAnim legsAnim = LegsAnim::IDLE;
    TorsoAnim torsoAnim = TorsoAnim::IDLE;
    bool shooting = false;

    // Frames
    int idleFrame = 0;
    int walkLegsFrame = 0, walkTorsoFrame = 0;
    int jumpLegsFrame = 0, jumpTorsoFrame = 0;
    int shootFrame = 0;

    // Temporizadores
    float idleTimer = 0.0f, walkLegsTimer = 0.0f, walkTorsoTimer = 0.0f;
    float jumpLegsTimer = 0.0f, jumpTorsoTimer = 0.0f, shootTimer = 0.0f;

    // Control de salto
    bool jumpComplete = false;

    // Agachado
    int crouchFrame = 0;
    float crouchTimer = 0.0f;
    float crouchDelay = 0.05f;
    bool crouching = false;
    bool crouchTransition = false;

    // Aiming up
    int aimingFrame = 0;
    float aimingTimer = 0.0f;
    float aimingDelay = 0.05f;
    bool aimingUp = false;
    bool aimingTransition = true;

    // Shooting up
    int shootUpFrame = 0;
    float shootUpTimer = 0.0f;
    float shootUpDelay = 0.05f;
    int shootUpFrameCount = 10;
    bool shootingUp = false;

    // Caminar agachado
    int crouchWalkFrame = 0;
    float crouchWalkTimer = 0.0f;
    float crouchWalkDelay = 0.08f;
    int crouchWalkFrameCount = 7;
    bool crouchWalking = false;

    // Disparo agachado
    int crouchShootFrame = 0;
    float crouchShootTimer = 0.0f;
    float crouchShootDelay = 0.05f;
    int crouchShootFrameCount = 10;
    bool crouchShooting = false;

    // Offsets
    VisualOffsets idleOffset = { -3.0f, 9.0f };
    VisualOffsets walkOffset = { -4.0f, 15.0f };
    VisualOffsets jumpOffset = { 0.0f, 20.0f };
    VisualOffsets jumpShootOffset = { -6.0f, 20.0f };
};