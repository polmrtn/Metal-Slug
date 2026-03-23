#pragma once
#include "Player.hpp"
#include "Soldier.hpp"
#include "AudioManager.hpp"
#include "SceneManager.hpp"
#include "bullet.hpp"
#include <vector>



class Game {
	public:
		Game();
		~Game();
		void Draw();
		void Update();
		void HandleInput();

	private:
		Player player;
		AudioManager audioManager;
		SceneManager sceneManager;
		std::vector<Soldier> soldiers; 
		std::vector<Bullet> bullets;
		std::vector<Soldier> CreateSoldiers();

};