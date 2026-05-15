#pragma once
#include <raylib.h>

enum class PrisonerType { GROUND, POLE };

enum class PrisonerState {
    IDLE,
    FREEING,      // animación de liberación
    WALKING,      // camina hacia el player
    REWARD,       // da el premio (pingpong)
    SALUTE,       // saludo militar
    RUNNING,      // corre hacia la izquierda
    DONE          // fuera de cámara, eliminar
};

class Prisoner {
public:
    Prisoner(Vector2 position, PrisonerType type, bool flipped = false);
    ~Prisoner();

    void Update(float playerX, float cameraLeft, float cameraRight);
    void Draw() const;
    void TakeDamage();
    bool IsActive() const { return active; }
    Rectangle GetHitBox() const;
    bool IsFreed() const { return state != PrisonerState::IDLE; }

    void SetGrounded(bool val) { grounded = val; }
    void SetPositionY(float y) { position.y = y; }
    float GetFeetY() const { return position.y + GetHitBox().height; }
    bool IsGrounded() const { return grounded; }
    Rectangle GetCollisionHitBox() const;  // para bloques y gravedad
    Rectangle GetSpriteSize() const;
    PrisonerType GetType() const { return type; }
    Vector2 GetPosition() const { return position; }
    bool IsFlippedDefault() const { return flippedDefault; }

private:
    Vector2     position;
    PrisonerType type;
    PrisonerState state = PrisonerState::IDLE;
    bool        active = true;
    bool        facingRight = true;

    int   frame = 0;
    float timer = 0.0f;
    bool  pingpongForward = true;

    // Reward pingpong
    bool  rewardGiven = false;


    static constexpr float IDLE_FRAME_DELAY = 0.12f;  // idle más lento
    static constexpr float WALK_FRAME_DELAY = 0.06f;  // walk más rápido
    static constexpr float WALK_SPEED = 80.0f;
    static constexpr float RUN_SPEED = 350.0f;
    static constexpr float FRAME_DELAY = 0.08f;
    float scale = 4.0f;

    // Sprite rows y tamaños
    // GROUND idle:    fila 0, 43x35, 5 frames pingpong
    // GROUND free:    fila 0, 43x35, frames 5-8
    // POLE idle:      fila 1, 44x48, 9 frames pingpong
    // POLE free:      fila 2, 46x48, 10 frames
    // WALK:           fila 4, 33x41, 12 frames loop
    // REWARD:         fila 5, 44x41, 11 frames pingpong
    // SALUTE:         fila 6, 44x41, 14 frames
    // RUN:            fila 7, 44x41, 8 frames loop

    static Texture2D texture;
    static bool      textureLoaded;

    void UpdateIdle(float dt);
    void UpdateFreeing(float dt);
    void UpdateWalking(float playerX, float dt);
    void UpdateReward(float playerX, float dt);
    void UpdateSalute(float dt);
    void UpdateRunning(float cameraLeft, float dt);

    void DrawFrame(float srcX, float srcY, float srcW, float srcH) const;
    void SpawnRewardItem();

    float walkTarget = 0.0f;
    bool walkInitialized = false;
    static constexpr float WALK_RANGE = 150.0f;  // ajusta

    float velY = 0.0f;
    bool grounded = false;
    static constexpr float GRAVITY = 800.0f;

    bool  poleBreaking = false;
    int   poleBreakFrame = 3;  // empieza desde el frame 3
    float poleBreakTimer = 0.0f;
    Vector2 poleBreakPos = { 0.0f, 0.0f };  // posición fija donde estaba el poste
    bool flippedDefault = false;
};