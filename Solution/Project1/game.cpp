#define _CRT_SECURE_NO_WARNINGS
#include "game.hpp"
#include "LevelMap.hpp" 
#include <raymath.h>


bool musicStarted = false;
Color BGCOLOR = { 195, 195, 170 };
Game::Game() : camera({ 1200.0f / 2 , 896 / 2 })
{
	FILE* file = fopen("level_blocks.txt", "r");
	if (file) {
		fclose(file);
		LoadBlocksFromFile("level_blocks.txt");
	}
	else {
		blocks = CreateBlocks();
		items = CreateItems();
	}
	bullets = CreateBullets();

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
		for (float x = fmod(gridOffset.x, gridSize); x < (float)GetScreenWidth(); x += gridSize) {
			DrawLineV({ x, 0 }, { x, (float)GetScreenHeight() }, GRAY);
		}
		for (float y = fmod(gridOffset.y, gridSize); y < (float)GetScreenHeight(); y += gridSize) {
			DrawLineV({ 0, y }, { (float)GetScreenWidth(), y }, GRAY);
		}

		DrawText("EDITOR MODE - F1:Salir | Click:Suelo | Right:Plataforma | Mid:Borrar | R:RampaUP | T:RampaDOWN | Y:Techo | 1:Soldado1 | 2:Soldado2 | B:Caja | G:MachinegunItem | F5:Guardar",
			10, 10, 12, RED);

		Vector2 mousePos = GetMousePosition();
		Vector2 worldPos = camera.GetScreenToWorld(mousePos);
		DrawText(TextFormat("World: (%.0f, %.0f)", worldPos.x, worldPos.y), 10, 30, 15, YELLOW);
	}
}

void Game::Update() {

	if (sceneManager.GetGamestate() == SceneManager::INTRO) {
		BeginDrawing();
		ClearBackground(BLACK);
		sceneManager.DrawTexts();
		EndDrawing();
		return;
	}
	else if (sceneManager.GetGamestate() == SceneManager::TITLE) {
		BeginDrawing();
		ClearBackground(BLACK);
		sceneManager.DrawTexts();
		if (!musicStarted) {
			audioManager.PlayMusic(audioManager.GetTitleMusic());
			musicStarted = true;
		}
		audioManager.UpdateMusic(audioManager.GetTitleMusic());
	}
	else if (sceneManager.GetGamestate() == SceneManager::GAME) {
		UiManager.Update();

		if (!player.IsAlive()) {
			BeginDrawing();
			Vector2 deathPos = player.GetDeathPosition();
			DrawText(TextFormat("YOU DIED at (%.0f, %.0f)", deathPos.x, deathPos.y),
				GetScreenWidth() / 2 - 200, GetScreenHeight() / 2 - 50, 20, RED);
			DrawText("Press R to respawn at death position",
				GetScreenWidth() / 2 - 200, GetScreenHeight() / 2, 20, WHITE);
			if (IsKeyPressed(KEY_R)) {
				player.Respawn();
			}
		}

		player.SavePreviousPosition();
		HandleInput();
		player.Update(camera.GetLeftLimit());
		ResolveCollisions();

		for (auto& item : items) {
			item.Update();

			if (item.IsActive() && CheckCollisionRecs(item.GetHitBox(), player.GetHitBox())) {
				if (item.GetType() == ItemType::SHOTGUN) {
					player.EquipMachinegun();
					item.Collect();
				}
			}

			if (item.ShouldSpawnMachinegun()) {
				item.ConsumeSpawn();
				Vector2 spawnPos = { item.GetHitBox().x + 60.0f, item.GetHitBox().y - 20.0f };
				Item newItem(spawnPos, ItemType::SHOTGUN);
				newItem.SetGravity(true);
				items.push_back(newItem);
			}
		}

		items.erase(std::remove_if(items.begin(), items.end(),
			[](const Item& i) { return !i.IsActive(); }), items.end());

		camera.Update(player.GetPosition(), backgroundManager.GetWidth(), backgroundManager.GetHeight(), player.GetIsGrounded());

		for (auto& soldier : soldiers) {
			soldier.UpdateAI(player);
			soldier.Update();
			if (soldier.WantsToShoot()) {
				Shoot(2, soldier.GetPosition(), soldier.IsFacingRight());
				soldier.ResetShootWants();
			}
		}

		for (auto& bullet : bullets) {
			bullet.Update();
		}
		CheckBulletsOutOfCamera();


		for (auto& grenade : grenades) {
			grenade.Update();
			grenade.CheckCollisionWithBlocks(blocks);
			grenade.CheckCollisionWithSoldiers(soldiers);
		}

		if (grenadeCooldown > 0.0f) grenadeCooldown -= GetFrameTime();
		if (shootTimer > 0.0f) shootTimer -= GetFrameTime();

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
					if (!IsKeyDown(KEY_UP)) {
						player.StopAimingUp();
					}
				}
			}
		}

		BeginDrawing();
		ClearBackground(BGCOLOR);
		Draw();
		backgroundManager.FollowPlayer(camera.GetCamera().target);
		backgroundManager.Update(GetFrameTime());

		if (!musicStarted) {
			audioManager.PlayMusic(audioManager.GetGameMusic());
			musicStarted = true;
		}
		audioManager.UpdateMusic(audioManager.GetGameMusic());
	}
}

