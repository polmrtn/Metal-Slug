#pragma once
#include "raylib.h"

// Estados de animación para piernas y torso
enum class LegsAnim { IDLE, WALKING, JUMPING };
enum class TorsoAnim { IDLE, WALKING, JUMPING, SHOOTING, THROWING, MACHINEGUN_IDLE, MACHINEGUN_SHOOTING, MACHINEGUN_THROWING, MACHINEGUN_AIMING_TRANSITION };

// Offsets visuales SOLO para piernas
struct VisualOffsets { float legsX, legsY, torsoX, torsoY; };

class PlayerAnim {
public:
    PlayerAnim();
    ~PlayerAnim();

    void LoadTextures();
    void UnloadTextures();

    // Actualizar todas las animaciones
    void Update(bool grounded, float velX, bool crouchingInput, bool aimingUpInput, bool hasMachinegun, float dt);

    // Getters principales
    Texture2D GetSheet() const { return spriteSheet; }
    VisualOffsets GetOffsets() const;
    LegsAnim GetLegsAnim() const { return legsAnim; }
    TorsoAnim GetTorsoAnim() const { return torsoAnim; }
    bool IsShooting() const { return shooting; }
    bool IsMachinegunIdle() const { return machinegunIdle; }
    bool IsMachinegunShooting() const { return machinegunShooting; }

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

    // Getters para caminar agachado
    int GetCrouchWalkFrame() const { return crouchWalkFrame; }
    bool IsCrouchWalking() const { return crouchWalking; }
    float GetCrouchWalkRowY() const { return 21 * 34.0f; }

    // Getters para disparo agachado
    int GetCrouchShootFrame() const { return crouchShootFrame; }
    bool IsCrouchShooting() const { return crouchShooting; }
    float GetCrouchShootRowY() const { return 22 * 34.0f; }
    float GetCrouchShootW() const { return 68.0f; }

    // Getters para aiming up
    int GetAimingFrame() const { return aimingFrame; }
    bool IsAimingUp() const { return aimingUp; }
    bool IsAimingTransition() const { return aimingTransition; }
    float GetAimingTransitionRowY() const { return 14 * 34.0f; }
    float GetAimingIdleRowY() const { return 15 * 34.0f; }
    float GetRowMachinegunAimingIdle() const { return 40 * 34.0f; }
    float GetMachinegunAimingIdleH() const { return 68.0f; }
    bool IsMachinegunAimingTransition() const { return machinegunAimingTransition; }

    void StartThrow();
    bool IsThrowing() const { return isThrowing; }
    int GetThrowFrame() const { return throwFrame; }
    float GetThrowRowY() const { return 13 * 34.0f; }

    // Getters para shooting up
    int GetShootUpFrame() const { return shootUpFrame; }
    bool IsShootingUp() const { return shootingUp; }
    float GetShootUpRowY() const { return 16 * 34.0f; }
    float GetShootUpH() const { return 68.0f; }

    // Control de animaciones
    void StartShoot();
    void StartShootUp();
    void StartCrouchShoot();
    void ResetJump();
    void ForceCrouch();
    void ForceStopShoot();

    // Machinegun
    void StartMachinegunIdle();
    void StartMachinegunShoot();
    void StopMachinegun();
    int GetMachinegunIdleFrame() const { return machinegunIdleFrame; }
    int GetMachinegunShootFrame() const { return machinegunShootFrame; }
    float GetRowMachinegunIdle() const { return 35 * 34.0f; }
    float GetRowMachinegunShoot() const { return 36 * 34.0f; }

    // Machinegun Throw
    void StartMachinegunThrow();
    bool IsMachinegunThrowing() const { return machinegunThrowing; }
    int GetMachinegunThrowFrame() const { return machinegunThrowFrame; }
    float GetRowMachinegunThrow() const { return 37 * 34.0f; }

    // Machinegun Aiming
    void StartMachinegunAiming();
    void StopMachinegunAiming();
    bool IsMachinegunAiming() const { return machinegunAimingUp; }
    int GetMachinegunAimingFrame() const { return machinegunAimingFrame; }
    float GetRowMachinegunAiming() const { return 39 * 34.0f; }

    int GetMachinegunShootUpFrame() const { return machinegunShootUpFrame; }
    bool IsMachinegunShootingUp() const { return machinegunShootingUp; }
    float GetMachinegunShootUpRowY() const { return 42 * 34.0f; }
    float GetMachinegunShootUpH() const { return 102.0f; }
    void StartMachinegunShootUp();

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

    // Velocidades de animación walking
    float walkLegsDelay = 0.05f;
    float walkTorsoDelay = 0.05f;

    // Agachado
    int crouchFrame = 0;
    float crouchTimer = 0.0f;
    float crouchTransitionDelay = 0.05f;
    float crouchIdleDelay = 0.15f;
    bool crouching = false;
    bool crouchTransition = false;

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

    // Animación de lanzar granada
    int throwFrame = 0;
    float throwTimer = 0.0f;
    float throwDelay = 0.05f;
    int throwFrameCount = 6;
    bool isThrowing = false;

    // Machinegun
    int machinegunIdleFrame = 0;
    int machinegunShootFrame = 0;
    float machinegunIdleTimer = 0.0f;
    float machinegunShootTimer = 0.0f;
    float machinegunIdleDelay = 0.1f;
    float machinegunShootDelay = 0.05f;
    int machinegunIdleFrameCount = 4;
    int machinegunShootFrameCount = 4;
    bool machinegunIdle = false;
    bool machinegunShooting = false;
    float machinegunShootCooldown = 0.0f;
    float machinegunShootCooldownMax = 0.15f;

    // Machinegun Throw
    int machinegunThrowFrame = 0;
    float machinegunThrowTimer = 0.0f;
    float machinegunThrowDelay = 0.05f;
    int machinegunThrowFrameCount = 6;
    bool machinegunThrowing = false;
    float machinegunThrowCooldown = 0.0f;
    float machinegunThrowCooldownMax = 0.2f;

    // Machinegun Aiming (mismo patrón que la pistola)
    bool machinegunAimingUp = false;
    bool machinegunAimingTransition = true;
    int machinegunAimingFrame = 0;
    float machinegunAimingTimer = 0.0f;
    float machinegunAimingDelay = 0.05f;

    // Machinegun Shooting Up
    int machinegunShootUpFrame = 0;
    float machinegunShootUpTimer = 0.0f;
    float machinegunShootUpDelay = 0.05f;
    int machinegunShootUpFrameCount = 4;
    bool machinegunShootingUp = false;

    // Offsets
    VisualOffsets idleOffset = { 1.0f, 9.0f, 3.0f, 0.0f };
    VisualOffsets walkOffset = { -1.0f, 15.0f, 3.0f, 0.0f };
    VisualOffsets jumpOffset = { 0.0f, 20.0f, 0.0f, 0.0f };
    VisualOffsets jumpShootOffset = { -6.0f, 20.0f, 0.0f, 0.0f };
};