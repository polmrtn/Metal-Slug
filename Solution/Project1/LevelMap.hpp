#pragma once
#include <raylib.h>
#include <vector>

// ─────────────────────────────────────────────────────────────
//  Tamaño del tile — modifica aquí si necesitas otro tamaño
// ─────────────────────────────────────────────────────────────
constexpr float TILE_SIZE = 50.0f;

// ─────────────────────────────────────────────────────────────
//  Tipos de tile
// ─────────────────────────────────────────────────────────────
enum class TileType {
    SOLID,      // Suelo sólido: bloquea arriba, abajo y laterales
    PLATFORM,   // Plataforma: solo bloquea cayendo desde arriba
    CEILING,    // Techo: solo bloquea saltando desde abajo
    RAMP_UP,    // Rampa que sube izquierda→derecha (45°)
};

// ─────────────────────────────────────────────────────────────
//  Colisionador fusionado — resultado del merge de tiles
//  Un CollisionRect representa uno o varios tiles del mismo tipo
//  fusionados en un único rectángulo.
// ─────────────────────────────────────────────────────────────
struct CollisionRect {
    Rectangle rect;
    TileType  type;

    // Para rampas: calcula la Y de la superficie en una X de mundo
    float GetRampSurfaceY(float worldX) const {
        float t = (worldX - rect.x) / rect.width;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        // RAMP_UP: izquierda = abajo, derecha = arriba
        return rect.y + rect.height - t * rect.height;
    }
};

// ─────────────────────────────────────────────────────────────
//  TileMap
//  Almacena tiles individuales y genera CollisionRects fusionados.
// ─────────────────────────────────────────────────────────────
class TileMap {
public:
    TileMap() = default;

    // ── Edición ──────────────────────────────────────────────
    // Añade un tile en coordenadas de tile (columna, fila)
    void AddTile(int col, int row, TileType type);
    // Añade un tile en coordenadas de mundo (se convierte a tile)
    void AddTileWorld(float worldX, float worldY, TileType type);
    // Elimina el tile en coordenadas de mundo más cercano al punto
    void RemoveTileWorld(float worldX, float worldY);
    // Elimina todos los tiles
    void Clear();

    // ── Merge ────────────────────────────────────────────────
    // Fusiona tiles adyacentes del mismo tipo en CollisionRects.
    // Llama esto después de cargar/editar el nivel.
    void Bake();

    // ── Acceso ───────────────────────────────────────────────
    const std::vector<CollisionRect>& GetColliders() const { return colliders; }

    // ── Serialización ────────────────────────────────────────
    void SaveToFile(const char* filename) const;
    void LoadFromFile(const char* filename);

    // ── Debug visual ─────────────────────────────────────────
    // Dibuja tiles (editor) y colliders fusionados
    void DrawTiles() const;       // tiles individuales con color por tipo
    void DrawColliders() const;   // rectángulos fusionados
    void DrawGrid() const;        // cuadrícula de referencia

private:
    struct Tile {
        int      col, row;
        TileType type;
    };

    std::vector<Tile>          tiles;      // tiles sin fusionar
    std::vector<CollisionRect> colliders;  // resultado del Bake()

    // Convierte col/row a Rectangle de mundo
    static Rectangle TileRect(int col, int row) {
        return { col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
    }

    // Encuentra el índice de un tile por col/row, -1 si no existe
    int FindTile(int col, int row) const;
};