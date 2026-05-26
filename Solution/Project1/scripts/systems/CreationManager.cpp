#define _CRT_SECURE_NO_WARNINGS
#include "CreationManager.hpp"
#include <cstdio>

CreationManager::CreationManager() {}
CreationManager::~CreationManager() {}

std::vector<Bullet>& CreationManager::GetBullets() { return bullets; }
std::vector<Soldier>& CreationManager::GetSoldiers() { return soldiers; }
std::vector<Grenade>& CreationManager::GetGrenades() { return grenades; }
std::vector<Item>& CreationManager::GetItems() { return items; }
std::vector<BloodEffect>& CreationManager::GetBloodEffects() { return bloodEffects; }
std::vector<Prisoner>& CreationManager::GetPrisoners() { return prisoners; }
std::vector<FloatingText>& CreationManager::GetFloatingTexts() { return floatingTexts; }

void CreationManager::LoadFromFile(const char* filename)
{
    tileMap.Clear();
    soldiers.clear();
    items.clear();
    prisoners.clear();

    FILE* f = fopen(filename, "r");
    if (!f) {
        TraceLog(LOG_WARNING, "CreationManager: no se pudo abrir %s", filename);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), f))
    {
        if (line[0] == 'S') {
            float x, y; int type, flipped;
            if (sscanf(line + 2, "%f %f %d %d", &x, &y, &type, &flipped) == 4)
                soldiers.emplace_back(type, Vector2{ x, y }, flipped == 1);
            else if (sscanf(line + 2, "%f %f %d", &x, &y, &type) == 3)
                soldiers.emplace_back(type, Vector2{ x, y }, false);
        }
        else if (line[0] == 'I') {
            float x, y; int type;
            if (sscanf(line + 2, "%f %f %d", &x, &y, &type) == 3) {
                ItemType itemType;
                switch (type) {
                case 0:  itemType = ItemType::SHOTGUN; break;
                case 1:  itemType = ItemType::BOX;     break;
                case 2:  itemType = ItemType::JETPACK; break;
                case 3:  itemType = ItemType::PLUSHY;  break;
                case 4:  itemType = ItemType::FISH;    break;
                case 5:  itemType = ItemType::MEDAL;   break;
                case 6:  itemType = ItemType::PIG;     break;
                case 7:  itemType = ItemType::BOMBS;   break;
                default: itemType = ItemType::BOX;     break;
                }
                items.emplace_back(Vector2{ x, y }, itemType);
            }
        }
        else if (line[0] == 'P' && line[1] == 'R') {
            float x, y; int t, flipped;
            if (sscanf(line + 3, "%f %f %d %d", &x, &y, &t, &flipped) == 4)
                prisoners.emplace_back(Vector2{ x, y },
                    t == 0 ? PrisonerType::GROUND : PrisonerType::POLE,
                    flipped == 1);
        }
    }

    fclose(f);
    tileMap.LoadFromFile(filename);
    TraceLog(LOG_INFO, "Nivel cargado: %d soldados, %d items, %d prisoners",
        (int)soldiers.size(), (int)items.size(), (int)prisoners.size());
}