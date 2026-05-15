#pragma once
#include <raylib.h>
#include "GlobalManagers.hpp"


class SystemCollision
{
public:
	SystemCollision();
	~SystemCollision();
	void CollisionUpdate();
	void SoldierBlockCollision();
	void GrenadesCollision();
	void BulletCollision();
	void PlayerBlockCollision();
	void ItemBlockCollision();
	void BulletBlockCollision();
	void ItemPlayerCollision();
	void BossAttackPlayerCollision();
	void PlayerBoxCollision();
	void PrisonerBlockCollision();
	
private:
	
	
};

