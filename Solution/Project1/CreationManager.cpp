#define _CRT_SECURE_NO_WARNINGS
#include "CreationManager.hpp"
#include <cstdio>

CreationManager::CreationManager() {}
CreationManager::~CreationManager() {}

std::vector<Bullet>& CreationManager::GetBullets() { return bullets; }
std::vector<Soldier>& CreationManager::GetSoldiers() { return soldiers; }
std::vector<Grenade>& CreationManager::GetGrenades() { return grenades; }
std::vector<Item>& CreationManager::GetItems() { return items; }

void CreationManager::LoadFromFile(const char* filename)
{
    tileMap.Clear();
    soldiers.clear();
    items.clear();

    FILE* f = fopen(filename, "r");
    if (!f) {
        TraceLog(LOG_WARNING, "CreationManager: no se pudo abrir %s", filename);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), f))
    {
        if (line[0] == 'S') {
            float x, y; int type;
            if (sscanf(line + 2, "%f %f %d", &x, &y, &type) == 3)
                soldiers.emplace_back(type, Vector2{ x, y });
        }
        else if (line[0] == 'I') {
            float x, y; int type;
            if (sscanf(line + 2, "%f %f %d", &x, &y, &type) == 3)
                items.emplace_back(Vector2{ x, y },
                    type == 1 ? ItemType::BOX : ItemType::SHOTGUN);
        }
        else {
            int col, row, type;
            if (sscanf(line, "%d %d %d", &col, &row, &type) == 3)
                tileMap.AddTile(col, row, (TileType)type);
        }
    }

    fclose(f);
    tileMap.Bake();

    TraceLog(LOG_INFO, "Nivel cargado: %d soldados, %d items",
        (int)soldiers.size(), (int)items.size());
}