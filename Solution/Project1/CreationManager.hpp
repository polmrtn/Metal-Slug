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
	
	//setters
	std::vector<Bullet> CreateBullets();
	std::vector<Block> CreateBlocks();
	std::vector<Item> CreateItems();
	std::vector<Soldier>CreateSoldiers();
	std::vector<Grenade> CreateGrenades();



	//getters
	std::vector<Bullet> GetBullets();
	std::vector<Block> GetBlocks();
	std::vector<Item> GetItems();
	std::vector<Soldier>GetSoldiers();
	std::vector<Grenade> GetGrenades();
	

private:
	std::vector<Bullet> bullets;
	std::vector<Soldier> soldiers;
	std::vector<Grenade> grenades;
	std::vector<Item> items;
	std::vector<Block> blocks;
	

};

