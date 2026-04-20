#pragma once
#include <raylib.h>

enum class ItemType {
    SHOTGUN,
};

class Item {
public:
    Item(Vector2 position, ItemType type);
    ~Item();

    void Draw();

    Rectangle GetHitBox() const;
    bool IsActive() const { return isActive; }
    ItemType GetType() const { return type; }
    void Collect();

private:
    Vector2 position;
    ItemType type;
    bool isActive = true;

    static Texture2D texture;
    static bool textureLoaded;
};