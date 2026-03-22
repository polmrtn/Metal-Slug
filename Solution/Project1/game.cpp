#include "game.hpp"

Game::Game()
{

}

Game::~Game()
{

}

void Game:: Draw()
{
	player.Draw();
}

void Game::Update()
{
	player.Update();  // Actualiza física del jugador
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