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
        if (line[0] == 'O') {
            // el TileMap lo lee internamente en LoadFromFile
            // pero aquí lo ignoramos — CreationManager delega al TileMap
        }
        else if (line[0] == 'T') {
            // tiles — los maneja TileMap::LoadFromFile
        }
        else if (line[0] == 'S') {
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
        else if (line[0] == 'I') {
            float x, y; int type;
            if (sscanf(line + 2, "%f %f %d", &x, &y, &type) == 3) {
                ItemType t = ItemType::SHOTGUN;
                if (type == 1) t = ItemType::BOX;
                if (type == 2) t = ItemType::JETPACK;
                items.emplace_back(Vector2{ x, y }, t);
            }
        }
    }

    fclose(f);

    // El TileMap carga sus propios tiles (O y T) por separado
    tileMap.LoadFromFile(filename);

    TraceLog(LOG_INFO, "Nivel cargado: %d soldados, %d items",
        (int)soldiers.size(), (int)items.size());
}