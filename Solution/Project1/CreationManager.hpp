#pragma once
#include <raylib.h>
#include <vector>
#include "Bullet.hpp"
#include "Soldier.hpp"
#include "Grenade.hpp"
#include "item.hpp"
#include "LevelMap.hpp"

class CreationManager
{
public:
    CreationManager();
    ~CreationManager();

    // ── TileMap (colisiones + nivel) ──────────────────────────
    TileMap& GetTileMap() { return tileMap; }

    // ── Entidades dinámicas ───────────────────────────────────
    std::vector<Bullet>& GetBullets();
    std::vector<Soldier>& GetSoldiers();
    std::vector<Grenade>& GetGrenades();
    std::vector<Item>& GetItems();

    // ── Carga del nivel ───────────────────────────────────────
    // Formato nuevo: tiles  (col row type por línea)
    // Soldados:  S x y type
    // Items:     I x y type
    void LoadFromFile(const char* filename);

private:
    TileMap              tileMap;
    std::vector<Bullet>  bullets;
    std::vector<Soldier> soldiers;
    std::vector<Grenade> grenades;
    std::vector<Item>    items;
};