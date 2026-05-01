#include "CreationManager.hpp"

CreationManager::CreationManager() {}
CreationManager::~CreationManager() {}

std::vector<Bullet>& CreationManager::GetBullets() { return bullets; }
std::vector<Block>& CreationManager::GetBlocks() { return blocks; }
std::vector<Item>& CreationManager::GetItems() { return items; }
std::vector<Soldier>& CreationManager::GetSoldiers() { return soldiers; }
std::vector<Grenade>& CreationManager::GetGrenades() { return grenades; }

void CreationManager::LoadBlocksFromFile(const char* filename) {
    blocks.clear();
    soldiers.clear();
    items.clear();

    FILE* file;
    fopen_s(&file, filename, "r");
    if (!file) {
        TraceLog(LOG_WARNING, "No se pudo cargar: %s", filename);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'B') {
            float x, y, w, h; int t;
            if (sscanf_s(line + 2, "%f,%f,%f,%f,%d", &x, &y, &w, &h, &t) == 5) {
                if (t == 2) blocks.emplace_back(x, y, w, h, BlockType::RAMP_UP);
                else if (t == 3) blocks.emplace_back(x, y, w, h, BlockType::RAMP_DOWN);
                else if (t == 4) blocks.emplace_back(x, y, w, h, BlockType::CEILING);
                else blocks.emplace_back(x, y, w, h, t == 1);
            }
        }
        else if (line[0] == 'S') {
            float x, y; int t;
            if (sscanf_s(line + 2, "%f,%f,%d", &x, &y, &t) == 3)
                soldiers.emplace_back(t, Vector2{ x, y });
        }
        else if (line[0] == 'I') {
            float x, y; int t;
            if (sscanf_s(line + 2, "%f,%f,%d", &x, &y, &t) == 3)
                items.emplace_back(Vector2{ x, y }, t == 1 ? ItemType::BOX : ItemType::SHOTGUN);
        }
    }
    fclose(file);
    TraceLog(LOG_INFO, "Nivel cargado: %d bloques, %d soldados, %d items",
        (int)blocks.size(), (int)soldiers.size(), (int)items.size());
}
