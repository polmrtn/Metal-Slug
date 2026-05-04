#pragma once
#include <raylib.h>

enum class ItemType {
    SHOTGUN,
    BOX,
};

class Item {
public:
    Item(Vector2 position, ItemType type);
    ~Item();

    void Update();
    void Draw();

    Rectangle GetHitBox() const;
    bool IsActive() const { return isActive; }
    ItemType GetType() const { return type; }
    void Collect();
    void Destroy();
    bool IsDestroyed() const { return isDestroyed; }
    bool DestructionFinished() const { return isDestroyed && destroyFrame >= DESTROY_FRAMES - 1; }
    bool ShouldSpawnMachinegun() const { return spawnMachinegun; }
    void ConsumeSpawn() { spawnMachinegun = false; }
    void SetGravity(bool val) { hasGravity = val; }
    void SetGrounded(bool val) { grounded = val; }
    bool IsGrounded() const { return grounded; }
    Vector2 GetPosition() const { return position; }
    void SetPositionY(float y) { position.y = y; }

private:
    Vector2 position;
    ItemType type;
    bool isActive = true;

    // Destrucción de caja
    bool isDestroyed = false;
    bool spawnMachinegun = false;
    int destroyFrame = 0;
    float destroyTimer = 0.0f;
    static constexpr float DESTROY_DELAY = 0.1f;
    static constexpr int DESTROY_FRAMES = 4;  // 4 frames de destrucción (sin contar el idle)
    static constexpr float BOX_W = 34.0f;
    static constexpr float BOX_H = 44.0f;
    static constexpr float BOX_SCALE = 3.0f;

    static Texture2D texture;
    static bool textureLoaded;
    static Texture2D boxTexture;
    static bool boxTextureLoaded;

    // Física (solo SHOTGUN spawneado)
    bool hasGravity = false;
    float velY = 0.0f;
    bool grounded = false;
    static constexpr float GRAVITY = 800.0f;

};