#pragma once
#include "Player.hpp"
#include "Soldier.hpp"
#include "AudioManager.hpp"
#include "SceneManager.hpp"
#include "Bullet.hpp"
#include <vector>


class Game {
	public:
		Game();
		~Game();
		void Draw();
		void Update();
		void HandleInput();
		void CheckForCollisions();

	private:
		Player player;
		AudioManager audioManager;
		SceneManager sceneManager;
		std::vector<Bullet> bullets;
		std::vector<Bullet> CreateBullets();
	
		std::vector<Soldier> soldiers; 
		std::vector<Soldier> CreateSoldiers();
};