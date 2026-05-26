#pragma once
#include <raylib.h>

enum class ItemType {
    SHOTGUN,
    BOX,
    PLUSHY,
    FISH,
    MEDAL,
    JETPACK,
    PIG,
    BOMBS,
};

struct ItemConfig {
    const char* texturePath;
    float frameW, frameH, scale;
    int totalFrames;
    float animFps;
    bool collectedByPlayer;
    bool destroyedByBullet;
    float startX, startY;
    bool framesVertical;
    float spacing;
};

static const ItemConfig ITEM_CONFIGS[] = {
    { "Graphics/items/items.png",             32.f, 32.f, 3.0f, 1, 0, true,  false, 96.f, 0.f, false, 0.f }, // SHOTGUN
    { "Graphics/items/ammocratedestoyed.png", 34.f, 44.f, 3.0f, 5, 0, false, true,  0.f,  0.f, false, 0.f }, // BOX
    { "Graphics/items/timmy.png",             20.f, 20.f, 3.0f, 5, 8, true,  false, 0.f,  0.f, false, 0.f }, // PLUSHY
    { "Graphics/items/fish.png",              34.f, 34.f, 3.0f, 6, 8, true,  false, 0.f,  0.f, false, 0.f }, // FISH
    { "Graphics/items/medal.png",             22.f, 22.f, 3.0f, 7, 8, true,  false, 0.f,  0.f, false, 0.f }, // MEDAL
    { "Graphics/items/items.png",             32.f, 32.f, 3.0f, 1, 0, true,  false, 32.f, 0.f, false, 0.f }, // JETPACK
    { "Graphics/items/pig37x21.png",          37.f, 21.f, 3.0f, 18, 8, true,  false, 0.f,  0.f, false, 0.f }, // PIG
    { "Graphics/items/items.png",             32.f, 32.f, 3.0f, 7, 0, true,  false, 0.f,  0.f, false, 0.f }, // BOMBS
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
    bool HasGravity() const { return hasGravity; }
    Vector2 GetPosition() const { return position; }
    void SetPositionY(float y) { position.y = y; }
    void Activate() { activated = true; }
    bool IsActivated() const { return activated; }
    float GetBoxTop() const { return position.y; }
    float GetBoxWidth() const { return BOX_W * BOX_SCALE; }
    float GetVisualHeight() const {
        if (type == ItemType::BOX) return BOX_H * BOX_SCALE;
        return Cfg().frameH * Cfg().scale;
    }
    float GetVisualBottom() const {
        if (type == ItemType::BOX) return position.y + BOX_H * BOX_SCALE;
        return position.y;  // para el resto position.y es la base
    }
    bool ShouldSpawnItem() const { return spawnMachinegun; }

private:
    Vector2 position;
    ItemType type;
    bool isActive = true;

    int currentFrame = 0;
    float animTimer = 0.0f;

    bool isDestroyed = false;
    bool spawnMachinegun = false;
    int destroyFrame = 0;
    float destroyTimer = 0.0f;
    static constexpr float DESTROY_DELAY = 0.1f;
    static constexpr int   DESTROY_FRAMES = 4;
    static constexpr float BOX_W = 34.0f;
    static constexpr float BOX_H = 44.0f;
    static constexpr float BOX_SCALE = 3.0f;

    static Texture2D textures[8];
    static bool      loaded[8];

    bool hasGravity = false;
    float velY = 0.0f;
    bool grounded = false;
    static constexpr float GRAVITY = 800.0f;
    bool activated = false;

    const ItemConfig& Cfg() const { return ITEM_CONFIGS[(int)type]; }
};