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
	for (auto& block : blocks) {
		block.Draw();
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

		// ========== 1. ACTUALIZAR JUGADOR ==========
		player.Update(camera.GetLeftLimit());

		// ========== 2. COLISIONES ==========
		ResolveCollisions();

		// ========== 3. ACTUALIZAR CÁMARA ==========
		camera.Update(player.GetPosition(), backgroundManager.GetWidth(), backgroundManager.GetHeight(), player.GetIsGrounded());

		// ========== 4. ACTUALIZAR SOLDADOS Y BALAS ==========
		for (auto& soldier : soldiers) {
			soldier.UpdateAI(player);
			soldier.Update();
		}
		for (auto& bullet : bullets) {
			bullet.Update();
		}

		// ========== 5. DIBUJAR ==========
		BeginDrawing();
		ClearBackground(BLACK);
		Draw();
		Timers();

		// ========== 6. AUDIO ==========
		if (!musicStarted)
		{
			audioManager.PlayMusic(audioManager.GetGameMusic());
			musicStarted = true;
		}
		audioManager.UpdateMusic(audioManager.GetGameMusic());
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

	// Detectar si está agachado y disparando
	bool isCrouching = player.IsCrouching();

	// Altura de disparo (ajusta estos valores)
	float normalYOffset = -20.0f;   // Altura normal (desde el centro)
	float crouchYOffset = -20.0f;   // Altura cuando está agachado
	float upYOffset = -20.0f;      // Altura cuando dispara hacia arriba

	switch (aimDir) {
	case PlayerDirection::LEFT:
		bulletPos = { playerPos.x, playerPos.y + playerHeight / 2 + (isCrouching ? crouchYOffset : normalYOffset) };
		directionX = -1;
		break;
	case PlayerDirection::RIGHT:
		bulletPos = { playerPos.x + playerWidth, playerPos.y + playerHeight / 2 + (isCrouching ? crouchYOffset : normalYOffset) };
		directionX = 1;
		break;
	case PlayerDirection::UP:
		bulletPos = { playerPos.x + playerWidth / 2, playerPos.y + upYOffset };
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
void Game::ResolveCollisions()
{
	BlockCollisions();
	BulletsCollision();
}
void Game::BulletsCollision() {
	auto bIt = bullets.begin();
	while (bIt != bullets.end()) {
		bool bulletHit = false;
		auto sIt = soldiers.begin();
		while (sIt != soldiers.end()) {
			if (sIt->GetisAlive() && CheckCollisionRecs(sIt->GetHurtBox(), bIt->GetHitbox())) {
				sIt->TriggerDeath();       // ← activa animación de muerte
				UiManager.AddScore(100);
				bulletHit = true;
				break;
			}
			++sIt;
		}
		if (bulletHit) bIt = bullets.erase(bIt);
		else           ++bIt;
	}

	// ── Borrar soldados muertos DESPUÉS del loop de balas ────────────────────
	auto sIt = soldiers.begin();
	while (sIt != soldiers.end()) {
		if (!sIt->GetisAlive() && sIt->IsDeadAnimFinished()) {
			sIt = soldiers.erase(sIt);
		}
		else {
			++sIt;
		}
	}
}

void Game::BlockCollisions()
{
	bool onGround = false;
	const float GROUND_TOLERANCE = 5.0f;  // Tolerancia de 5 píxeles
	auto It = soldiers.begin();
	for (const auto& block : blocks) {
		Rectangle playerRect = player.GetHitBox();
		Rectangle blockRect = block.GetRect();

		float feetY = playerRect.y + playerRect.height;
		float blockTopY = blockRect.y;

		while (It != soldiers.end()) {
			if (CheckCollisionRecs(It->GetHurtBox(), blockRect))
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
			// Verificar si el jugador está sobre el bloque (con tolerancia)
			
		}
		bool isOverBlock = (playerRect.x + playerRect.width > blockRect.x + GROUND_TOLERANCE &&
			playerRect.x < blockRect.x + blockRect.width - GROUND_TOLERANCE);

		// Si los pies están cerca del bloque (dentro de 10 píxeles)
		if (isOverBlock && feetY >= blockTopY - 10.0f && player.GetVelocityY() >= 0) {
			float newY = blockTopY - playerRect.height;
			player.SetY(newY);
			player.SetVelocityY(0);
			onGround = true;
			break;  // Salir del bucle después de la primera colisión
		}
		

		
		
	}
	player.SetGrounded(onGround);
	// Si está en el suelo, asegurar que la velocidad Y es 0
	if (onGround) {
		player.SetVelocityY(0);

	}
}

std::vector<Bullet> Game::CreateBullets()
{
	std::vector<Bullet> bullets;
	for (int i = 0; i < bullets.size(); i++) {

		if (bullets[i].GetX() < 0 || bullets[i].GetX() > GetScreenWidth()) {
			bullets.erase(bullets.begin() + i);
			i--;
		}
	}
	return bullets;
}

std::vector<Soldier>  Game::CreateSoldiers()
	{
		std::vector<Soldier> soldiers;
		soldiers.reserve(4);
		for (int i = 0; i < 1; ++i) {
			float xpos, ypos;
			ypos = (10 * i + 40) + 100;
			xpos = (100 * i + 40) + 500;
			/*soldiers.emplace_back(Soldier(1, { xpos,ypos }));*/
			soldiers.emplace_back(Soldier(2, { xpos,ypos }));
		}
		soldiers.emplace_back(Soldier(1, {200, 200}));

		return soldiers;
	}

std::vector<Block> Game::CreateBlocks()
{
	std::vector<Block> blocks;

	// Bloque de suelo a y=800 (para que el jugador caiga desde y=100 hasta y=800)
	blocks.emplace_back(Block(0, 850, 2700, 100));
	blocks.emplace_back(Block(2700, 950, 2700, 100));

	return blocks;
}

