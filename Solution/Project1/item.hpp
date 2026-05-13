#pragma once
#include <raylib.h>

enum class ItemType {
    SHOTGUN,
    BOX,
<<<<<<< HEAD
    PLUSHY,
    FISH,
    MEDAL,
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

static const ItemConfig ITEM_CONFIGS[] = {
    { "Graphics/items.png",             32.f, 32.f, 3.0f, 1, 0,  true,  false, 96.f, 0.f, false, 0.f }, // SHOTGUN
    { "Graphics/ammocratedestoyed.png", 34.f, 44.f, 3.0f, 5, 0,  false, true,  0.f,  0.f, false, 0.f }, // BOX
    { "Graphics/timmy.png",             20.f, 20.f, 3.0f, 5, 8,  true,  false, 0.f,  0.f, false, 0.f }, // PLUSHY
    { "Graphics/fish.png",              34.f, 34.f, 3.0f, 6, 8,  true,  false, 0.f,  0.f, false, 0.f }, // FISH
    { "Graphics/medal.png",              22.f, 22.f, 3.0f, 7, 8,  true,  false, 0.f,  0.f, false, 0.f }, // MEDAL
=======
    JETPACK,
>>>>>>> main
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
    bool HasGravity() const { return hasGravity; }

    void Collect();
    void Destroy();

    bool ShouldSpawnItem() const { return spawnItem; }
    void ConsumeSpawn() { spawnItem = false; }
    Vector2 GetSpawnPosition() const { return { position.x + 60.f, position.y }; }

    void SetGravity(bool val) { hasGravity = val; }
    void SetGrounded(bool val) { isGrounded = val; }
    bool GetGrounded() const { return isGrounded; }
    Vector2 GetPosition() const { return position; }
    void SetPosition(Vector2 pos) { position = pos; }
    void SetPositionY(float y) { position.y = y; }
<<<<<<< HEAD
    bool IsGrounded() const { return isGrounded; }
=======
    void Activate() { activated = true; }
    bool IsActivated() const { return activated; }
    float GetBoxTop() const { return position.y; }
    float GetBoxWidth() const { return BOX_W * BOX_SCALE; }
>>>>>>> main

private:
    Vector2 position;
    ItemType type;
    bool isActive = true;

    int currentFrame = 0;
    float animTimer = 0.0f;

    bool isDestroyed = false;
    bool spawnItem = false;
    int destroyFrame = 0;
    float destroyTimer = 0.0f;

    static constexpr float DESTROY_DELAY = 0.1f;
    static constexpr int DESTROY_FRAMES = 4;
    static constexpr float GRAVITY = 800.0f;
    bool activated = false;

    bool hasGravity = false;
    float velY = 0.0f;
    bool isGrounded = false;

    static Texture2D textures[5];
    static bool loaded[5];

    const ItemConfig& Cfg() const { return ITEM_CONFIGS[(int)type]; }
};