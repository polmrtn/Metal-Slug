#include "game.hpp"
bool musicStarted = false;
Rectangle obstacle = Rectangle{ 800,600,200,175 };
Game::Game()
{
	soldiers = CreateSoldiers();
	bullets = CreateBullets();
	
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
	for (auto& bullet : bullets) {
		bullet.Draw();
	}	
}

void Game::Update()
{
	if (sceneManager.GetGamestate() == SceneManager::TITLE) {
		sceneManager.DrawTexts();
	    audioManager.PlayMusic(audioManager.GetTitleMusic());
	    UpdateMusicStream(audioManager.GetTitleMusic());
		
	}
	else if (sceneManager.GetGamestate() == SceneManager::GAME) {
		
		UpdateMusicStream(audioManager.GetGameMusic());
		Draw();
		DrawRectangleLinesEx(obstacle, 5, WHITE);
		bool isColliding = CheckCollisionRecs(player.GetRect(), obstacle);
		player.DrawHitBox(isColliding);
		ClearBackground(BLACK);
		player.Update();

		for (auto& Soldier : soldiers) { //auto&(is a variable that the compiler assumes from the vector) in this case type Soldier, this initializes the update in each soldier
			Soldier.Update();
		}
		for (auto& bullet : bullets) {
			bullet.Update(); //update all bullets
		}
	

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
		player.StopMovingHorizontal();
	}

	//AIMING UP
	if (IsKeyDown(KEY_UP))
	{
		player.StartAimingUp();
	}
	else
	{
		player.StopAimingUp();
	}

	//CROUCHING
	if (IsKeyDown(KEY_DOWN))
	{
		player.StartCrouching();
	}
	else
	{
		player.StopCrouching();
	}

	//JUMP
	if (IsKeyPressed(KEY_SPACE))
	{
		player.Jump();
	}

	//CHANGE SCENE
	if (IsKeyPressed(KEY_ENTER) && sceneManager.currentState == SceneManager::TITLE)
	{
		//audioManager.StopMusic(audioManager.GetTitleMusic());
		//audioManager.PlayMusic(audioManager.GetGameMusic());
		sceneManager.SetGameState(SceneManager::GAME);
	}

	if (IsKeyPressed(KEY_D) || IsKeyDown(KEY_D))
	{
		player.Shoot();

		Vector2 playerPos = player.GetPosition();
		float playerWidth = player.GetWidth();  // Siempre el mismo ancho
		float playerHeight = player.GetHeight(); // Cambia si está agachado

		Vector2 bulletPos;
		PlayerDirection aimDir = player.GetAimDirection();
		int bulletSpeed = 10;
		int directionX = 0;
		int directionY = 0;

		switch (aimDir) {
		case PlayerDirection::LEFT:
			bulletPos = { playerPos.x, playerPos.y + playerHeight / 2 };
			directionX = -1;
			break;
		case PlayerDirection::RIGHT:
			bulletPos = { playerPos.x + playerWidth, playerPos.y + playerHeight / 2 };
			directionX = 1;
			break;
		case PlayerDirection::UP:
			bulletPos = { playerPos.x + playerWidth / 2, playerPos.y };
			directionY = -1;
			break;
		case PlayerDirection::DOWN:
			bulletPos = { playerPos.x + playerWidth / 2, playerPos.y + playerHeight };
			directionY = 1;
			break;
		}

		bullets.emplace_back(bulletPos, bulletSpeed, directionX, directionY);
	}

}

std::vector<Soldier>  Game::CreateSoldiers()
{
	std::vector<Soldier> soldiers;
	for (int i = 0; i < 10; i++) {
		float xpos, ypos;
		ypos = (10 * i + 40) + 100;
		xpos = (10 * i+ 40) + 100;
		soldiers.push_back(Soldier(1, { xpos,ypos}));
	}
	
	return soldiers;
}
std::vector<Bullet> Game::CreateBullets() {
	std::vector<Bullet> bullets;
	for (int i = 0; i < bullets.size(); i++) {
		// Usar el getter GetX() para acceder a la posición X
		if (bullets[i].GetX() < 0 || bullets[i].GetX() > GetScreenWidth()) {
			bullets.erase(bullets.begin() + i);
			i--; // Decrementar índice porque el vector se redujo
		}
	}
	return bullets;
}