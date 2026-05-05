#pragma once
#include <raylib.h>

class Boss {
public:
    Boss();
    ~Boss();

    void Init();
    void Update(float playerX);
    void Draw();

    bool IsActive() const { return active; }
    int  GetHealth() const { return health; }

    void TakeDamage() {
        if (active) {
            health--;
            StartFlash();
        }
    }
    void StartFlash() {
        isFlashing = true;
        hitFlashTimer = 0.0f;
        hitFlashCount = 0;
    }
    bool IsFlashing() const { return isFlashing && hitFlashCount % 2 == 0; }

    Rectangle GetHitBox() const {
        return { posX, posY, CANNON_FRAME_W * CANNON_SCALE, CANNON_FRAME_H * CANNON_SCALE };
    }

private:
    // ── Estado general ────────────────────────────────────────
    bool active = false;
    int  health = 10; //cambiar luego a 200

    // ── Posición ──────────────────────────────────────────────
    float posX = 16130.0f;
    float posY = 90.0f;

    // ── Estado del cañón ──────────────────────────────────────
    enum class CannonState {
        MOVING,
        OPENING_DOWN,
        SHOOTING_DOWN,
        CLOSING_DOWN,
        OPENING_UP,
        SHOOTING_UP,
        CLOSING_UP,
    };

    CannonState cannonState = CannonState::MOVING;
    float       stateTimer = 0.0f;
    float       pauseDuration = 0.8f;

    bool  cannonGoingUp = true;
    int   cannonFrame = 0;
    int   openFrame = 0;
    int   closeFrame = 0;

    float cannonFrameTimer = 0.0f;
    float cannonFrameDelay = 0.08f;

    Texture2D cannonSheet = { 0 };

    static constexpr int   CANNON_FRAMES = 19;
    static constexpr float CANNON_FRAME_W = 56.0f;
    static constexpr float CANNON_FRAME_H = 96.0f;
    static constexpr float CANNON_SCALE = 4.0f;

    static constexpr int   OPEN_DOWN_FRAMES = 9;
    static constexpr float OPEN_DOWN_ROW_Y = 1 * 96.0f;
    static constexpr int   SHOOT_DOWN_FRAMES = 8;
    static constexpr float SHOOT_DOWN_ROW_Y = 2 * 96.0f;

    static constexpr int   OPEN_UP_FRAMES = 9;
    static constexpr float OPEN_UP_ROW_Y = 3 * 96.0f;
    static constexpr int   SHOOT_UP_FRAMES = 8;
    static constexpr float SHOOT_UP_ROW_Y = 4 * 96.0f;

    void UpdateCannon(float dt);

    // ── Hit flash ─────────────────────────────────────────────
    float hitFlashTimer = 0.0f;
    static constexpr float HIT_FLASH_DURATION = 0.05f;
    static constexpr int   HIT_FLASH_COUNT = 2;
    int   hitFlashCount = 0;
    bool  isFlashing = false;

    // ── Bolas de plasma ───────────────────────────────────────
    struct PlasmaBall {
        Vector2 pos;
        Vector2 vel;
        bool    active = false;
        float   gravity = 600.0f;
    };

    static constexpr int MAX_PLASMA = 3;
    PlasmaBall plasma[MAX_PLASMA];
    int        shootRepeatCount = 0;
    int        plasmaFired = 0;
    float      plasmaRadius = 8.0f;
    float      plasmaSpread = 50.0f;
    float      capturedPlayerX = 0.0f;

    void FirePlasma();
    void UpdatePlasma(float dt);
    void DrawPlasma() const;

    // ── Destello disparo (sincronizado con frames del ataque) ─
    Texture2D flashSheet = { 0 };
    int       flashFrame = 0;
    bool      flashActive = false;

    static constexpr int   FLASH_FRAMES = 6;
    static constexpr float FLASH_FRAME_W = 56.0f;
    static constexpr float FLASH_FRAME_H = 96.0f;
    static constexpr float FLASH_ROW_Y = 0.0f;
    static constexpr float FLASH_SCALE = 4.0f;

    // ── Fase 2 ────────────────────────────────────────────────
    bool phase2 = false;
    bool phase2Pending = false;

    enum class LaserState {
        MOVING,
        CHARGING,   // fila 0 o 3 — 2 sprites alternando
        FIRING,     // fila 1 o 4 — 10 sprites + laser 1s
    };

    LaserState    laserState = LaserState::MOVING;
    int           laserFrame = 0;
    float         laserFrameTimer = 0.0f;
    float         laserFrameDelay = 0.1f;
    float         laserTimer = 0.0f;
    static constexpr float LASER_DURATION = 1.0f;

    Texture2D laserSheet = { 0 };
    static constexpr float LASER_FRAME_W = 88.0f;
    static constexpr float LASER_FRAME_H = 96.0f;
    static constexpr int   CHARGE_FRAMES = 2;
    static constexpr int   FIRE_FRAMES = 10;

    static constexpr float CHARGE_DOWN_ROW_Y = 0 * 96.0f; 
    static constexpr float FIRE_DOWN_ROW_Y = 1 * 96.0f;  
    static constexpr float CHARGE_UP_ROW_Y = 2 * 96.0f;  
    static constexpr float FIRE_UP_ROW_Y = 3 * 96.0f; 

    static constexpr float CHARGE_DURATION = 2.0f;  // segundos cargando
    float chargeTimer = 0.0f;

    void UpdateLaser(float dt);
    void DrawLaser() const;

};