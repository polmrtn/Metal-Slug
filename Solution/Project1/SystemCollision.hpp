#pragma once
#include <raylib.h>
#include "CreationManager.hpp"
#include "Player.hpp"
#include "AudioManager.hpp"
#include "UiManager.hpp"
#include "CameraManager.hpp"

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
	void ItemPlayerCollision(Item item);

private:
	CreationManager creationManager;
	Player player;
	AudioManager audioManager;
	UiManager uiManager;
	CameraManager cameraManager;
	
};

