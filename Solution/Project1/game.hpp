#pragma once
#include "Player.hpp"
#include "Soldier.hpp"
#include "AudioManager.hpp"
#include "SceneManager.hpp"
#include "Bullet.hpp"
#include "BackgroundManager.hpp"
#include "CameraManager.hpp"
#include "LevelMap.hpp"
#include "UiManager.hpp"
#include <vector>


class Game {
	public:
		Game();
		~Game();
		void Draw();
		void Update();
		void HandleInput();
		void BlockCollisions();
		void Shoot();
		void Timers();
		void BulletsCollision();
		void ResolveCollisions();
	private:
		float shootTimer = 0.0f;
		float shootDelay = 1.0f;
		CameraManager camera;
		Player player;
		UiManager UiManager;
		AudioManager audioManager;
		SceneManager sceneManager;
		BackgroundManager backgroundManager;
		std::vector<Bullet> bullets;
		std::vector<Bullet> CreateBullets();
		std::vector<Soldier> soldiers; 
		std::vector<Soldier> CreateSoldiers();
		std::vector<Block> blocks;
		std::vector<Block> CreateBlocks();
};