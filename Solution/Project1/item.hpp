#pragma once
#include <raylib.h>

// Item types (merged)
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
    float frameW;
    float frameH;
    float scale;
    int totalFrames;
    float animFps;
    bool collectedByPlayer;
    bool destroyedByBullet;
    float startX;
    float startY;
    bool framesVertical;
    float spacing;
};

// Configs for all item types (add or tweak paths as needed)
static const ItemConfig ITEM_CONFIGS[] = {
    { "Graphics/items.png",             32.f, 32.f, 3.0f, 1, 0,  true,  false, 96.f, 0.f, false, 0.f }, // SHOTGUN
    { "Graphics/ammocratedestoyed.png", 34.f, 44.f, 3.0f, 5, 0,  false, true,  0.f,  0.f, false, 0.f }, // BOX
    { "Graphics/timmy.png",             20.f, 20.f, 3.0f, 5, 8,  true,  false, 0.f,  0.f, false, 0.f }, // PLUSHY
    { "Graphics/fish.png",              34.f, 34.f, 3.0f, 6, 8,  true,  false, 0.f,  0.f, false, 0.f }, // FISH
    { "Graphics/medal.png",             22.f, 22.f, 3.0f, 7, 8,  true,  false, 0.f,  0.f, false, 0.f }, // MEDAL
    { "Graphics/items.png",             32.f, 32.f, 3.0f, 1, 0,  true,  false, 32.f,  0.f, false, 0.f }, // JETPACK
	{ "Graphics/pig.png",               32.f, 32.f, 3.0f, 4, 8,  true,  false, 0.f,  0.f, false, 0.f }, // PIG
	{ "Graphics/items.png",             32.f, 32.f, 3.0f, 7, 0,  true,  false, 0.f,  0.f, false, 0.f }, // BOMBS

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

    // Collection / destruction
    void Collect();
    void Destroy();
    bool IsDestroyed() const { return isDestroyed; }
    bool DestructionFinished() const { return isDestroyed && destroyFrame >= DESTROY_FRAMES - 1; }

    // Spawn helpers (compatibility: some systems expect "spawn item", others "spawn machinegun")
    bool ShouldSpawnItem() const { return spawnItem; }
    void ConsumeSpawn() { spawnItem = false; }
    bool ShouldSpawnMachinegun() const { return spawnMachinegun; }
    void ConsumeSpawnMachinegun() { spawnMachinegun = false; }

    // Physics helpers
    void SetGravity(bool val) { hasGravity = val; }
    void SetGrounded(bool val) { grounded = val; }
    bool IsGrounded() const { return grounded; }

    // Position helpers
    Vector2 GetPosition() const { return position; }
    void SetPosition(Vector2 pos) { position = pos; }
    void SetPositionY(float y) { position.y = y; }

    // Editor / activation helpers
    void Activate() { activated = true; }
    bool IsActivated() const { return activated; }
    float GetBoxTop() const { return position.y; }
    float GetBoxWidth() const { return BOX_W * BOX_SCALE; }
    bool HasGravity() const { return hasGravity; }
    float GetVisualHeight() const {
        if (type == ItemType::BOX) return BOX_H * BOX_SCALE;
        return 22.0f * 3.0f;  // machinegun y jetpack
    }



private:
    Vector2 position;
    ItemType type;
    bool isActive = true;

    // Animation state
    int currentFrame = 0;
    float animTimer = 0.0f;

    // Destruction (BOX)
    bool isDestroyed = false;
    bool spawnItem = false;         // used by SystemCollision (legacy)
    bool spawnMachinegun = false;   // used by some code paths (compat)
    int destroyFrame = 0;
    float destroyTimer = 0.0f;
    static constexpr float DESTROY_DELAY = 0.1f;
    static constexpr int DESTROY_FRAMES = 4;

    // BOX sprite constants (kept for drawing/hitbox)
    static constexpr float BOX_W = 34.0f;
    static constexpr float BOX_H = 44.0f;
    static constexpr float BOX_SCALE = 3.0f;

    // Shared textures (one per config)
    static Texture2D textures[8];
    static bool loaded[8];

    // Physics (for spawned items)
    bool hasGravity = false;
    float velY = 0.0f;
    bool grounded = false;
    static constexpr float GRAVITY = 800.0f;

    // Editor state
    bool activated = false;

    const ItemConfig& Cfg() const { return ITEM_CONFIGS[(int)type]; }
};