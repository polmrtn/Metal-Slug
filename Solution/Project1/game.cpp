#include "game.hpp"
#include "bullet.hpp"

bool musicStarted = false;

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
		
		player.Update();

		for (auto& bullet : bullets) {
			bullet.Update(); //update all bullets
		}
		for (int i = 0; i < bullets.size(); i++) {
			// Usar el getter GetX() para acceder a la posición X
			if (bullets[i].GetX() < 0 || bullets[i].GetX() > GetScreenWidth()) {
				bullets.erase(bullets.begin() + i);
				i--; // Decrementar índice porque el vector se redujo
			}
		}

		for (auto& Soldier : soldiers) { //auto&(is a variable that the compiler assumes from the vector) in this case type Soldier, this initializes the update in each soldier
			Soldier.Update();
		}

		Draw();
		ClearBackground(BLACK);

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
	if (IsKeyPressed(KEY_ENTER) && sceneManager.currentState == SceneManager::TITLE)
	{
		audioManager.StopMusic(audioManager.GetTitleMusic());
		audioManager.PlayMusic(audioManager.GetGameMusic());
		sceneManager.SetGameState(SceneManager::GAME);
	}
	if (IsKeyPressed(KEY_D) || IsKeyDown(KEY_D))
	{
		player.Shoot();

		// Crear bala cuando se presiona D
		Vector2 playerPos = player.GetPosition();
		float playerWidth = player.GetWidth();
		float playerHeight = player.GetHeight();

		Vector2 bulletPos = {
			playerPos.x + playerWidth / 2,
			playerPos.y + playerHeight / 2
		};

		// Usar la dirección actual del jugador
		int direction = player.GetDirection();

		// Velocidad de la bala (ajusta este valor según necesites)
		int bulletSpeed = 10;

		bullets.emplace_back(bulletPos, bulletSpeed, direction);
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


