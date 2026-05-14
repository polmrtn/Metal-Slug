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

    bool IsMachinegunCrouching() const { return machinegunCrouching; }
    bool IsMachinegunCrouchTransition() const { return machinegunCrouchTransition; }
    int GetMachinegunCrouchFrame() const { return machinegunCrouchFrame; }
    float GetRowMachinegunCrouchTransition() const { return 49 * 34.0f; }
    float GetRowMachinegunCrouchIdle() const { return 51 * 34.0f; }
    float GetMachinegunCrouchTransitionH() const { return 68.0f; }
    float GetMachinegunCrouchIdleH() const { return 34.0f; }
    void StartMachinegunCrouch();
    void StopMachinegunCrouch();

    // Crouch Walk
    bool IsMachinegunCrouchWalking() const { return machinegunCrouchWalking; }
    int GetMachinegunCrouchWalkFrame() const { return machinegunCrouchWalkFrame; }
    float GetRowMachinegunCrouchWalk() const { return 52 * 34.0f; }

    // Crouch Throw
    bool IsMachinegunCrouchThrowing() const { return machinegunCrouchThrowing; }
    int GetMachinegunCrouchThrowFrame() const { return machinegunCrouchThrowFrame; }
    float GetRowMachinegunCrouchThrow() const { return 53 * 34.0f; }
    float GetMachinegunCrouchThrowH() const { return 68.0f; }
    void StartMachinegunCrouchThrow();

    // Crouch Shoot
    bool IsMachinegunCrouchShooting() const { return machinegunCrouchShooting; }
    int GetMachinegunCrouchShootFrame() const { return machinegunCrouchShootFrame; }
    float GetRowMachinegunCrouchShoot() const { return 55 * 34.0f; }
    void StartMachinegunCrouchShoot();

    bool IsCrouchThrowing() const { return crouchThrowing; }
    int GetCrouchThrowFrame() const { return crouchThrowFrame; }
    float GetCrouchThrowRowY() const { return 24 * 34.0f; }
    float GetCrouchThrowW() const { return 68.0f; }
    void StartCrouchThrow();

    void LoadParachute();
    void UnloadParachute();
    void UpdateParachute(float dt);
    void DrawParachute(Vector2 playerPos, float scale, bool facingLeft);
    bool IsParachuteActive() const { return parachuteActive; }
    void StartParachute() { parachuteActive = true; parachuteFrame = 0; parachuteTimer = 0.0f; }
    void StopParachute() { parachuteActive = false; parachuteFrame = 0; }

    void LoadParachute2();
    void UnloadParachute2();
    void UpdateParachuteLanding(float dt);
    void DrawParachuteLanding(Vector2 playerPos, float scale, bool facingLeft);
    void StartParachuteLanding();
    void StopParachuteLanding();
    bool IsParachuteLanding() const { return parachuteLanding; }
    bool IsParachuteLandingFinished() const { return parachuteLandingFrame >= parachuteLandingFrameCount - 1; }

    // Melee
    void StartMelee();
    bool IsMeleeAttacking() const { return meleeAttacking; }
    int  GetMeleeFrame() const { return meleeFrame; }
    // pistola pie: fila 58-59, machinegun pie: 62-63
    // pistola agachado: 60-61, machinegun agachado: 64-65
    float GetMeleeRowY(bool machinegun, bool crouching) const {
        if (!machinegun && !crouching) return 58 * 34.0f;  // fila 58, cubre 58+59
        if (!machinegun && crouching) return 60 * 34.0f;  // fila 60, cubre 60+61
        if (machinegun && !crouching) return 62 * 34.0f;  // fila 62, cubre 62+63
        return 64 * 34.0f;                                  // fila 64, cubre 64+65
    }

    void StartP1Anim(int loops = 8);
    void UpdateP1Anim(float dt);
    void DrawP1Anim(Vector2 playerPos, float scale, bool facingLeft) const;
    bool IsP1AnimActive() const { return p1AnimActive; }

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
    float walkLegsDelay = 0.025f;
    float walkTorsoDelay = 0.025f;

    // Agachado
    int crouchFrame = 0;
    float crouchTimer = 0.0f;
    float crouchTransitionDelay = 0.025f;
    float crouchIdleDelay = 0.075f;
    bool crouching = false;
    bool crouchTransition = false;

    // Caminar agachado
    int crouchWalkFrame = 0;
    float crouchWalkTimer = 0.0f;
    float crouchWalkDelay = 0.04f;
    int crouchWalkFrameCount = 7;
    bool crouchWalking = false;

    // Disparo agachado
    int crouchShootFrame = 0;
    float crouchShootTimer = 0.0f;
    float crouchShootDelay = 0.025f;
    int crouchShootFrameCount = 10;
    bool crouchShooting = false;

    // Pistol Crouch Throw
    int crouchThrowFrame = 0;
    float crouchThrowTimer = 0.0f;
    float crouchThrowDelay = 0.025f;
    float crouchThrowEndDelay = 0.075f;  // delay más largo para los últimos 2 frames
    int crouchThrowFrameCount = 6;
    bool crouchThrowing = false;

    // Aiming up
    int aimingFrame = 0;
    float aimingTimer = 0.0f;
    float aimingDelay = 0.05f;
    bool aimingUp = false;
    bool aimingTransition = true;

    // Shooting up
    int shootUpFrame = 0;
    float shootUpTimer = 0.0f;
    float shootUpDelay = 0.025f;
    int shootUpFrameCount = 10;
    bool shootingUp = false;

    // Animación de lanzar granada
    int throwFrame = 0;
    float throwTimer = 0.0f;
    float throwDelay = 0.025f;
    int throwFrameCount = 6;
    bool isThrowing = false;

    // Machinegun
    int machinegunIdleFrame = 0;
    int machinegunShootFrame = 0;
    float machinegunIdleTimer = 0.0f;
    float machinegunShootTimer = 0.0f;
    float machinegunIdleDelay = 0.05f;
    float machinegunShootDelay = 0.025f;
    int machinegunIdleFrameCount = 4;
    int machinegunShootFrameCount = 4;
    bool machinegunIdle = false;
    bool machinegunShooting = false;
    float machinegunShootCooldown = 0.0f;
    float machinegunShootCooldownMax = 0.15f;

    // Machinegun Throw
    int machinegunThrowFrame = 0;
    float machinegunThrowTimer = 0.0f;
    float machinegunThrowDelay = 0.025f;
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
    float machinegunShootUpDelay = 0.025f;
    int machinegunShootUpFrameCount = 4;
    bool machinegunShootingUp = false;

    // Machinegun Crouch
    bool machinegunCrouching = false;
    bool machinegunCrouchTransition = false;
    int machinegunCrouchFrame = 0;
    float machinegunCrouchTimer = 0.0f;
    float machinegunCrouchTransitionDelay = 0.025f;
    float machinegunCrouchIdleDelay = 0.075f;
    int machinegunCrouchTransitionFrameCount = 3;
    int machinegunCrouchIdleFrameCount = 4;

    // Machinegun Crouch Walk
    int machinegunCrouchWalkFrame = 0;
    float machinegunCrouchWalkTimer = 0.0f;
    float machinegunCrouchWalkDelay = 0.04f;
    int machinegunCrouchWalkFrameCount = 7;
    bool machinegunCrouchWalking = false;

    // Machinegun Crouch Throw
    int machinegunCrouchThrowFrame = 0;
    float machinegunCrouchThrowTimer = 0.0f;
    float machinegunCrouchThrowDelay = 0.025f;
    int machinegunCrouchThrowFrameCount = 6;
    bool machinegunCrouchThrowing = false;
    float machinegunCrouchThrowCooldown = 0.0f;
    float machinegunCrouchThrowCooldownMax = 0.25f;
    float machinegunCrouchThrowEndDelay = 0.075f;

    // Machinegun Crouch Shoot
    int machinegunCrouchShootFrame = 0;
    float machinegunCrouchShootTimer = 0.0f;
    float machinegunCrouchShootDelay = 0.025f;
    int machinegunCrouchShootFrameCount = 4;
    bool machinegunCrouchShooting = false;

    Texture2D parachuteSheet{ 0 };

    // Parachute
    bool parachuteActive = false;
    int  parachuteFrame = 0;
    float parachuteTimer = 0.0f;
    const float parachuteDelay = 0.12f;   // velocidad animación, ajusta a gusto
    const int   parachuteFrameCount = 5;
    const float PARACHUTE_W = 52.0f;
    const float PARACHUTE_H = 43.0f;

    Texture2D parachuteSheet2{ 0 };

    // Parachute landing
    bool parachuteLanding = false;
    int  parachuteLandingFrame = 0;
    float parachuteLandingTimer = 0.0f;
    const float parachuteLandingDelay = 0.08f;
    const int   parachuteLandingFrameCount = 17;
    const float PARACHUTE2_W = 62.0f;
    const float PARACHUTE2_H = 59.0f;
    Vector2 landingPosition;  // Posición fija donde aterrizó
    bool hasLandingPosition = false;  // Si ya se guardó la posición

    // Offsets
    VisualOffsets idleOffset = { 1.0f, 9.0f, 3.0f, 0.0f };
    VisualOffsets walkOffset = { -1.0f, 15.0f, 3.0f, 0.0f };
    VisualOffsets jumpOffset = { 0.0f, 20.0f, 0.0f, 0.0f };
    VisualOffsets jumpShootOffset = { -6.0f, 20.0f, 0.0f, 0.0f };

    // Melee
    bool  meleeAttacking = false;
    int   meleeFrame = 0;
    float meleeTimer = 0.0f;
    float meleeFrameDelay = 0.05f;
    static constexpr int MELEE_FRAMES = 6;

    Texture2D texP1Anim;
    int   p1AnimFrame = 0;
    float p1AnimTimer = 0.0f;
    float p1AnimDelay = 0.08f;
    static constexpr int   P1_ANIM_FRAMES = 7;
    static constexpr float P1_ANIM_W = 30.0f;
    static constexpr float P1_ANIM_H = 31.0f;
    int   p1AnimLoopCount = 0;
    bool  p1AnimActive = false;
    int p1AnimMaxLoops = 5;
};