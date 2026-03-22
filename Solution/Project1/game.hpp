#pragma once
#include "Player.hpp"
#include "Soldier.hpp"
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
		std::vector<Soldier> soldiers; 
		std::vector<Soldier> CreateSoldiers();
};