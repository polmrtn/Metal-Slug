#define _CRT_SECURE_NO_WARNINGS
#include "LevelMap.hpp"
#include <algorithm>
#include <cstdio>
#include <cmath>

static Color TileColor(TileType t) {
    switch (t) {
    case TileType::SOLID:    return ColorAlpha(BLUE, 0.35f);
    case TileType::PLATFORM: return ColorAlpha(GREEN, 0.35f);
    case TileType::CEILING:  return ColorAlpha(PURPLE, 0.35f);
    case TileType::RAMP_UP:  return ColorAlpha(ORANGE, 0.35f);
    default:                 return ColorAlpha(WHITE, 0.2f);
    }
}
static Color TileBorder(TileType t) {
    switch (t) {
    case TileType::SOLID:    return BLUE;
    case TileType::PLATFORM: return GREEN;
    case TileType::CEILING:  return PURPLE;
    case TileType::RAMP_UP:  return ORANGE;
    default:                 return GRAY;
    }
}

// ─────────────────────────────────────────────────────────────
//  Edición
// ─────────────────────────────────────────────────────────────
int TileMap::FindTileAt(float worldX, float worldY) const {
    const float EPS = 0.5f;
    for (int i = 0; i < (int)tiles.size(); ++i)
        if (fabsf(tiles[i].worldX - worldX) < EPS &&
            fabsf(tiles[i].worldY - worldY) < EPS)
            return i;
    return -1;
}

void TileMap::AddTileWorld(float worldX, float worldY, TileType type) {
    // Snap al grid con el offset actual
    float snappedX = SnapToGrid(worldX, gridOffset.x);
    float snappedY = SnapToGrid(worldY, gridOffset.y);

    int idx = FindTileAt(snappedX, snappedY);
    if (idx >= 0) tiles[idx].type = type;
    else          tiles.push_back({ snappedX, snappedY, type });
}

void TileMap::RemoveTileWorld(float worldX, float worldY) {
    float snappedX = SnapToGrid(worldX, gridOffset.x);
    float snappedY = SnapToGrid(worldY, gridOffset.y);

    int idx = FindTileAt(snappedX, snappedY);
    if (idx >= 0) tiles.erase(tiles.begin() + idx);
}

void TileMap::Clear() {
    tiles.clear();
    colliders.clear();
}

// ─────────────────────────────────────────────────────────────
//  Bake — fusiona tiles adyacentes del mismo tipo
// ─────────────────────────────────────────────────────────────
void TileMap::Bake() {
    colliders.clear();
    if (tiles.empty()) return;

    struct MarkedTile {
        float    worldX, worldY;
        TileType type;
        bool     used = false;
    };

    std::vector<MarkedTile> marked;
    marked.reserve(tiles.size());
    for (auto& t : tiles) marked.push_back({ t.worldX, t.worldY, t.type, false });

    // Ordenar por Y luego X
    std::sort(marked.begin(), marked.end(), [](const MarkedTile& a, const MarkedTile& b) {
        if (fabsf(a.worldY - b.worldY) > 0.1f) return a.worldY < b.worldY;
        return a.worldX < b.worldX;
        });

    const float EPS = 0.5f;

    for (int i = 0; i < (int)marked.size(); ++i) {
        if (marked[i].used) continue;

        // Rampas: collider individual
        if (marked[i].type == TileType::RAMP_UP) {
            colliders.push_back({
                { marked[i].worldX, marked[i].worldY, TILE_SIZE, TILE_SIZE },
                marked[i].type
                });
            marked[i].used = true;
            continue;
        }

        TileType type = marked[i].type;
        float    startX = marked[i].worldX;
        float    rowY = marked[i].worldY;

        // 1. Extender en X
        float endX = startX;
        for (int j = i + 1; j < (int)marked.size(); ++j) {
            if (marked[j].used) continue;
            if (fabsf(marked[j].worldY - rowY) > EPS) break;  // ← diferente fila, para
            if (marked[j].type != type) continue;              // ← diferente tipo, salta
            if (fabsf(marked[j].worldX - (endX + TILE_SIZE)) > EPS) break;  // ← no adyacente, para
            endX = marked[j].worldX;
            marked[j].used = true;  // ← AÑADE ESTO — marca como usado al extender en X
        }

        // 2. Extender en Y
        float endY = rowY;
        bool  canExtend = true;
        while (canExtend) {
            float nextY = endY + TILE_SIZE;
            for (float cx = startX; cx <= endX + EPS; cx += TILE_SIZE) {
                bool found = false;
                for (auto& m : marked)
                    if (!m.used && m.type == type &&
                        fabsf(m.worldY - nextY) < EPS &&
                        fabsf(m.worldX - cx) < EPS)
                    {
                        found = true; break;
                    }
                if (!found) { canExtend = false; break; }
            }
            if (canExtend) endY = nextY;
        }

        // 3. Marcar usados
        for (auto& m : marked)
            if (!m.used && m.type == type &&
                m.worldX >= startX - EPS && m.worldX <= endX + EPS &&
                m.worldY >= rowY - EPS && m.worldY <= endY + EPS)
                m.used = true;

        // 4. Crear CollisionRect
        float w = (endX - startX) + TILE_SIZE;
        float h = (endY - rowY) + TILE_SIZE;
        colliders.push_back({ { startX, rowY, w, h }, type });
    }

    TraceLog(LOG_INFO, "TileMap::Bake — %d tiles -> %d colliders",
        (int)tiles.size(), (int)colliders.size());
}

