#define _CRT_SECURE_NO_WARNINGS
#include "CreationManager.hpp"
#include "GlobalManagers.hpp"
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
	Globals::cameraManager.Reset();

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
            if (sscanf(line + 2, "%f %f %d", &x, &y, &type) == 3) {
                ItemType itemType;
                switch (type) {
                case 0: itemType = ItemType::SHOTGUN; break;
                case 1: itemType = ItemType::BOX;     break;
                case 2: itemType = ItemType::PLUSHY;  break;
                case 3: itemType = ItemType::FISH;    break;
                case 4: itemType = ItemType::MEDAL;   break;
                default: itemType = ItemType::BOX;    break;
                }
                items.emplace_back(Vector2{ x, y }, itemType);
            }
        }
        else if (line[0] == 'Z') {
            float triggerX, minY, maxY;
            char id[64];
            if (sscanf(line + 2, "%f %f %f %s", &triggerX, &minY, &maxY, id) == 4)
                // CameraManager::AddZone signature requires 8 args now:
                // AddZone(const std::string& id, float triggerX,
                //        float minY, float maxY,
                //        float minX, float maxX,
                //        bool clampX, bool clampY);
                // Preserve old behaviour: do not override X-limits for legacy 4-field lines
                cameraManager.AddZone(id, triggerX, minY, maxY, 0.0f, 0.0f, false, true);
        }
    }
        
    

    fclose(f);

    // El TileMap carga sus propios tiles (O y T) por separado
    tileMap.LoadFromFile(filename);

    TraceLog(LOG_INFO, "Nivel cargado: %d soldados, %d items",
        (int)soldiers.size(), (int)items.size());
}