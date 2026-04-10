#include "game.hpp"
#include <raymath.h>
bool musicStarted = false;

Game::Game() : camera({ 1280.0f/2 , 896/2  })
{
	soldiers = CreateSoldiers();
	bullets = CreateBullets();
	blocks = CreateBlocks();
	
}

Game::~Game()
{

}

void Game::Draw()
{
	camera.Begin();
	
	backgroundManager.Draw();
	player.Draw();

	for (auto& Soldier : soldiers) {
		Soldier.Draw();
	}

	for (auto& bullet : bullets) {
		bullet.Draw();
	}
	
	camera.End();

	UiManager.DrawCredits(camera.GetCamera());
	
}

void Game::Timers()
{
	shootTimer += GetFrameTime();
}
void Game::Update()
{
	
	if (sceneManager.GetGamestate() == SceneManager::INTRO) {
		BeginDrawing();
		ClearBackground(BLACK);
		sceneManager.DrawTexts();
	}
	else if (sceneManager.GetGamestate() == SceneManager::TITLE) {
		BeginDrawing();
		ClearBackground(BLACK);
		sceneManager.DrawTexts();
		if (!musicStarted)
		{
			audioManager.PlayMusic(audioManager.GetTitleMusic());
			musicStarted = true;
		}
		audioManager.UpdateMusic(audioManager.GetTitleMusic());
	}
	else if (sceneManager.GetGamestate() == SceneManager::GAME) {
		UiManager.Update();
		ResolveCollisions();
		camera.Update(player.GetPosition(), backgroundManager.GetWidth(), backgroundManager.GetHeight(),player.GetIsGrounded());
		BeginDrawing();
		ClearBackground(BLACK);
		Draw();
		Timers();
		if (!musicStarted)
		{
			audioManager.PlayMusic(audioManager.GetGameMusic());
			musicStarted = true;
		}
		audioManager.UpdateMusic(audioManager.GetGameMusic());
		player.Update(camera.GetLeftLimit());
		for (auto& soldier : soldiers) {
			soldier.Update();
			//update all bullets
		}
		for (auto& bullet : bullets) {
			bullet.Update();
			//update all bullets
		}
		
	}
}


void Game::Shoot()
{

	Vector2 playerPos = player.GetPosition();
	float playerWidth = player.GetWidth();  // Siempre el mismo ancho
	float playerHeight = player.GetHeight(); // Cambia si está agachado

	Vector2 bulletPos;
	PlayerDirection aimDir = player.GetAimDirection();
	int bulletSpeed = 30;
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





void Game::HandleInput()
{
	if (IsKeyPressed(KEY_L))
	{
		UiManager.NextLevel();
	}
	if (IsKeyPressed(KEY_J)) 
	{
		UiManager.AddScore(100);
	}
	if (IsKeyPressed(KEY_C))
	{
		if (UiManager.GetCredits() < 99) 
		{
			UiManager.SetCredits(1);
		}
			
	}
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
	if (IsKeyPressed(KEY_ENTER))
	{
		if (sceneManager.currentState == SceneManager::TITLE) {
			audioManager.StopMusic(audioManager.GetTitleMusic());
			audioManager.PlaySound(audioManager.GetGameSound());
			sceneManager.SetGameState(SceneManager::GAME);
			musicStarted = false;
		}
		else if (sceneManager.currentState == SceneManager::INTRO) {
			sceneManager.SetGameState(SceneManager::TITLE);
			musicStarted = false;
		}
	}

	if (IsKeyPressed(KEY_D) || IsKeyDown(KEY_D) && shootTimer >= shootDelay)
	{
		player.Shoot();
		Shoot();
		shootTimer = 0;
	}

}
void Game::BulletsCollision() {
	auto bIt = bullets.begin();
	while (bIt != bullets.end()) {
		bool bulletHit = false;
		auto sIt = soldiers.begin();
		while (sIt != soldiers.end()) {
			if (CheckCollisionRecs(sIt->GetHitBox(), bIt->GetHitbox())) {
				sIt = soldiers.erase(sIt);
				UiManager.AddScore(100);
				bulletHit = true;
				break;
			}
			else {
				++sIt;
			}
		}

		if (bulletHit) {
			bIt = bullets.erase(bIt);
		}
		else {
			++bIt;
		}
	}
}

void Game::BlockCollisions()
{

	Rectangle playerRect = player.GetHitBox();
	auto It = soldiers.begin();

	for (const auto& block : blocks) {
		Rectangle blockRect = block.GetRect();

		if (CheckCollisionRecs(playerRect, blockRect)) {
			if (player.GetVelocityY() > 0) {
				player.SetY(blockRect.y - player.GetHeight());
				player.SetVelocityY(0);
				player.SetGrounded(true);
				break;
			}
		}

		while (It != soldiers.end()) {
			if (CheckCollisionRecs(It->GetHitBox(), blockRect))
			{
				if (It->GetVelocityY() >= 0) {
					It->SetY(blockRect.y - It->GetHeight());
					It->SetVelocityY(0);
					It->SetGrounded(true);
					break;
				}
			}
			else {
				++It;
			}
		}
		
	
	}

}


void Game::ResolveCollisions() {
	BlockCollisions();
	BulletsCollision();
	
}

std::vector<Soldier>  Game::CreateSoldiers()
{
	std::vector<Soldier> soldiers;
	soldiers.reserve(10);
	for (int i = 0; i < 10; ++i) {
		float xpos, ypos;
		ypos = (10 * i + 40) + 100;
		xpos = (100 * i + 40) + 500;
		soldiers.emplace_back(Soldier(1, { xpos,ypos }));
	}

	return soldiers;
}
std::vector<Bullet> Game::CreateBullets() {
	std::vector<Bullet> bullets;
	for (int i = 0; i < bullets.size(); i++) {

		if (bullets[i].GetX() < 0 || bullets[i].GetX() > GetScreenWidth()) {
			bullets.erase(bullets.begin() + i);
			i--; 
		}
	}
	return bullets;
}

std::vector<Block> Game::CreateBlocks() {
	std::vector<Block> blocks;

	blocks.emplace_back(Block(0, 1000, 2000, 100));
	return blocks;
}
