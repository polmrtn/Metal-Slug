#pragma once
#include "Player.hpp"

class Game {
	public:
		Game();
		~Game();
		void Draw();
		void Update();
		void HandleInput();
	private:
		Player player;
};