// ─────────────────────────────────────────────────────────────
//  Serialización — guarda posición mundial directamente
// ─────────────────────────────────────────────────────────────
void TileMap::SaveToFile(const char* filename) const {
    FILE* f = fopen(filename, "w");
    if (!f) { TraceLog(LOG_WARNING, "TileMap: no se pudo guardar %s", filename); return; }

    // Guardar offset del grid para restaurarlo en el editor
    fprintf(f, "O %.4f %.4f\n", gridOffset.x, gridOffset.y);

    // Guardar posición mundial exacta de cada tile
    for (auto& t : tiles)
        fprintf(f, "T %.4f %.4f %d\n", t.worldX, t.worldY, (int)t.type);

    fclose(f);
    TraceLog(LOG_INFO, "TileMap guardado: %s (%d tiles)", filename, (int)tiles.size());
}

void TileMap::LoadFromFile(const char* filename) {
    Clear();
    FILE* f = fopen(filename, "r");
    if (!f) { TraceLog(LOG_WARNING, "TileMap: no se pudo cargar %s", filename); return; }

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == 'O') {
            float ox, oy;
            if (sscanf(line + 2, "%f %f", &ox, &oy) == 2)
                gridOffset = { ox, oy };
        }
        else if (line[0] == 'T') {
            float wx, wy; int type;
            if (sscanf(line + 2, "%f %f %d", &wx, &wy, &type) == 3)
                tiles.push_back({ wx, wy, (TileType)type });
        }
        // S e I los maneja CreationManager
    }

    fclose(f);
    Bake();
    TraceLog(LOG_INFO, "TileMap cargado: %s (%d tiles, %d colliders)",
        filename, (int)tiles.size(), (int)colliders.size());
}

// ─────────────────────────────────────────────────────────────
//  Debug visual
// ─────────────────────────────────────────────────────────────
void TileMap::DrawTiles() const {
    for (auto& t : tiles) {
        Rectangle r = { t.worldX, t.worldY, TILE_SIZE, TILE_SIZE };

        if (t.type == TileType::RAMP_UP) {
            Vector2 a = { r.x,           r.y + r.height };
            Vector2 b = { r.x + r.width, r.y };
            Vector2 c = { r.x + r.width, r.y + r.height };
            DrawTriangle(a, b, c, TileColor(t.type));
            DrawLine((int)a.x, (int)a.y, (int)b.x, (int)b.y, TileBorder(t.type));
            DrawLine((int)b.x, (int)b.y, (int)c.x, (int)c.y, TileBorder(t.type));
            DrawLine((int)c.x, (int)c.y, (int)a.x, (int)a.y, TileBorder(t.type));
        }
        else {
            DrawRectangleRec(r, TileColor(t.type));
            DrawRectangleLinesEx(r, 1.0f, TileBorder(t.type));
        }

        const char* label = "";
        switch (t.type) {
        case TileType::SOLID:    label = "S"; break;
        case TileType::PLATFORM: label = "P"; break;
        case TileType::CEILING:  label = "C"; break;
        case TileType::RAMP_UP:  label = "R"; break;
        }
        DrawText(label, (int)(r.x + 2), (int)(r.y + 2), 10, TileBorder(t.type));
    }
}

void TileMap::DrawColliders() const {
    for (auto& c : colliders) {
        if (c.type == TileType::RAMP_UP) {
            Vector2 a = { c.rect.x,               c.rect.y + c.rect.height };
            Vector2 b = { c.rect.x + c.rect.width, c.rect.y };
            Vector2 c2 = { c.rect.x + c.rect.width, c.rect.y + c.rect.height };
            DrawTriangle(a, b, c2, ColorAlpha(ORANGE, 0.25f));
            DrawLine((int)a.x, (int)a.y, (int)b.x, (int)b.y, ORANGE);
            DrawLine((int)b.x, (int)b.y, (int)c2.x, (int)c2.y, ORANGE);
            DrawLine((int)c2.x, (int)c2.y, (int)a.x, (int)a.y, ORANGE);
        }
        else {
            DrawRectangleLinesEx(c.rect, 2.0f, TileBorder(c.type));
        }
    }
}