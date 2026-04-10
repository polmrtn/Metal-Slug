#pragma once
#include "raylib.h"

// Estados de animación para piernas y torso (pueden ir por separado)
enum class LegsAnim { IDLE, WALKING, JUMPING };
enum class TorsoAnim { IDLE, WALKING, JUMPING, SHOOTING };

// Offsets visuales SOLO para piernas (el torso va en posición base)
struct VisualOffsets { float legsX, legsY; };

class PlayerAnim {
public:
    PlayerAnim();
    ~PlayerAnim();
    
    void LoadTextures();
    void UnloadTextures();
    
    // Actualizar todas las animaciones con el estado actual del jugador
    void Update(bool grounded, float velX, bool crouching, bool aimingUp, float dt);
    
    // Getters principales
    Texture2D GetSheet() const { return spriteSheet; }
    VisualOffsets GetOffsets() const;
    LegsAnim GetLegsAnim() const { return legsAnim; }
    TorsoAnim GetTorsoAnim() const { return torsoAnim; }
    bool IsShooting() const { return shooting; }
    
    // Frames actuales de cada animación
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
    
    // Control de disparo y salto
    void StartShoot();
    void ResetJump();

private:
    Texture2D spriteSheet;
    
    LegsAnim legsAnim = LegsAnim::IDLE;
    TorsoAnim torsoAnim = TorsoAnim::IDLE;
    bool shooting = false;
    
    int idleFrame = 0;
    int walkLegsFrame = 0, walkTorsoFrame = 0;
    int jumpLegsFrame = 0, jumpTorsoFrame = 0;
    int shootFrame = 0;
    
    float idleTimer = 0, walkLegsTimer = 0, walkTorsoTimer = 0;
    float jumpLegsTimer = 0, jumpTorsoTimer = 0, shootTimer = 0;
    
    bool jumpComplete = false;
    
    VisualOffsets idleOffset = { -3, 9 };
    VisualOffsets walkOffset = { -4, 15 };
    VisualOffsets jumpOffset = { 0, 20 };
    VisualOffsets jumpShootOffset = { -6, 20 };
};