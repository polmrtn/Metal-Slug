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
			if (soldier.WantsToShoot()) {
				// Llamamos a la función Shoot que arreglamos antes
				Shoot(2, soldier.GetPosition(), soldier.IsFacingRight());

				// IMPORTANTE: Resetear la bandera para que no dispare 60 balas por segundo
				soldier.ResetShootWants();
			}
	
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


void Game::Shoot(int BulletType, Vector2 startPos, bool faceRight)
{
	// Variables base para la nueva bala
	Vector2 bulletPos = { 0, 0 };
	float directionX = 0;
	float directionY = 0;
	float bulletSpeed = 1000.0f;
	float yOffset = -20.0f;

	// --- LÓGICA PARA BALA TIPO 1 (JUGADOR) ---
	if (BulletType == 1)
	{
		Vector2 pPos = player.GetPosition();
		float pW = player.GetWidth();
		float pH = player.GetHeight();
		PlayerDirection aimDir = player.GetAimDirection();
		bulletSpeed = 1000.0f;

		switch (aimDir) {
		case PlayerDirection::LEFT:
			bulletPos = { pPos.x, pPos.y + pH / 2 + yOffset };
			directionX = -1.0f;
			break;
		case PlayerDirection::RIGHT:
			bulletPos = { pPos.x + pW, pPos.y + pH / 2 + yOffset };
			directionX = 1.0f;
			break;
		case PlayerDirection::UP:
			bulletPos = { pPos.x + pW / 2, pPos.y + yOffset };
			directionY = -1.0f;
			break;
		case PlayerDirection::DOWN:
			bulletPos = { pPos.x + pW / 2, pPos.y + pH };
			directionY = 1.0f;
			break;
		}
	}
	// --- LÓGICA PARA BALA TIPO 2 (SOLDADO / GRANADA) ---
	else if (BulletType == 2)
	{
		bulletPos = startPos;
		bulletSpeed = 50.0f;
		// Velocidad horizontal (qué tan lejos llega)
		bulletSpeed = 400.0f;

		directionX = faceRight ? 1.0f : -1.0f;

		// IMPULSO HACIA ARRIBA: Debe ser negativo para que "salte"
		// Prueba con -4.0f para un arco alto o -2.0f para un arco bajo
		directionY = -5.0f;

	
	}

	// Finalmente, añadimos la bala al vector con los datos calculados
	bullets.emplace_back(bulletPos, bulletSpeed, directionX, directionY, BulletType);
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
		Shoot(1, player.GetPosition(), (player.GetAimDirection() != PlayerDirection::LEFT));
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
		else ++bIt;
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
		auto bIt = bullets.begin();
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
	auto bIt = bullets.begin();
	while (bIt != bullets.end()) {
		bool bulletJustHit = false;

		// 1. Si NO está explotando, checar colisión con bloques
		if (!bIt->IsExploding()) {
			for (const auto& block : blocks) {
				if (CheckCollisionRecs(bIt->GetHitbox(), block.GetRect())) {
					bulletJustHit = true;
					break;
				}
			}
		}

		// 2. Manejar el impacto
		if (bulletJustHit) {
			if (bIt->GetType() == 1) {
				bIt = bullets.erase(bIt);
				continue; // ← ya avanza el iterador, no llega al ++bIt
			}
			else if (bIt->GetType() == 2) {
				if (!bIt->IsExploding()) {
					// Solo activar la explosión UNA vez al tocar el suelo
					bIt->SetExploding(true);
					bIt->GetAnim().SetAnimation(BulletState::EXPLOSIONSOLDIER);
				}

				if (bIt->GetAnim().IsAnimationFinished()) {
					bIt = bullets.erase(bIt);
					continue; // ← importante para no hacer ++bIt después
				}
			}
		}
		++bIt;

		// 4. Si no se borró en ningún paso anterior, avanzar
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
		soldiers.reserve(1);
		for (int i = 0; i < 1; ++i) {
			float xpos, ypos;
			ypos = (10 * i + 40) + 100;
			xpos = (100 * i + 40) + 500;
			/*soldiers.emplace_back(Soldier(1, { xpos,ypos }));*/
			soldiers.emplace_back(Soldier(2, { xpos,ypos }));
			
		}
		soldiers.emplace_back(Soldier(1, { 200, 200 }));

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

