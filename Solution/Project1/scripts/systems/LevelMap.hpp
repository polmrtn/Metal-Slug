#pragma once
#include <raylib.h>
#include <vector>

constexpr float TILE_SIZE = 30.0f;

enum class TileType {
    SOLID,
    PLATFORM,
    CEILING,
    RAMP_UP,
};

struct CollisionRect {
    Rectangle rect;
    TileType  type;

    float GetRampSurfaceY(float worldX) const {
        float t = (worldX - rect.x) / rect.width;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        return rect.y + rect.height - t * rect.height;
    }
};

class TileMap {
public:
    TileMap() = default;

    // Añade tile en posición mundial exacta (snapped al grid con offset)
    void AddTileWorld(float worldX, float worldY, TileType type);
    // Elimina tile en posición mundial
    void RemoveTileWorld(float worldX, float worldY);
    void Clear();
    void Bake();

    const std::vector<CollisionRect>& GetColliders() const { return colliders; }

    void SaveToFile(const char* filename) const;
    void LoadFromFile(const char* filename);

    void DrawTiles() const;
    void DrawColliders() const;

    void SetGridOffset(Vector2 offset) { gridOffset = offset; }
    Vector2 GetGridOffset() const { return gridOffset; }

private:
    struct Tile {
        float    worldX, worldY;  // posición mundial exacta (ya con snap)
        TileType type;
    };

    std::vector<Tile>          tiles;
    std::vector<CollisionRect> colliders;
    Vector2                    gridOffset = { 0.0f, 0.0f };

    // Snap de una coordenada mundial al grid con offset
    float SnapToGrid(float val, float offset) const {
        return floorf((val - offset) / TILE_SIZE) * TILE_SIZE + offset;
    }

    int FindTileAt(float worldX, float worldY) const;
};