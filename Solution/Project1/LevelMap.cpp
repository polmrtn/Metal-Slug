#define _CRT_SECURE_NO_WARNINGS
#include "LevelMap.hpp"
#include <algorithm>
#include <cstdio>
#include <cmath>

// ─────────────────────────────────────────────────────────────
//  Colores de debug por tipo
// ─────────────────────────────────────────────────────────────
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
int TileMap::FindTile(int col, int row) const {
    for (int i = 0; i < (int)tiles.size(); ++i)
        if (tiles[i].col == col && tiles[i].row == row) return i;
    return -1;
}

void TileMap::AddTile(int col, int row, TileType type) {
    int idx = FindTile(col, row);
    if (idx >= 0) tiles[idx].type = type;   // sobreescribe si ya existe
    else          tiles.push_back({ col, row, type });
}

void TileMap::AddTileWorld(float worldX, float worldY, TileType type) {
    int col = (int)floorf(worldX / TILE_SIZE);
    int row = (int)floorf(worldY / TILE_SIZE);
    AddTile(col, row, type);
}

void TileMap::RemoveTileWorld(float worldX, float worldY) {
    int col = (int)floorf(worldX / TILE_SIZE);
    int row = (int)floorf(worldY / TILE_SIZE);
    int idx = FindTile(col, row);
    if (idx >= 0) tiles.erase(tiles.begin() + idx);
}

void TileMap::Clear() {
    tiles.clear();
    colliders.clear();
}

// ─────────────────────────────────────────────────────────────
//  Bake — fusiona tiles adyacentes del mismo tipo
//  Algoritmo: barre fila a fila de izquierda a derecha,
//  extiende en X todo lo posible, luego intenta extender en Y.
// ─────────────────────────────────────────────────────────────
void TileMap::Bake() {
    colliders.clear();
    if (tiles.empty()) return;

    // Trabajamos con una copia marcada para no repetir tiles
    struct MarkedTile { int col, row; TileType type; bool used = false; };
    std::vector<MarkedTile> marked;
    marked.reserve(tiles.size());
    for (auto& t : tiles) marked.push_back({ t.col, t.row, t.type, false });

    // Ordenar por fila, luego columna
    std::sort(marked.begin(), marked.end(), [](const MarkedTile& a, const MarkedTile& b) {
        if (a.row != b.row) return a.row < b.row;
        return a.col < b.col;
        });

    for (int i = 0; i < (int)marked.size(); ++i) {
        if (marked[i].used) continue;

        // Rampas nunca se fusionan — cada una es su propio collider
        if (marked[i].type == TileType::RAMP_UP) {
            Rectangle r = TileRect(marked[i].col, marked[i].row);
            colliders.push_back({ r, marked[i].type });
            marked[i].used = true;
            continue;
        }

        TileType type = marked[i].type;
        int startCol = marked[i].col;
        int row = marked[i].row;

        // 1. Extender en X (misma fila, misma tipo, adyacente)
        int endCol = startCol;
        for (int j = i + 1; j < (int)marked.size(); ++j) {
            if (marked[j].used) continue;
            if (marked[j].row != row) break;            // ya no es la misma fila
            if (marked[j].type != type) continue;
            if (marked[j].col != endCol + 1) break;     // no adyacente
            endCol = marked[j].col;
        }

        // 2. Extender en Y (filas siguientes con el mismo rango de columnas)
        int endRow = row;
        bool canExtend = true;
        while (canExtend) {
            int nextRow = endRow + 1;
            // Comprobar que existen todos los tiles [startCol..endCol] en nextRow
            for (int c = startCol; c <= endCol; ++c) {
                bool found = false;
                for (auto& m : marked) {
                    if (!m.used && m.row == nextRow && m.col == c && m.type == type) {
                        found = true; break;
                    }
                }
                if (!found) { canExtend = false; break; }
            }
            if (canExtend) endRow = nextRow;
        }

        // 3. Marcar todos los tiles usados en el rectángulo
        for (auto& m : marked) {
            if (!m.used && m.type == type &&
                m.col >= startCol && m.col <= endCol &&
                m.row >= row && m.row <= endRow)
                m.used = true;
        }

        // 4. Crear CollisionRect
        float x = startCol * TILE_SIZE;
        float y = row * TILE_SIZE;
        float w = (endCol - startCol + 1) * TILE_SIZE;
        float h = (endRow - row + 1) * TILE_SIZE;
        colliders.push_back({ { x, y, w, h }, type });
    }

    TraceLog(LOG_INFO, "TileMap::Bake — %d tiles → %d colliders",
        (int)tiles.size(), (int)colliders.size());
}

// ─────────────────────────────────────────────────────────────
//  Serialización
// ─────────────────────────────────────────────────────────────
void TileMap::SaveToFile(const char* filename) const {
    FILE* f = fopen(filename, "w");
    if (!f) { TraceLog(LOG_WARNING, "TileMap: no se pudo guardar %s", filename); return; }
    for (auto& t : tiles)
        fprintf(f, "%d %d %d\n", t.col, t.row, (int)t.type);
    fclose(f);
    TraceLog(LOG_INFO, "TileMap guardado: %s (%d tiles)", filename, (int)tiles.size());
}

void TileMap::LoadFromFile(const char* filename) {
    Clear();
    FILE* f = fopen(filename, "r");
    if (!f) { TraceLog(LOG_WARNING, "TileMap: no se pudo cargar %s", filename); return; }
    int col, row, type;
    while (fscanf(f, "%d %d %d", &col, &row, &type) == 3)
        AddTile(col, row, (TileType)type);
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
        Rectangle r = TileRect(t.col, t.row);
        if (t.type == TileType::RAMP_UP) {
            // Triángulo: esquina inf-izq, esquina sup-izq, esquina inf-der
            Vector2 a = { r.x,          r.y + r.height };
            Vector2 b = { r.x,          r.y };
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
        // Etiqueta del tipo
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
            Vector2 a = { c.rect.x,              c.rect.y + c.rect.height };
            Vector2 b = { c.rect.x,              c.rect.y };
            Vector2 cc2 = { c.rect.x + c.rect.width, c.rect.y + c.rect.height };
            DrawTriangle(a, b, cc2, ColorAlpha(ORANGE, 0.25f));
            DrawLine((int)a.x, (int)a.y, (int)b.x, (int)b.y, ORANGE);
            DrawLine((int)b.x, (int)b.y, (int)cc2.x, (int)cc2.y, ORANGE);
            DrawLine((int)cc2.x, (int)cc2.y, (int)a.x, (int)a.y, ORANGE);
        }
        else {
            DrawRectangleLinesEx(c.rect, 2.0f, TileBorder(c.type));
        }
    }
}

void TileMap::DrawGrid() const {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    // Dibuja una cuadrícula de referencia (en coordenadas de pantalla)
    for (float x = fmodf(0, TILE_SIZE); x < screenW; x += TILE_SIZE)
        DrawLineV({ x, 0 }, { x, (float)screenH }, ColorAlpha(GRAY, 0.3f));
    for (float y = fmodf(0, TILE_SIZE); y < screenH; y += TILE_SIZE)
        DrawLineV({ 0, y }, { (float)screenW, y }, ColorAlpha(GRAY, 0.3f));
}