#include "CreationManager.hpp"

CreationManager::CreationManager() {}
CreationManager::~CreationManager() {}

std::vector<Bullet>& CreationManager::GetBullets() { return bullets; }
std::vector<Block>& CreationManager::GetBlocks() { return blocks; }
std::vector<Item>& CreationManager::GetItems() { return items; }
std::vector<Soldier>& CreationManager::GetSoldiers() { return soldiers; }
std::vector<Grenade>& CreationManager::GetGrenades() { return grenades; }