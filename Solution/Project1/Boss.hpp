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
    bool IsInIntro() const { return introState != IntroState::DONE; }

    void TakeDamage() {
        if (active || introState != IntroState::DONE) {
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

    void StartIntro() {
        if (introState == IntroState::IDLE) {
            playerInRange = true;
            introState = IntroState::UNVEILING;  // ← salta WAITING directamente
            tentFrame = 0;
            tentFrameTimer = 0.0f;
            preIntroTimer = 0.0f;
        }
    }

private:
    // ── Estado general ────────────────────────────────────────
    bool active = false;
    int  health = 10; //cambiar luego a 200

    // ── Posición ──────────────────────────────────────────────
    float posX = 16090.0f;
    float posY = 98.0f;

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

        static constexpr int TRAIL_LENGTH = 6;
        Vector2 trail[TRAIL_LENGTH];  
        int     trailCount = 0;
        float   trailTimer = 0.0f;
        float   trailDelay = 0.033f;     

        float trailAnimTimer[TRAIL_LENGTH] = {};
        int   trailAnimFrame[TRAIL_LENGTH] = {};
        static constexpr float TRAIL_ANIM_DELAY = 0.05f;
        static constexpr int   TRAIL_ANIM_ROWS = 3;
    };

    static constexpr int MAX_PLASMA = 3;
    PlasmaBall plasma[MAX_PLASMA];
    int        shootRepeatCount = 0;
    int        plasmaFired = 0;
    float      plasmaRadius = 8.0f;
    float      plasmaSpread = 50.0f;
    float      capturedPlayerX = 0.0f;

    Texture2D plasmaSheet = { 0 };
    static constexpr float PLASMA_FRAME_W = 10.0f;
    static constexpr float PLASMA_FRAME_H = 10.0f;
    static constexpr float PLASMA_SCALE = 4.0f;

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
    static constexpr float LASER_FRAME_W = 112.0f;
    static constexpr float LASER_FRAME_H = 96.0f;
    static constexpr int   CHARGE_FRAMES = 2;
    static constexpr int   FIRE_FRAMES = 10;

    static constexpr float CHARGE_DOWN_ROW_Y = 2 * 96.0f; 
    static constexpr float FIRE_DOWN_ROW_Y = 3 * 96.0f;  
    static constexpr float CHARGE_UP_ROW_Y = 0 * 96.0f;  
    static constexpr float FIRE_UP_ROW_Y = 1 * 96.0f; 

    static constexpr float CHARGE_DURATION = 2.0f;  // segundos cargando
    float chargeTimer = 0.0f;

    void UpdateLaser(float dt);
    void DrawLaser() const;
    float laserOffsetX = -223.0f;  // ajusta hasta que quede bien
    float laserOffsetY = 0.0f;
    float laserOffsetDownY = 85.0f;   // offset Y cuando dispara abajo
    float laserOffsetUpY = -50.0f;   // offset Y cuando dispara arriba

    float offsetDownX = 0.0f;
    float offsetDownY = 0.0f;
    float offsetUpX = 0.0f;
    float offsetUpY = -150.0f;

    // ── Flash laser fase 2 ────────────────────────────────────
    Texture2D laserFlashSheet = { 0 };
    int       laserFlashFrame = 0;
    int       laserFlashStep = 0;   // paso en el patrón (0-23)
    float     laserFlashTimer = 0.0f;
    float laserFlashDelay = 0.06f;  // se sobreescribe en Init()
    bool      laserFlashActive = false;

    static constexpr float LASER_FLASH_FRAME_W = 112.0f;
    static constexpr float LASER_FLASH_FRAME_H = 96.0f;
    static constexpr int   LASER_FIRE_FRAMES = 17;  // fila 1

    // Patrón de carga: 0-1-0-1-0-1-0-1-2-1-2-1-2-1-2-3-2-3-2-3-2-3
    static constexpr int CHARGE_PATTERN_LENGTH = 22;
    static constexpr int CHARGE_PATTERN[22] = {
        0,1,0,1,0,1,0,1,
        2,1,2,1,2,1,2,
        3,2,3,2,3,2,3
    };

    void UpdateLaserFlash(float dt);
    void DrawLaserFlash() const;

    // ── Manta (intro) ─────────────────────────────────────────
    enum class IntroState {
        IDLE,       // esperando que el jugador llegue a X
        WAITING,    // jugador llegó, manta visible, esperando 0.5s
        UNVEILING,  // animación de manta quitándose (15 frames)
        PAUSE,      // pausa antes de que el cañón empiece
        DONE,       // intro terminada, boss activo
    };

    IntroState introState = IntroState::IDLE;
    float      introTimer = 0.0f;
    int        tentFrame = 0;
    float      tentFrameTimer = 0.0f;
    float      tentFrameDelay = 0.10f;
    float tentOffsetX = 0.0f;

    Texture2D  tentSheet = { 0 };
    static constexpr float TENT_FRAME_W = 120.0f;
    static constexpr float TENT_FRAME_H = 104.0f;
    static constexpr int   TENT_FRAMES = 15;
    static constexpr float TENT_SCALE = 4.0f;
    static constexpr float TENT_WAIT = 0.5f;   // espera antes de animación
    static constexpr float TENT_PAUSE = 0.3f;   // pausa antes de activar cañón
    static constexpr float TENT_ACTIVATE_X = 15500.0f;

    void UpdateIntro(float playerX, float dt);
    void DrawTent() const;

    float preIntroTimer = 0.0f;
    static constexpr float PRE_INTRO_DELAY = 5.0f;
    bool playerInRange = false;

    Texture2D laserBeamSheet = { 0 };
    int       laserBeamFrame = 0;
    float     laserBeamTimer = 0.0f;
    float     laserBeamDelay = 0.06f;
    static constexpr float LASER_BEAM_W = 80.0f;
    static constexpr float LASER_BEAM_H = 26.0f;
    static constexpr int   LASER_BEAM_FRAMES = 4;
    float laserBeamX = 0.0f;
    float laserBeamSpeed = 800.0f;  // píxeles/segundo — ajusta
    bool  laserBeamActive = false;

    // ── Beam laser arriba ─────────────────────────────────────
    Texture2D beamSheet = { 0 };
    int       beamFrame = 0;
    float     beamTimer = 0.0f;
    float     beamDelay = 0.1f;
    bool      beamRetracting = false;  // false=disparando, true=recogiendo
    float flashLoopTimer = 0.0f;

    static constexpr float BEAM_W = 16.0f;
    static constexpr float BEAM_H = 32.0f;
    static constexpr int   BEAM_FRAMES = 2;   // fila 0: loop 0-1
    static constexpr int   BEAM_RET_FRAMES = 8; // fila 1: sprites 0-7
    float laserFlashRowY = 0.0f;

};