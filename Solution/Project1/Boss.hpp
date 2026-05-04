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
    int  health = 200;

    // ── Posición ──────────────────────────────────────────────
    float posX = 16130.0f;
    float posY = 90.0f;

    // ── Estado del cañón ──────────────────────────────────────
    enum class CannonState {
        MOVING,
        OPENING_DOWN,    // fila 1 adelante
        SHOOTING_DOWN,   // fila 2
        CLOSING_DOWN,    // fila 1 atrás
        OPENING_UP,      // fila 3 adelante
        SHOOTING_UP,     // fila 4
        CLOSING_UP,      // fila 3 atrás
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

    // ── Constantes spritesheet ────────────────────────────────
    static constexpr int   CANNON_FRAMES = 19;
    static constexpr float CANNON_FRAME_W = 56.0f;
    static constexpr float CANNON_FRAME_H = 96.0f;
    static constexpr float CANNON_SCALE = 4.0f;

    static constexpr int   OPEN_DOWN_FRAMES = 9;   // fila 1
    static constexpr float OPEN_DOWN_ROW_Y = 1 * 96.0f;
    static constexpr int   SHOOT_DOWN_FRAMES = 8;   // fila 2
    static constexpr float SHOOT_DOWN_ROW_Y = 2 * 96.0f;

    static constexpr int   OPEN_UP_FRAMES = 9;   // fila 3
    static constexpr float OPEN_UP_ROW_Y = 3 * 96.0f;
    static constexpr int   SHOOT_UP_FRAMES = 8;   // fila 4
    static constexpr float SHOOT_UP_ROW_Y = 4 * 96.0f;

    // ── Helpers ───────────────────────────────────────────────
    void UpdateCannon(float dt);

    float hitFlashTimer = 0.0f;
    static constexpr float HIT_FLASH_DURATION = 0.08f;
    static constexpr int   HIT_FLASH_COUNT = 6;  // número de parpadeos
    int   hitFlashCount = 0;
    bool  isFlashing = false;
};