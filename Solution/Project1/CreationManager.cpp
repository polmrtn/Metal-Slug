#include "CreationManager.hpp"
CreationManager::CreationManager()
{
	blocks = CreateBlocks();
	items = CreateItems();
	bullets = CreateBullets();
	soldiers = CreateSoldiers();
	grenades = CreateGrenades();
}

CreationManager::~CreationManager()
{
}
std::vector<Bullet> CreationManager::CreateBullets() {
	std::vector<Bullet> bullets;
	return bullets;
}
std::vector<Block> CreationManager::CreateBlocks() {
	std::vector<Block> blocks;
	return blocks;
}
std::vector<Item> CreationManager::CreateItems() {
	std::vector<Item> items;
	return items;
}

std::vector<Soldier> CreationManager::CreateSoldiers()
{
	return std::vector<Soldier>();
	return soldiers;
}

std::vector<Grenade> CreationManager::CreateGrenades()
{
	return std::vector<Grenade>();
	return grenades;
}

std::vector<Bullet> CreationManager::GetBullets()
{
	return bullets;
}

std::vector<Block> CreationManager::GetBlocks()
{
	return blocks;
}

std::vector<Item> CreationManager::GetItems()
{
	return items;
}

std::vector<Soldier> CreationManager::GetSoldiers()
{
	return soldiers;
}

std::vector<Grenade> CreationManager::GetGrenades()
{
	return grenades;
}