void Game::Shoot(int BulletType, Vector2 startPos, bool faceRight)
{
	Vector2 bulletPos = { 0.0f, 0.0f };
	float directionX = 0.0f;
	float directionY = 0.0f;
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
			bulletPos = { pPos.x, pPos.y + pH / 2.0f + (isCrouching ? crouchYOffset : normalYOffset) };
			directionX = -1.0f;
			break;
		case PlayerDirection::RIGHT:
			bulletPos = { pPos.x + pW, pPos.y + pH / 2.0f + (isCrouching ? crouchYOffset : normalYOffset) };
			directionX = 1.0f;
			break;
		case PlayerDirection::UP:
			bulletPos = { pPos.x + pW / 2.0f, pPos.y + yOffset };
			directionY = -1.0f;
			break;
		case PlayerDirection::DOWN:
			bulletPos = { pPos.x + pW / 2.0f, pPos.y + pH };
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

	bullets.emplace_back(bulletPos, (int)bulletSpeed, (int)directionX, (int)directionY, BulletType);
}

void Game::ThrowGrenade() {
	GrenadeThrowData data = player.ThrowGrenade();
	if (data.valid) {
		grenades.emplace_back(data.startPos, data.initialVelocity);
	}
}

void Game::HandleInput()
{
	if (!player.IsAlive()) {
		if (IsKeyPressed(KEY_R)) {
			player.Respawn();
		}
		return;
	}

	// ========== CAMBIO DE ESCENA ==========
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

	// ========== MODO EDITOR ==========
	static float f1Cooldown = 0.0f;
	if (IsKeyPressed(KEY_F1) && f1Cooldown <= 0.0f) {
		editorMode = !editorMode;
		f1Cooldown = 0.2f;
		TraceLog(LOG_INFO, "Editor mode: %s", editorMode ? "ON" : "OFF");
	}
	if (f1Cooldown > 0.0f) f1Cooldown -= GetFrameTime();

	if (editorMode) {
		if (IsKeyDown(KEY_W)) gridOffset.y -= 5.0f;
		if (IsKeyDown(KEY_S)) gridOffset.y += 5.0f;
		if (IsKeyDown(KEY_A)) gridOffset.x -= 5.0f;
		if (IsKeyDown(KEY_D)) gridOffset.x += 5.0f;

		Vector2 mousePos = GetMousePosition();
		Vector2 worldPos = camera.GetScreenToWorld(mousePos);

		int tileX = (int)floor((worldPos.x - gridOffset.x) / gridSize);
		int tileY = (int)floor((worldPos.y - gridOffset.y) / gridSize);

		float blockX = gridOffset.x + tileX * gridSize;
		float blockY = gridOffset.y + tileY * gridSize;

		if (IsKeyPressed(KEY_Y)) {
			blocks.emplace_back(blockX, blockY, gridSize, gridSize, BlockType::CEILING);
			TraceLog(LOG_INFO, "Techo creado en (%.0f, %.0f)", blockX, blockY);
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			blocks.emplace_back(blockX, blockY, gridSize, gridSize, true);
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
			blocks.emplace_back(blockX, blockY, gridSize, gridSize, false);
		}
		if (IsKeyPressed(KEY_R)) {
			blocks.emplace_back(blockX, blockY, gridSize, gridSize, BlockType::RAMP_UP);
			TraceLog(LOG_INFO, "Rampa UP creada en (%.0f, %.0f)", blockX, blockY);
		}
		if (IsKeyPressed(KEY_T)) {
			blocks.emplace_back(blockX, blockY, gridSize, gridSize, BlockType::RAMP_DOWN);
			TraceLog(LOG_INFO, "Rampa DOWN creada en (%.0f, %.0f)", blockX, blockY);
		}
		static float soldierSpawnCooldown = 0.0f;
		if (soldierSpawnCooldown > 0.0f) soldierSpawnCooldown -= GetFrameTime();

		if (IsKeyPressed(KEY_ONE) && soldierSpawnCooldown <= 0.0f) {
			soldiers.emplace_back(1, worldPos);
			soldierSpawnCooldown = 0.3f;
			TraceLog(LOG_INFO, "Soldado tipo 1 en (%.0f, %.0f)", worldPos.x, worldPos.y);
		}
		else if (IsKeyPressed(KEY_TWO) && soldierSpawnCooldown <= 0.0f) {
			soldiers.emplace_back(2, worldPos);
			soldierSpawnCooldown = 0.3f;
			TraceLog(LOG_INFO, "Soldado tipo 2 en (%.0f, %.0f)", worldPos.x, worldPos.y);
		}
		static float itemSpawnCooldown = 0.0f;
		if (itemSpawnCooldown > 0.0f) itemSpawnCooldown -= GetFrameTime();

		if (IsKeyPressed(KEY_B) && itemSpawnCooldown <= 0.0f) {
			items.emplace_back(worldPos, ItemType::BOX);
			itemSpawnCooldown = 0.3f;
			TraceLog(LOG_INFO, "Caja en (%.0f, %.0f)", worldPos.x, worldPos.y);
		}
		if (IsKeyPressed(KEY_G) && itemSpawnCooldown <= 0.0f) {
			items.emplace_back(worldPos, ItemType::SHOTGUN);
			itemSpawnCooldown = 0.3f;
			TraceLog(LOG_INFO, "Machinegun item en (%.0f, %.0f)", worldPos.x, worldPos.y);
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
			// Borrar bloque
			float bX = blockX;
			float bY = blockY;
			auto it = std::remove_if(blocks.begin(), blocks.end(),
				[bX, bY](const Block& b) {
					return b.GetRect().x == bX && b.GetRect().y == bY;
				});
			blocks.erase(it, blocks.end());

			// Borrar item
			auto iIt = std::remove_if(items.begin(), items.end(),
				[&worldPos](const Item& i) {
					return CheckCollisionPointRec(worldPos, i.GetHitBox());
				});
			items.erase(iIt, items.end());
		}
		if (IsKeyPressed(KEY_F5)) {
			SaveBlocksToFile("level_blocks.txt");
		}
	}

	// ========== SI ESTÁ CAYENDO, NO PROCESAR INPUT DEL JUGADOR ==========
	if (player.IsFalling()) return;

	// ========== DEBUG ==========
	if (IsKeyPressed(KEY_L)) UiManager.NextLevel();
	if (IsKeyPressed(KEY_J)) UiManager.AddScore(100);
	if (IsKeyPressed(KEY_C)) {
		if (UiManager.GetCredits() < 99) UiManager.SetCredits(1);
	}

	// ========== MOVIMIENTO ==========
	if (machinegunBurst) {
		if (machinegunBurstDir == PlayerDirection::LEFT) {
			if (IsKeyDown(KEY_LEFT)) player.MoveLeft();
			else player.StopMovingHorizontal();
		}
		else if (machinegunBurstDir == PlayerDirection::RIGHT) {
			if (IsKeyDown(KEY_RIGHT)) player.MoveRight();
			else player.StopMovingHorizontal();
		}
		if (IsKeyPressed(KEY_SPACE)) player.Jump();
		if (IsKeyDown(KEY_DOWN)) player.StartCrouching();
		else player.StopCrouching();
		return;
	}

	if (IsKeyDown(KEY_LEFT)) player.MoveLeft();
	else if (IsKeyDown(KEY_RIGHT)) player.MoveRight();
	else player.StopMovingHorizontal();

	if (IsKeyPressed(KEY_UP)) player.StartAimingUp();
	if (IsKeyReleased(KEY_UP)) player.StopAimingUp();

	if (IsKeyDown(KEY_DOWN)) player.StartCrouching();
	else player.StopCrouching();

	if (IsKeyPressed(KEY_SPACE)) player.Jump();

	if (IsKeyPressed(KEY_D) && shootTimer <= 0.0f) {
		if (player.GetCurrentWeapon() == WeaponType::MACHINEGUN) {
			if (player.GetAmmo() > 0) {
				player.Shoot();
				machinegunBurst = true;
				machinegunBurstCount = 0;
				machinegunBurstTimer = 0.0f;
				shootTimer = shootDelayMachinegun;
				machinegunBurstDir = (player.GetAimDirection() == PlayerDirection::UP)
					? PlayerDirection::UP
					: (IsKeyDown(KEY_LEFT) ? PlayerDirection::LEFT : PlayerDirection::RIGHT);
			}
		}
		else {
			player.Shoot();
			Shoot(1, { 0.0f, 0.0f }, true);
			shootTimer = shootDelayPistol;
		}
	}

	if (IsKeyPressed(KEY_S) && player.IsAlive() && grenadeCooldown <= 0.0f) {
		ThrowGrenade();
		grenadeCooldown = grenadeDelay;
	}
}

