#include "game.hpp"
#include "LevelMap.hpp" 
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

void Game::Update(){
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

		// ========== 1. GUARDAR POSICIÓN ANTERIOR ==========
		player.SavePreviousPosition();

		// ========== 2. INPUT ==========
		HandleInput();

		// ========== 3. ACTUALIZAR JUGADOR (FÍSICA Y MOVIMIENTO) ==========
		player.Update(camera.GetLeftLimit());

		// ========== 4. COLISIONES ==========
		ResolveCollisions();

		// ========== 5. ACTUALIZAR CÁMARA ==========
		camera.Update(player.GetPosition(), backgroundManager.GetWidth(), backgroundManager.GetHeight(), player.GetIsGrounded());

		// ========== 6. ACTUALIZAR SOLDADOS Y BALAS ==========
		for (auto& soldier : soldiers) {
			soldier.UpdateAI(player);
			soldier.Update();
		}
		for (auto& bullet : bullets) {
			bullet.Update();
		}

		// ========== 7. DIBUJAR ==========
		BeginDrawing();
		ClearBackground(BLACK);
		Draw();
		Timers();

		// ========== 8. AUDIO ==========
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

void Game::ResolveCollisions(){
	Rectangle playerHB = player.GetHitBox();
	bool onGround = false;

	for (const auto& block : blocks) {
		Rectangle blockRect = block.GetRect();

		// 1. Verificamos si hay colisión general (AABB)
		if (CheckCollisionRecs(playerHB, blockRect)) {

			// 2. Lógica para SUELO (Solo si el jugador cae)
			// Chequeamos si la velocidad Y es positiva (cayendo) 
			// y si la base del jugador estaba por encima del bloque antes del frame actual
			if (player.GetVelocityY() >= 0 &&
				(player.GetY() + player.GetHeight() - player.GetVelocityY() <= blockRect.y + 5.0f)) {

				player.SetY(blockRect.y - player.GetHeight());
				player.SetVelocityY(0);
				onGround = true;
			}

			// 3. Lógica lateral (Paredes)
			// Solo si el bloque es realmente una pared (puedes añadir un bool al Block)
			// O si la diferencia de altura es suficiente para chocar lateralmente
			else {
				// Aquí iría tu lógica de SetRightCollision que ya tenías
				// Pero asegurándote de que no se ejecute si ya detectamos que es suelo
			}
		}
	}
	player.SetGrounded(onGround);
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

void Game::BlockCollisions() {
	bool onGround = false;
	const float GROUND_TOLERANCE = 5.0f;

	// Resetear colisiones laterales cada frame
	player.SetLeftCollision(false);
	player.SetRightCollision(false);

	for (const auto& block : blocks) {
		Rectangle playerRect = player.GetHitBox();
		Rectangle blockRect = block.GetRect();

		// ========== COLISIÓN SUELO (con detección de atravesar desde abajo) ==========
		float feetY = playerRect.y + playerRect.height;
		float blockTopY = blockRect.y;
		float previousFeetY = player.GetPreviousY() + player.GetHeight();

		// Si la velocidad Y es negativa (está subiendo), probablemente viene desde abajo
		bool wasBelow = (player.GetVelocityY() < 0 && previousFeetY <= blockTopY);

		float verticalDistance = feetY - blockTopY;
		bool isVerticalNear = (verticalDistance >= 0 && verticalDistance <= 50.0f);
		bool isOverBlock = (playerRect.x + playerRect.width > blockRect.x + GROUND_TOLERANCE &&
			playerRect.x < blockRect.x + blockRect.width - GROUND_TOLERANCE);

		// Solo colisionar si NO viene desde abajo y está cayendo
		if (isOverBlock && isVerticalNear && player.GetVelocityY() >= 0 && !wasBelow && verticalDistance <= 20.0f) {
			// Verificar si es suelo sólido o plataforma
			if (block.IsGround()) {
				// Suelo sólido: siempre colisiona
				float newY = blockTopY - playerRect.height;
				player.SetY(newY);
				player.SetVelocityY(0);
				onGround = true;
				TraceLog(LOG_INFO, "SUELO SOLIDO - newY: %.2f", newY);
			}
			else {
				// Plataforma: solo colisiona si NO viene desde abajo (ya lo tenemos con !wasBelow)
				float newY = blockTopY - playerRect.height;
				player.SetY(newY);
				player.SetVelocityY(0);
				onGround = true;
				TraceLog(LOG_INFO, "PLATAFORMA - newY: %.2f", newY);
			}
		}

		// ========== COLISIONES LATERALES (SOLO PARA SUELOS SÓLIDOS) ==========
		if (block.IsGround()) {
			// Colisión lateral izquierda
			Rectangle leftHitBox = player.GetLeftHitBox();
			if (CheckCollisionRecs(leftHitBox, blockRect)) {
				float newX = blockRect.x + blockRect.width;
				if (player.GetX() < newX) {
					player.SetX(newX);
					player.SetLeftCollision(true);
					TraceLog(LOG_INFO, "COLISION IZQUIERDA - newX: %.2f", newX);
				}
				else {
					player.SetLeftCollision(true);
				}
			}

			// Colisión lateral derecha
			Rectangle rightHitBox = player.GetRightHitBox();
			if (CheckCollisionRecs(rightHitBox, blockRect)) {
				float newX = blockRect.x - player.GetWidth();
				if (player.GetX() + player.GetWidth() > blockRect.x) {
					player.SetX(newX);
					player.SetRightCollision(true);
					TraceLog(LOG_INFO, "COLISION DERECHA - newX: %.2f", newX);
				}
				else {
					player.SetRightCollision(true);
				}
			}
		}
		// Si es plataforma (isGround = false), NO se aplican colisiones laterales
	}

	player.SetGrounded(onGround);
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
		soldiers.reserve(2);
		for (int i = 0; i < 2; ++i) {
			float xpos, ypos;
			ypos = (10 * i + 40) + 100;
			xpos = (100 * i + 40) + 500;
			soldiers.emplace_back(Soldier(1, { xpos,ypos }));
		}

		return soldiers;
	}

std::vector<Block> Game::CreateBlocks()
{
	std::vector<Block> blocks;

	// Suelos sólidos (isGround = true)
	blocks.emplace_back(Block(0, 850, 500, 100, true));
	blocks.emplace_back(Block(500, 950, 500, 100, true));

	// Plataforma elevada (isGround = false - atravesable desde abajo)
	blocks.emplace_back(Block(600, 550, 500, 100, false));
	
	return blocks;
}

