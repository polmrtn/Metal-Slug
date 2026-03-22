#include "game.hpp"

Game::Game()
{
	soldiers = CreateSoldiers();
}

Game::~Game()
{

}

void Game:: Draw()
{
	player.Draw();

	for (auto& Soldier : soldiers) {//auto&(is a variable that the compiler assumes from the vector) in this case type Soldier, this initializes the draw in each soldier
		Soldier.Draw();
	}
}

void Game::Update()
{
	player.Update();  // Actualiza física del jugador
	for (auto& Soldier : soldiers) { //auto&(is a variable that the compiler assumes from the vector) in this case type Soldier, this initializes the update in each soldier
		Soldier.Update();
	}
}



void Game::HandleInput()
{
	if (IsKeyDown(KEY_LEFT))
	{
		player.MoveLeft();
	}
	else if (IsKeyDown(KEY_RIGHT))
	{
		player.MoveRight();
	}
	else
	{
		player.StopMoving();
	}
	if (IsKeyPressed(KEY_SPACE))
	{
		player.Jump();
	}
}

std::vector<Soldier>  Game::CreateSoldiers()
{
	std::vector<Soldier> soldiers;
	for (int i = 0; i < 10; i++) {
		float xpos, ypos;
		ypos = (10 * i) + 100;
		xpos = (10 * i) + 100;
		soldiers.push_back(Soldier(1, { xpos,ypos}));
	}
	
	return soldiers;
}