void Game::ResolveCollisions() {
	Rectangle playerHB = player.GetHitBox();
	bool onGround = false;

	for (const auto& block : blocks) {
		Rectangle blockRect = block.GetRect();
		if (CheckCollisionRecs(playerHB, blockRect)) {
			if (player.GetVelocityY() >= 0 &&
				(player.GetY() + player.GetHeight() - player.GetVelocityY() <= blockRect.y + 5.0f)) {
				player.SetY(blockRect.y - player.GetHeight());
				player.SetVelocityY(0);
				onGround = true;
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

		// 1. DAÑO A SOLDADOS (tipo 1 y 3)
		if (bIt->GetType() == 1 || bIt->GetType() == 3) {
			auto sIt = soldiers.begin();
			while (sIt != soldiers.end()) {
				if (sIt->GetisAlive() && CheckCollisionRecs(sIt->GetHurtBox(), bIt->GetHitbox())) {
					sIt->TriggerDeath();
					UiManager.AddScore(100);
					bulletHit = true;
					break;
				}
				++sIt;
			}
		}

		// 2. DAÑO AL JUGADOR (tipo 2 - explosión)
		if (bIt->GetType() == 2 && bIt->IsExploding()) {
			if (player.IsAlive() && !player.IsInvincible()) {
				if (CheckCollisionRecs(bIt->GetHitbox(), player.GetHitBox())) {
					player.TakeDamage();
					TraceLog(LOG_INFO, "Player hit by explosion");
				}
			}
		}

		// 3. COLISIÓN CON CAJAS
		if (!bulletHit) {
			for (auto& item : items) {
				if (item.IsActive() && item.GetType() == ItemType::BOX) {
					if (CheckCollisionRecs(bIt->GetHitbox(), item.GetHitBox()) &&
						(bIt->GetType() == 1 || bIt->GetType() == 3)) {
						item.Destroy();
						bulletHit = true;
						break;
					}
				}
			}
		}

		if (bulletHit) {
			bIt = bullets.erase(bIt);
		}
		else {
			++bIt;
		}
	}

	// 4. LIMPIEZA DE SOLDADOS MUERTOS
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
		soldier.SetLeftCollision(false);
		soldier.SetRightCollision(false);

		float distToPlayer = fabsf(player.GetPosition().x - soldier.GetPosition().x);
		bool playerNearby = distToPlayer < 800.0f;

		if (!playerNearby) {
			soldier.SetGrounded(false);
			for (const auto& block : blocks) {
				if (block.IsRamp() || block.GetType() == BlockType::CEILING) continue;
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
			continue;
		}

		soldier.SetGrounded(false);

		for (const auto& block : blocks) {
			if (block.IsRamp() || block.GetType() == BlockType::CEILING) continue;
			Rectangle blockRect = block.GetRect();

			if (blockRect.x > soldier.GetPosition().x + 300.0f ||
				blockRect.x + blockRect.width < soldier.GetPosition().x - 300.0f) continue;

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

	Rectangle playerRect = player.GetHitBox();

	for (const auto& block : blocks) {
		Rectangle blockRect = block.GetRect();

		if (blockRect.x > playerRect.x + 400.0f || blockRect.x + blockRect.width < playerRect.x - 400.0f) continue;

		// ========== TECHO ==========
		if (block.GetType() == BlockType::CEILING) {
			if (CheckCollisionRecs(playerRect, blockRect)) {
				if (playerRect.y < blockRect.y + blockRect.height && player.GetVelocityY() < 0) {
					player.SetY(blockRect.y + blockRect.height);
					player.SetVelocityY(0);
				}
			}
			continue;
		}

		// ========== RAMPAS ==========
		if (block.IsRamp()) {
			float playerCenterX = playerRect.x + playerRect.width / 2.0f;
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

		// ========== SOLDADOS ==========
		SoldierBlockCollision();

		// ========== JUGADOR SUELO ==========
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

		// ========== LATERALES ==========
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

	// ========== BALAS CON BLOQUES ==========
	auto bIt = bullets.begin();
	while (bIt != bullets.end()) {
		bool bulletJustHit = false;

		if (!bIt->IsExploding()) {
			// Tipo 2 (granada): colisiona con bloques sólidos cuando va hacia abajo
			if (bIt->GetType() == 2 && bIt->GetDirectionY() > 0) {
				for (const auto& block : blocks) {
					if (block.GetType() == BlockType::NORMAL && block.IsGround() &&
						CheckCollisionRecs(bIt->GetHitbox(), block.GetRect())) {
						bulletJustHit = true;
						break;
					}
				}
			}
			// Tipo 1 y 3: solo con bloques sólidos azules
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
	return bullets;
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

	// Bloques con prefijo B
	for (const auto& block : blocks) {
		Rectangle rect = block.GetRect();
		int typeValue = 0;
		if (block.GetType() == BlockType::RAMP_UP) typeValue = 2;
		else if (block.GetType() == BlockType::RAMP_DOWN) typeValue = 3;
		else if (block.GetType() == BlockType::CEILING) typeValue = 4;
		else typeValue = block.IsGround() ? 1 : 0;
		fprintf(file, "B %.0f,%.0f,%.0f,%.0f,%d\n",
			rect.x, rect.y, rect.width, rect.height, typeValue);
	}

	// Soldados con prefijo S
	for (const auto& soldier : soldiers) {
		fprintf(file, "S %.0f,%.0f,%d\n",
			soldier.GetX(), soldier.GetY(), const_cast<Soldier&>(soldier).GetType());
	}

	// Items con prefijo I
	for (const auto& item : items) {
		int itemType = (item.GetType() == ItemType::BOX) ? 1 : 0;
		fprintf(file, "I %.0f,%.0f,%d\n",
			item.GetPosition().x, item.GetPosition().y, itemType);
	}

	fclose(file);
}

void Game::LoadBlocksFromFile(const char* filename) {
	blocks.clear();
	soldiers.clear();
	items.clear();

	FILE* file;
	fopen_s(&file, filename, "r");
	if (!file) {
		TraceLog(LOG_WARNING, "No se pudo cargar el archivo %s", filename);
		return;
	}

	char line[256];
	while (fgets(line, sizeof(line), file)) {
		// Formato nuevo con prefijo
		if (line[0] == 'B') {
			float x, y, w, h;
			int typeValue;
			if (sscanf_s(line + 2, "%f,%f,%f,%f,%d", &x, &y, &w, &h, &typeValue) == 5) {
				if (typeValue == 2) blocks.emplace_back(x, y, w, h, BlockType::RAMP_UP);
				else if (typeValue == 3) blocks.emplace_back(x, y, w, h, BlockType::RAMP_DOWN);
				else if (typeValue == 4) blocks.emplace_back(x, y, w, h, BlockType::CEILING);
				else blocks.emplace_back(x, y, w, h, typeValue == 1);
			}
		}
		else if (line[0] == 'S') {
			float x, y;
			int type;
			if (sscanf_s(line + 2, "%f,%f,%d", &x, &y, &type) == 3) {
				soldiers.emplace_back(type, Vector2{ x, y });
			}
		}
		else if (line[0] == 'I') {
			float x, y;
			int type;
			if (sscanf_s(line + 2, "%f,%f,%d", &x, &y, &type) == 3) {
				items.emplace_back(Vector2{ x, y },
					type == 1 ? ItemType::BOX : ItemType::SHOTGUN);
			}
		}
		else {
			// Formato antiguo sin prefijo
			float x, y, w, h;
			int typeValue;
			if (sscanf_s(line, "%f,%f,%f,%f,%d", &x, &y, &w, &h, &typeValue) == 5) {
				if (typeValue == 2) blocks.emplace_back(x, y, w, h, BlockType::RAMP_UP);
				else if (typeValue == 3) blocks.emplace_back(x, y, w, h, BlockType::RAMP_DOWN);
				else if (typeValue == 4) blocks.emplace_back(x, y, w, h, BlockType::CEILING);
				else blocks.emplace_back(x, y, w, h, typeValue == 1);
			}
		}
	}

	fclose(file);
	TraceLog(LOG_INFO, "Nivel cargado: %d bloques, %d soldados, %d items",
		(int)blocks.size(), (int)soldiers.size(), (int)items.size());
}

std::vector<Item> Game::CreateItems() {
	std::vector<Item> items;
	return items;
}

void Game::ShootMachinegun(float yOffset) {
	Vector2 pPos = player.GetPosition();
	float pW = player.GetWidth();
	float pH = player.GetHeight();
	PlayerDirection aimDir = player.GetAimDirection();
	float baseY = pPos.y + pH / 2.0f - 50.0f + yOffset;

	Vector2 bulletPos;
	float dirX = 0.0f, dirY = 0.0f;

	switch (aimDir) {
	case PlayerDirection::LEFT:
		bulletPos = { pPos.x - 120.0f, baseY };
		dirX = -1.0f;
		break;
	case PlayerDirection::RIGHT:
		bulletPos = { pPos.x + pW + 70.0f, baseY };
		dirX = 1.0f;
		break;
	case PlayerDirection::UP:
		bulletPos = { pPos.x + pW / 2.0f + yOffset - 30.0f, pPos.y - 150.0f - 30.0f };
		dirY = -1.0f;
		break;
	default:
		bulletPos = { pPos.x + pW, baseY };
		dirX = 1.0f;
		break;
	}

	bullets.emplace_back(bulletPos, 1000, (int)dirX, (int)dirY, 3);
}

void Game::CheckBulletsOutOfCamera() {
	// Obtener los límites de la cámara en el mundo
	Camera2D cam = camera.GetCamera();
	float screenWidth = (float)GetScreenWidth();
	float screenHeight = (float)GetScreenHeight();

	// Calcular los bordes de la cámara en coordenadas del mundo
	float leftBound = cam.target.x - screenWidth / 2.0f;
	float rightBound = cam.target.x + screenWidth / 2.0f;
	float topBound = cam.target.y - screenHeight / 2.0f;
	float bottomBound = cam.target.y + screenHeight / 2.0f;

	// Añadir un margen (por ejemplo 100 píxeles) para que no se borren justo en el borde
	float margin = 200.0f;
	leftBound -= margin;
	rightBound += margin;
	topBound -= margin;
	bottomBound += margin;

	// Recorrer las balas y eliminar las que estén fuera
	auto bIt = bullets.begin();
	while (bIt != bullets.end()) {
		Vector2 bulletPos = bIt->GetPosition();

		// Verificar si la bala está fuera de los límites de la cámara
		if (bulletPos.x < leftBound || bulletPos.x > rightBound ||
			bulletPos.y < topBound || bulletPos.y > bottomBound) {

			TraceLog(LOG_INFO, "Bullet removed (out of camera bounds)");
			bIt = bullets.erase(bIt);
		}
		else {
			++bIt;
		}
	}
}