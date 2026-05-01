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

    // Getters — devuelven referencia para permitir modificación directa
    std::vector<Bullet>& GetBullets();
    std::vector<Block>& GetBlocks();
    std::vector<Item>& GetItems();
    std::vector<Soldier>& GetSoldiers();
    std::vector<Grenade>& GetGrenades();

private:
    std::vector<Bullet>   bullets;
    std::vector<Block>    blocks;
    std::vector<Item>     items;
    std::vector<Soldier>  soldiers;
    std::vector<Grenade>  grenades;
};