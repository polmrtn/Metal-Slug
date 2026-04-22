#define _CRT_SECURE_NO_WARNINGS
#include "game.hpp"
#include "LevelMap.hpp" 
#include <raymath.h>


bool musicStarted = false;
Color BGCOLOR = { 195, 195, 170 };
Game::Game() : camera({ 1200.0f/2 , 896/2  })
{
	FILE* file = fopen("level_blocks.txt", "r");
	if (file) {
		fclose(file);
		LoadBlocksFromFile("level_blocks.txt");
	}
	else {
		blocks = CreateBlocks();  // Solo si no hay archivo
	}
	soldiers = CreateSoldiers();
	bullets = CreateBullets();	
	items = CreateItems();
}

Game::~Game()
{

}

void Game::Draw()
{
	camera.Begin();
	
	backgroundManager.Draw();
	
	
	player.Draw();
	backgroundManager.Drawfrontground();

	for (auto& Soldier : soldiers) {
		Soldier.Draw();
	}

	// Dibujar bloques
	for (auto& block : blocks) {
		block.Draw();
	}

	for (auto& bullet : bullets) {
		bullet.Draw();
	}

	for (auto& grenade : grenades) {
		grenade.Draw();
	}

	for (auto& item : items) {
		item.Draw();
	}
	
	camera.End();

	UiManager.DrawCredits(camera.GetCamera());

	if (editorMode) {
		// Dibujar grid
		for (float x = fmod(gridOffset.x, gridSize); x < GetScreenWidth(); x += gridSize) {
			DrawLineV({ x, 0 }, { x, (float)GetScreenHeight() }, GRAY);
		}
		for (float y = fmod(gridOffset.y, gridSize); y < GetScreenHeight(); y += gridSize) {
			DrawLineV({ 0, y }, { (float)GetScreenWidth(), y }, GRAY);
		}

		// Texto de ayuda
		DrawText("EDITOR MODE - F1: Salir | Click: Suelo | Right: Plataforma | Middle: Borrar | F5: Guardar",
			10, 10, 15, RED);

		// Posición del ratón en el mundo
		Vector2 mousePos = GetMousePosition();
		Vector2 worldPos = camera.GetScreenToWorld(mousePos);
		DrawText(TextFormat("World: (%.0f, %.0f)", worldPos.x, worldPos.y), 10, 35, 15, YELLOW);
	}
	
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

		if (!player.IsAlive()) {
			BeginDrawing();

			// Mostrar mensaje con la posición de muerte
			Vector2 deathPos = player.GetDeathPosition();
			DrawText(TextFormat("YOU DIED at (%.0f, %.0f)", deathPos.x, deathPos.y),
				GetScreenWidth() / 2 - 200, GetScreenHeight() / 2 - 50, 20, RED);
			DrawText("Press R to respawn at death position",
				GetScreenWidth() / 2 - 200, GetScreenHeight() / 2, 20, WHITE);

			if (IsKeyPressed(KEY_R)) {
				player.Respawn();
			}
		}

		// ========== 1. GUARDAR POSICIÓN ANTERIOR ==========
		player.SavePreviousPosition();

		// ========== 2. INPUT ==========
		HandleInput();

		// ========== 3. ACTUALIZAR JUGADOR (FÍSICA Y MOVIMIENTO) ==========
		player.Update(camera.GetLeftLimit());

		// ========== 4. COLISIONES ==========
		ResolveCollisions();

		for (auto& item : items) {
			item.Update();  // ← añadir

			if (item.IsActive() && CheckCollisionRecs(item.GetHitBox(), player.GetHitBox())) {
				if (item.GetType() == ItemType::SHOTGUN) {
					player.EquipMachinegun();
					item.Collect();
				}
				// La caja no se recoge caminando encima, se destruye con bala
			}

			// Spawn machinegun al destruirse la caja
			if (item.ShouldSpawnMachinegun()) {
				item.ConsumeSpawn();
				Vector2 spawnPos = { item.GetHitBox().x + 60.0f, item.GetHitBox().y - 20.0f };
				Item newItem(spawnPos, ItemType::SHOTGUN);
				newItem.SetGravity(true);
				items.push_back(newItem);
			}
		}

		// Limpiar items inactivos
		items.erase(std::remove_if(items.begin(), items.end(),
			[](const Item& i) { return !i.IsActive(); }), items.end());

		// ========== 5. ACTUALIZAR CÁMARA ==========
		camera.Update(player.GetPosition(), backgroundManager.GetWidth(), backgroundManager.GetHeight(), player.GetIsGrounded());

		// ========== 6. ACTUALIZAR SOLDADOS Y BALAS ==========
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
		for (auto& grenade : grenades) {
			grenade.Update();
			grenade.CheckCollisionWithBlocks(blocks);
			grenade.CheckCollisionWithSoldiers(soldiers);
		}

		// Actualizar cooldown de granada
		if (grenadeCooldown > 0.0f) {
			grenadeCooldown -= GetFrameTime();
		}

		// Actualizar cooldown de disparo
		if (shootTimer > 0.0f) {
			shootTimer -= GetFrameTime();
		}

		// Limpiar granadas inactivas
		grenades.erase(std::remove_if(grenades.begin(), grenades.end(),
			[](const Grenade& g) { return !g.IsActive(); }), grenades.end());

		// ========== RÁFAGA MACHINEGUN ==========
		if (machinegunBurst) {
			machinegunBurstTimer += GetFrameTime();
			if (machinegunBurstTimer >= machinegunBurstDelay) {
				machinegunBurstTimer = 0.0f;

				if (player.GetAmmo() > 0) {
					ShootMachinegun(burstOffsets[machinegunBurstCount]);
					player.UseAmmo();
				}

				machinegunBurstCount++;
				if (machinegunBurstCount >= MACHINEGUN_BURST_SIZE || player.GetAmmo() <= 0) {
					machinegunBurst = false;
					machinegunBurstCount = 0;
					// Si ya no está pulsado UP, bajar el arma ahora
					if (!IsKeyDown(KEY_UP)) {
						player.StopAimingUp();
					}
				}
			}
		}

		// ========== 7. DIBUJAR ==========
		BeginDrawing();
		ClearBackground(BGCOLOR);
		Draw();
		backgroundManager.FollowPlayer(camera.GetCamera().target);
		backgroundManager.Update(GetFrameTime());
		

		// ========== 8. AUDIO ==========
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
	Vector2 bulletPos = { 0, 0 };
	float directionX = 0;
	float directionY = 0;
	float bulletSpeed = 1000.0f;
	float yOffset = -20.0f;

	if (BulletType == 1)
	{
		Vector2 pPos = player.GetPosition();
		float pW = player.GetWidth();
		float pH = player.GetHeight();
		PlayerDirection aimDir = player.GetAimDirection();
		bool isCrouching = player.IsCrouching();
		float normalYOffset = -50.0f;
		float crouchYOffset = -40.0f;

		switch (aimDir) {
		case PlayerDirection::LEFT:
			bulletPos = { pPos.x, pPos.y + pH / 2 + (isCrouching ? crouchYOffset : normalYOffset) };
			directionX = -1.0f;
			break;
		case PlayerDirection::RIGHT:
			bulletPos = { pPos.x + pW, pPos.y + pH / 2 + (isCrouching ? crouchYOffset : normalYOffset) };
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
	else if (BulletType == 2)
	{
		bulletPos = startPos;
		bulletSpeed = 400.0f;
		directionX = faceRight ? 1.0f : -1.0f;
		directionY = -5.0f;
	}

	bullets.emplace_back(bulletPos, bulletSpeed, directionX, directionY, BulletType);
}

void Game::ThrowGrenade() {
	GrenadeThrowData data = player.ThrowGrenade();
	if (data.valid) {
		grenades.emplace_back(data.startPos, data.initialVelocity);
	}
}

//handleinput antiguo
//void Game::HandleInput()
//{
//	if (!player.IsAlive()) {
//		if (IsKeyPressed(KEY_R)) {
//			player.Respawn();
//		}
//		return;
//	}
//
//	if (IsKeyPressed(KEY_L)) {
//		UiManager.NextLevel();
//	}
//	if (IsKeyPressed(KEY_J)) {
//		UiManager.AddScore(100);
//	}
//	if (IsKeyPressed(KEY_C)) {
//		if (UiManager.GetCredits() < 99) {
//			UiManager.SetCredits(1);
//		}
//	}
//
//	// ========== MOVIMIENTO ==========
//	if (IsKeyDown(KEY_LEFT)) {
//		player.MoveLeft();
//	}
//	else if (IsKeyDown(KEY_RIGHT)) {
//		player.MoveRight();
//	}
//	else {
//		player.StopMovingHorizontal();
//	}
//
//	// ========== AIMING UP ==========
//	if (IsKeyPressed(KEY_UP) && !machinegunBurst) {
//		player.StartAimingUp();
//	}
//	if (IsKeyReleased(KEY_UP) && !machinegunBurst) {
//		player.StopAimingUp();
//	}
//
//	// ========== CROUCHING ==========
//	if (IsKeyDown(KEY_DOWN)) {
//		player.StartCrouching();
//	}
//	else {
//		player.StopCrouching();
//	}
//
//	// ========== JUMP ==========
//	if (IsKeyPressed(KEY_SPACE)) {
//		player.Jump();
//	}
//
//	// ========== DISPARO ==========
//	if (IsKeyPressed(KEY_D) && shootTimer <= 0.0f) {
//		if (player.GetCurrentWeapon() == WeaponType::MACHINEGUN) {
//			if (player.GetAmmo() > 0) {
//				player.Shoot();
//				machinegunBurst = true;
//				machinegunBurstCount = 0;
//				machinegunBurstTimer = 0.0f;
//				shootTimer = shootDelayMachinegun;
//			}
//		}
//		else {
//			player.Shoot();
//			Shoot(1, { 0, 0 }, true);
//			shootTimer = shootDelayPistol;
//		}
//	}
//
//	// ========== GRANADA ==========
//	if (IsKeyPressed(KEY_S) && player.IsAlive() && grenadeCooldown <= 0.0f) {
//		ThrowGrenade();
//		grenadeCooldown = grenadeDelay;
//	}
//
//	// ========== CHANGE SCENE ==========
//	if (IsKeyPressed(KEY_ENTER)) {
//		if (sceneManager.currentState == SceneManager::TITLE) {
//			audioManager.StopMusic(audioManager.GetTitleMusic());
//			audioManager.PlaySound(audioManager.GetGameSound());
//			sceneManager.SetGameState(SceneManager::GAME);
//			musicStarted = false;
//		}
//		else if (sceneManager.currentState == SceneManager::INTRO) {
//			sceneManager.SetGameState(SceneManager::TITLE);
//			musicStarted = false;
//		}
//	}
//
//	// ========== MODO EDITOR ==========
//	static float f1Cooldown = 0.0f;
//	if (IsKeyPressed(KEY_F1) && f1Cooldown <= 0.0f) {
//		editorMode = !editorMode;
//		f1Cooldown = 0.2f;
//		TraceLog(LOG_INFO, "Editor mode: %s", editorMode ? "ON" : "OFF");
//	}
//	if (f1Cooldown > 0.0f) {
//		f1Cooldown -= GetFrameTime();
//	}
//
//	if (editorMode) {
//		// Mover grid con WASD
//		if (IsKeyDown(KEY_W)) gridOffset.y -= 5;
//		if (IsKeyDown(KEY_S)) gridOffset.y += 5;
//		if (IsKeyDown(KEY_A)) gridOffset.x -= 5;
//		if (IsKeyDown(KEY_D)) gridOffset.x += 5;
//
//		Vector2 mousePos = GetMousePosition();
//		Vector2 worldPos = camera.GetScreenToWorld(mousePos);
//
//		int tileX = (int)floor((worldPos.x - gridOffset.x) / gridSize);
//		int tileY = (int)floor((worldPos.y - gridOffset.y) / gridSize);
//
//		float blockX = gridOffset.x + tileX * gridSize;
//		float blockY = gridOffset.y + tileY * gridSize;
//
//		// Click izquierdo: suelo normal
//		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
//			blocks.emplace_back(blockX, blockY, gridSize, gridSize, true);
//		}
//		// Click derecho: plataforma
//		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
//			blocks.emplace_back(blockX, blockY, gridSize, gridSize, false);
//		}
//		// Tecla R: rampa que sube
//		if (IsKeyPressed(KEY_R)) {
//			blocks.emplace_back(blockX, blockY, gridSize, gridSize, BlockType::RAMP_UP);
//			TraceLog(LOG_INFO, "Rampa UP creada en (%.0f, %.0f)", blockX, blockY);
//		}
//		// Tecla T: rampa que baja
//		if (IsKeyPressed(KEY_T)) {
//			blocks.emplace_back(blockX, blockY, gridSize, gridSize, BlockType::RAMP_DOWN);
//			TraceLog(LOG_INFO, "Rampa DOWN creada en (%.0f, %.0f)", blockX, blockY);
//		}
//		// Click medio: borrar bloque
//		if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
//			auto it = std::remove_if(blocks.begin(), blocks.end(),
//				[blockX, blockY](const Block& b) {
//					return b.GetRect().x == blockX && b.GetRect().y == blockY;
//				});
//			blocks.erase(it, blocks.end());
//			TraceLog(LOG_INFO, "Bloque borrado en (%.0f, %.0f)", blockX, blockY);
//		}
//
//		// Guardar nivel
//		if (IsKeyPressed(KEY_F5)) {
//			SaveBlocksToFile("level_blocks.txt");
//		}
//	}
//
//}

void Game::HandleInput()
{
	if (!player.IsAlive()) {
		if (IsKeyPressed(KEY_R)) {
			player.Respawn();
		}
		return;
	}

	// ========== CAMBIO DE ESCENA (SIEMPRE DISPONIBLE) ==========
	if (IsKeyPressed(KEY_ENTER)) {
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

	// ========== MODO EDITOR (SIEMPRE DISPONIBLE) ==========
	static float f1Cooldown = 0.0f;
	if (IsKeyPressed(KEY_F1) && f1Cooldown <= 0.0f) {
		editorMode = !editorMode;
		f1Cooldown = 0.2f;
		TraceLog(LOG_INFO, "Editor mode: %s", editorMode ? "ON" : "OFF");
	}
	if (f1Cooldown > 0.0f) {
		f1Cooldown -= GetFrameTime();
	}

	if (editorMode) {
		// Mover grid con WASD
		if (IsKeyDown(KEY_W)) gridOffset.y -= 5;
		if (IsKeyDown(KEY_S)) gridOffset.y += 5;
		if (IsKeyDown(KEY_A)) gridOffset.x -= 5;
		if (IsKeyDown(KEY_D)) gridOffset.x += 5;

		Vector2 mousePos = GetMousePosition();
		Vector2 worldPos = camera.GetScreenToWorld(mousePos);

		int tileX = (int)floor((worldPos.x - gridOffset.x) / gridSize);
		int tileY = (int)floor((worldPos.y - gridOffset.y) / gridSize);

		float blockX = gridOffset.x + tileX * gridSize;
		float blockY = gridOffset.y + tileY * gridSize;

		// Tecla Y: techo (ceiling)
		if (IsKeyPressed(KEY_Y)) {
			blocks.emplace_back(blockX, blockY, gridSize, gridSize, BlockType::CEILING);
			TraceLog(LOG_INFO, "Techo creado en (%.0f, %.0f)", blockX, blockY);
		}
		// Click izquierdo: suelo normal
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			blocks.emplace_back(blockX, blockY, gridSize, gridSize, true);
		}
		// Click derecho: plataforma
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
			blocks.emplace_back(blockX, blockY, gridSize, gridSize, false);
		}
		// Tecla R: rampa que sube
		if (IsKeyPressed(KEY_R)) {
			blocks.emplace_back(blockX, blockY, gridSize, gridSize, BlockType::RAMP_UP);
			TraceLog(LOG_INFO, "Rampa UP creada en (%.0f, %.0f)", blockX, blockY);
		}
		// Tecla T: rampa que baja
		if (IsKeyPressed(KEY_T)) {
			blocks.emplace_back(blockX, blockY, gridSize, gridSize, BlockType::RAMP_DOWN);
			TraceLog(LOG_INFO, "Rampa DOWN creada en (%.0f, %.0f)", blockX, blockY);
		}
		// Click medio: borrar bloque
		if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
			auto it = std::remove_if(blocks.begin(), blocks.end(),
				[blockX, blockY](const Block& b) {
					return b.GetRect().x == blockX && b.GetRect().y == blockY;
				});
			blocks.erase(it, blocks.end());
			TraceLog(LOG_INFO, "Bloque borrado en (%.0f, %.0f)", blockX, blockY);
		}

		// Guardar nivel
		if (IsKeyPressed(KEY_F5)) {
			SaveBlocksToFile("level_blocks.txt");
		}
	}

	// ========== SI ESTÁ CAYENDO, NO PROCESAR INPUT DEL JUGADOR ==========
	if (player.IsFalling()) return;

	// ========== DEBUG (solo si NO está cayendo) ==========
	if (IsKeyPressed(KEY_L)) {
		UiManager.NextLevel();
	}
	if (IsKeyPressed(KEY_J)) {
		UiManager.AddScore(100);
	}
	if (IsKeyPressed(KEY_C)) {
		if (UiManager.GetCredits() < 99) {
			UiManager.SetCredits(1);
		}
	}

	// ========== MOVIMIENTO ==========
	if (machinegunBurst) {
		// Durante la ráfaga horizontal: solo permitir moverse en el MISMO sentido
		if (machinegunBurstDir == PlayerDirection::LEFT) {
			if (IsKeyDown(KEY_LEFT)) player.MoveLeft();
			else player.StopMovingHorizontal();
		}
		else if (machinegunBurstDir == PlayerDirection::RIGHT) {
			if (IsKeyDown(KEY_RIGHT)) player.MoveRight();
			else player.StopMovingHorizontal();
		}
		// Salto permitido siempre
		if (IsKeyPressed(KEY_SPACE)) player.Jump();
		// Agacharse permitido siempre
		if (IsKeyDown(KEY_DOWN)) player.StartCrouching();
		else player.StopCrouching();
		// KEY_UP, dirección contraria y disparo bloqueados → no hacer nada más
		return;
	}

	// ========== MOVIMIENTO NORMAL (sin ráfaga) ==========
	if (IsKeyDown(KEY_LEFT)) {
		player.MoveLeft();
	}
	else if (IsKeyDown(KEY_RIGHT)) {
		player.MoveRight();
	}
	else {
		player.StopMovingHorizontal();
	}

	// ========== AIMING UP ==========
	if (IsKeyPressed(KEY_UP)) {
		player.StartAimingUp();
	}
	if (IsKeyReleased(KEY_UP)) {
		player.StopAimingUp();
	}

	// ========== CROUCHING ==========
	if (IsKeyDown(KEY_DOWN)) {
		player.StartCrouching();
	}
	else {
		player.StopCrouching();
	}

	// ========== JUMP ==========
	if (IsKeyPressed(KEY_SPACE)) {
		player.Jump();
	}

	// ========== DISPARO ==========
	if (IsKeyPressed(KEY_D) && shootTimer <= 0.0f) {
		if (player.GetCurrentWeapon() == WeaponType::MACHINEGUN) {
			if (player.GetAmmo() > 0) {
				player.Shoot();
				machinegunBurst = true;
				machinegunBurstCount = 0;
				machinegunBurstTimer = 0.0f;
				shootTimer = shootDelayMachinegun;
				// Guardar dirección al inicio de la ráfaga
				machinegunBurstDir = (player.GetAimDirection() == PlayerDirection::UP)
					? PlayerDirection::UP
					: (IsKeyDown(KEY_LEFT) ? PlayerDirection::LEFT : PlayerDirection::RIGHT);
			}
		}
		else {
			player.Shoot();
			Shoot(1, { 0, 0 }, true);
			shootTimer = shootDelayPistol;
		}
	}

	// ========== GRANADA ==========
	if (IsKeyPressed(KEY_S) && player.IsAlive() && grenadeCooldown <= 0.0f) {
		ThrowGrenade();
		grenadeCooldown = grenadeDelay;
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
	GrenadesCollision();
}

void Game::BulletsCollision() {
    auto bIt = bullets.begin();
    
    while (bIt != bullets.end()) {
        bool bulletHit = false;
        
        // 1. DAÑO A SOLDADOS (Solo balas tipo 1 y 3)
        if (bIt->GetType() == 1 || bIt->GetType() == 3) {
            auto sIt = soldiers.begin();
            while (sIt != soldiers.end()) {
                // Agrupamos con paréntesis (Tipo 1 O Tipo 3)
                if (sIt->GetisAlive() && CheckCollisionRecs(sIt->GetHurtBox(), bIt->GetHitbox())) {
                    sIt->TriggerDeath();
                    UiManager.AddScore(100);
                    bulletHit = true;
                    break; // La bala impactó, salimos del loop de soldados
                }
                ++sIt;
            }
        }

        // 2. DAÑO AL JUGADOR (Solo bala tipo 2 - Granada/Explosión)
        if (bIt->GetType() == 2 && bIt->IsExploding()) {
            if (player.IsAlive() && !player.IsInvincible()) {
                if (CheckCollisionRecs(bIt->GetHitbox(), player.GetHitBox())) {
                    player.TakeDamage();
                    TraceLog(LOG_INFO, "Player hit by explosion");
                    // Nota: No ponemos bulletHit = true aquí porque la explosión 
                    // suele ser un área persistente que se borra por animación, no por impacto.
                }
            }
        }


		// ← AQUÍ, antes del if(bulletHit), colisión con cajas
		if (!bulletHit) {
			for (auto& item : items) {
				if (item.IsActive() && item.GetType() == ItemType::BOX) {
					if (CheckCollisionRecs(bIt->GetHitbox(), item.GetHitBox()) && bIt->GetType() == 1 || bIt->GetType() == 3) {
						item.Destroy();
						bulletHit = true;
						break;
					}
				}
			}
		}

        // 3. LIMPIEZA DE BALAS
        if (bulletHit) {
            bIt = bullets.erase(bIt); // Borrar bala tipo 1 o 3 tras impactar
        } else {
            ++bIt;
        }
    }

    // 4. LIMPIEZA DE SOLDADOS MUERTOS
    auto sIt = soldiers.begin();
    while (sIt != soldiers.end()) {
        if (!sIt->GetisAlive() && sIt->IsDeadAnimFinished()) {
            sIt = soldiers.erase(sIt);
        } else {
            ++sIt;
        }
    }
}

void Game::GrenadesCollision() {
	for (auto& grenade : grenades) {
		if (grenade.HasExploded()) {
			Rectangle explosionBox = grenade.GetExplosionHitBox();

			for (auto& soldier : soldiers) {
				if (soldier.GetisAlive() && CheckCollisionRecs(soldier.GetHurtBox(), explosionBox)) {
					soldier.TriggerDeath();
					UiManager.AddScore(100);
					TraceLog(LOG_INFO, "Soldier killed by grenade explosion");
				}
			}
		}
	}
}
void Game::SoldierBlockCollision()
{
	for (auto& soldier : soldiers) {
		soldier.SetLeftCollision(false);  // ← resetear laterales aquí
		soldier.SetRightCollision(false);

		float distToPlayer = abs(player.GetPosition().x - soldier.GetPosition().x);
		bool playerNearby = distToPlayer < 800.0f; // ← distancia absoluta

		if (!playerNearby) {
			// Soldado lejano: solo aplicar gravedad, no mover ni colisionar lateralmente
			soldier.SetGrounded(false);
			// Chequear solo suelo para que no caiga al vacío
			for (const auto& block : blocks) {
				if (!block.IsGround()) continue;
				Rectangle blockRect = block.GetRect();
				Rectangle hurtBox = soldier.GetHurtBox();
				float feetY = hurtBox.y + hurtBox.height;
				float blockTopY = blockRect.y;
				float verticalDist = feetY - blockTopY;
				bool isOverBlock = (hurtBox.x + hurtBox.width > blockRect.x &&
					hurtBox.x < blockRect.x + blockRect.width);
				if (isOverBlock && verticalDist >= 0 && verticalDist <= 20.0f && soldier.GetVelocityY() >= 0) {
					soldier.SetY(blockTopY - soldier.GetHeight());
					soldier.SetVelocityY(0);
					soldier.SetGrounded(true);
				}
			}
			continue; // ← saltar colisiones laterales si está lejos
		}

		// ========== SOLDADO CERCANO ==========
		soldier.SetGrounded(false);

		for (const auto& block : blocks) {
			if (!block.IsGround()) continue;
			Rectangle blockRect = block.GetRect();

			if (blockRect.x > soldier.GetPosition().x + 300.0f ||
				blockRect.x + blockRect.width < soldier.GetPosition().x - 300.0f) continue;

			Rectangle hurtBox = soldier.GetHurtBox();

			// SUELO
			float feetY = hurtBox.y + hurtBox.height;
			float blockTopY = blockRect.y;
			float verticalDist = feetY - blockTopY;
			bool isOverBlock = (hurtBox.x + hurtBox.width > blockRect.x &&
				hurtBox.x < blockRect.x + blockRect.width);

			if (isOverBlock && verticalDist >= 0 && verticalDist <= 20.0f && soldier.GetVelocityY() >= 0) {
				soldier.SetY(blockTopY - soldier.GetHeight());
				soldier.SetVelocityY(0);
				soldier.SetGrounded(true);
			}

			// LATERALES
			bool verticalOverlap = (hurtBox.y + hurtBox.height > blockRect.y + 5.0f &&
				hurtBox.y < blockRect.y + blockRect.height - 5.0f);
			if (verticalOverlap) {
				if (CheckCollisionRecs(soldier.GetLeftHitBox(), blockRect)) {
					soldier.SetLeftCollision(true);
				}
				if (CheckCollisionRecs(soldier.GetRightHitBox(), blockRect)) {
					soldier.SetRightCollision(true);
				}
			}
		}
	}
}

		
void Game::BlockCollisions() {
	bool onGround = false;
	const float GROUND_TOLERANCE = 5.0f;

	player.SetLeftCollision(false);
	player.SetRightCollision(false);

	Rectangle playerRect = player.GetHitBox(); // ← calcular UNA vez fuera del loop

	for (const auto& block : blocks) {
		Rectangle blockRect = block.GetRect();

		// ========== SKIP BLOQUES LEJANOS ==========
		if (blockRect.x > playerRect.x + 400.0f || blockRect.x + blockRect.width < playerRect.x - 400.0f) continue;

		// ========== COLISIÓN CON TECHO ==========
		if (block.GetType() == BlockType::CEILING) {
			if (CheckCollisionRecs(playerRect, blockRect)) {
				if (playerRect.y < blockRect.y + blockRect.height && player.GetVelocityY() < 0) {
					player.SetY(blockRect.y + blockRect.height);
					player.SetVelocityY(0);
				}
			}
			continue;
		}

		// ========== COLISIÓN CON RAMPAS ==========
		if (block.IsRamp()) {
			float playerCenterX = playerRect.x + playerRect.width / 2;
			if (playerCenterX >= blockRect.x && playerCenterX <= blockRect.x + blockRect.width) {
				float rampHeight = block.GetHeightAtX(playerCenterX);
				float playerFeetY = playerRect.y + playerRect.height;
				if (playerFeetY >= rampHeight && playerFeetY <= rampHeight + 20.0f) {
					player.SetY(rampHeight - playerRect.height);
					player.SetVelocityY(0);
					onGround = true;
				}
			}
			continue;
		}

		// ========== COLISIÓN SOLDADOS ==========
		SoldierBlockCollision();

		// ========== COLISIÓN JUGADOR SUELO ==========
		float feetY = playerRect.y + playerRect.height;
		float blockTopY = blockRect.y;
		float previousFeetY = player.GetPreviousY() + player.GetHeight();
		bool wasBelow = (player.GetVelocityY() < 0 && previousFeetY <= blockTopY);
		float verticalDistance = feetY - blockTopY;
		bool isOverBlock = (playerRect.x + playerRect.width > blockRect.x + GROUND_TOLERANCE &&
			playerRect.x < blockRect.x + blockRect.width - GROUND_TOLERANCE);

		if (isOverBlock && verticalDistance >= 0 && verticalDistance <= 20.0f &&
			player.GetVelocityY() >= 0 && !wasBelow) {
			player.SetY(blockTopY - playerRect.height);
			player.SetVelocityY(0);
			onGround = true;
		}

		// ========== COLISIONES LATERALES ==========
		if (block.IsGround()) {
			Rectangle leftHitBox = player.GetLeftHitBox();
			if (CheckCollisionRecs(leftHitBox, blockRect)) {
				float overlap = (blockRect.x + blockRect.width) - leftHitBox.x;
				player.SetX(player.GetX() + overlap);
				player.SetLeftCollision(true);
			}
			Rectangle rightHitBox = player.GetRightHitBox();
			if (CheckCollisionRecs(rightHitBox, blockRect)) {
				float newX = blockRect.x - player.GetWidth();
				if (player.GetX() + player.GetWidth() > blockRect.x) {
					player.SetX(newX);
					player.SetRightCollision(true);
				}
				else {
					player.SetRightCollision(true);
				}
			}
		}
	}

	// ========== ITEMS CON SUELO ==========
	for (auto& item : items) {
		if (!item.IsGrounded() && item.GetType() == ItemType::SHOTGUN) {
			Rectangle itemRect = item.GetHitBox();
			for (const auto& block : blocks) {
				Rectangle blockRect = block.GetRect();
				float feetY = itemRect.y + itemRect.height;
				float blockTopY = blockRect.y;
				bool isOver = (itemRect.x + itemRect.width > blockRect.x &&
					itemRect.x < blockRect.x + blockRect.width);
				if (isOver && feetY >= blockTopY && feetY <= blockTopY + 20.0f) {
					item.SetGrounded(true);
					item.SetGravity(false);
				}
			}
		}
	}

	player.SetGrounded(onGround);
	if (onGround) player.SetVelocityY(0);

	// ========== BALAS CON BLOQUES (UNA SOLA VEZ, FUERA DEL LOOP) ==========
	auto bIt = bullets.begin();
	while (bIt != bullets.end()) {
		bool bulletJustHit = false;

		if (!bIt->IsExploding()) {
			// Tipo 2 (granada): solo colisionar cuando va hacia abajo
			if (bIt->GetType() == 2 && bIt->GetDirectionY() > 0) {
				for (const auto& block : blocks) {
					if (CheckCollisionRecs(bIt->GetHitbox(), block.GetRect())) {
						bIt->SetPosition({ bIt->GetPosition().x, block.GetRect().y - bIt->GetHeight() });
						bulletJustHit = true;
						break;
					}
				}
			}
			// Tipo 1 y 3: solo con bloques sólidos normales
			else if (bIt->GetType() == 1 || bIt->GetType() == 3) {
				for (const auto& block : blocks) {
					if (block.GetType() == BlockType::NORMAL && block.IsGround() &&
						CheckCollisionRecs(bIt->GetHitbox(), block.GetRect())) {
						bulletJustHit = true;
						break;
					}
				}
			}
		}

		if (bulletJustHit) {
			if (bIt->GetType() == 1 || bIt->GetType() == 3) {
				bIt = bullets.erase(bIt);
				continue;
			}
			else if (bIt->GetType() == 2) {
				bIt->SetExploding(true);
				bIt->GetAnim().SetAnimation(BulletState::EXPLOSIONSOLDIER);
			}
		}

		if (bIt->GetType() == 2 && bIt->IsExploding()) {
			if (bIt->GetAnim().IsAnimationFinished()) {
				bIt = bullets.erase(bIt);
				continue;
			}
		}

		++bIt;
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
		soldiers.reserve(20);
		soldiers.emplace_back(Soldier(2, { 1000, 200 }));
		soldiers.emplace_back(Soldier(1, { 1000, 200 }));
		soldiers.emplace_back(Soldier(1, { 2000, 200 }));
		soldiers.emplace_back(Soldier(1, { 3000, 200 }));
		soldiers.emplace_back(Soldier(1, { 4000, 200 }));
		soldiers.emplace_back(Soldier(1, { 5000, 200 }));

		return soldiers;
	}

std::vector<Block> Game::CreateBlocks()
{
	std::vector<Block> blocks;
	return blocks;
}

void Game::SaveBlocksToFile(const char* filename) {
	FILE* file;
	fopen_s(&file, filename, "w");
	if (!file) return;

	for (const auto& block : blocks) {
		Rectangle rect = block.GetRect();
		int typeValue = 0;
		if (block.GetType() == BlockType::RAMP_UP) typeValue = 2;
		else if (block.GetType() == BlockType::RAMP_DOWN) typeValue = 3;
		else if (block.GetType() == BlockType::CEILING) typeValue = 4;
		else typeValue = block.IsGround() ? 1 : 0;

		fprintf(file, "%.0f,%.0f,%.0f,%.0f,%d\n",
			rect.x, rect.y, rect.width, rect.height, typeValue);
	}
	fclose(file);
}

void Game::LoadBlocksFromFile(const char* filename) {
	blocks.clear();
	FILE* file;
	fopen_s(&file, filename, "r");
	if (!file) {
		TraceLog(LOG_WARNING, "No se pudo cargar el archivo %s", filename);
		return;
	}

	float x, y, w, h;
	int typeValue;
	while (fscanf_s(file, "%f,%f,%f,%f,%d\n", &x, &y, &w, &h, &typeValue) == 5) {
		if (typeValue == 2) {
			blocks.emplace_back(x, y, w, h, BlockType::RAMP_UP);
		}
		else if (typeValue == 3) {
			blocks.emplace_back(x, y, w, h, BlockType::RAMP_DOWN);
		}
		else if (typeValue == 4) { 
			blocks.emplace_back(x, y, w, h, BlockType::CEILING);
		}
		else {
			blocks.emplace_back(x, y, w, h, typeValue == 1);
		}
	}

	fclose(file);
	TraceLog(LOG_INFO, "Bloques cargados desde %s (%d bloques)", filename, (int)blocks.size());
}

std::vector<Item> Game::CreateItems() {
	std::vector<Item> items;
	items.emplace_back(Vector2{ 300.0f, 560.0f }, ItemType::BOX); 
	return items;
}

void Game::ShootMachinegun(float yOffset) {
	Vector2 pPos = player.GetPosition();
	float pW = player.GetWidth();
	float pH = player.GetHeight();
	PlayerDirection aimDir = player.GetAimDirection();
	float baseY = pPos.y + pH / 2 - 50.0f + yOffset;

	Vector2 bulletPos;
	float dirX = 0, dirY = 0;

	switch (aimDir) {
	case PlayerDirection::LEFT:
		bulletPos = { pPos.x - 120.0f, baseY };  // ajusta el valor
		dirX = -1.0f;
		break;
	case PlayerDirection::RIGHT:
		bulletPos = { pPos.x + pW + 70.0f, baseY };  // ajusta el valor
		dirX = 1.0f;
		break;
	case PlayerDirection::UP:
		bulletPos = { pPos.x + pW / 2 + yOffset - 30.0f, pPos.y - 150.0f - 30.0f };  // ajusta los dos valores
		dirY = -1.0f;
		break;
	default:
		bulletPos = { pPos.x + pW, baseY };
		dirX = 1.0f;
		break;
	}

	bullets.emplace_back(bulletPos, 1000.0f, dirX, dirY, 3);
}
