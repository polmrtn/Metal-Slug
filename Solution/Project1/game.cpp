#define _CRT_SECURE_NO_WARNINGS
#include "game.hpp"
#include "LevelMap.hpp" 
#include <raymath.h>
#include <algorithm>
#include "CreationManager.hpp"


bool musicStarted = false;
Color BGCOLOR = { 195, 195, 170 };
Game::Game() : camera({ 1200.0f / 2 , 896 / 2 })
{
	FILE* file = fopen("level_blocks.txt", "r");
	if (file) {
		fclose(file);
		LoadBlocksFromFile("level_blocks.txt");
	}
	

}

Game::~Game()
{
}

void Game::Reset()
{
	player.ResetToStart();
	if (!player.IsAlive()) player.Respawn();
	camera.Reset();
	musicStarted = false;

	FILE* file = fopen("level_blocks.txt", "r");
	if (file) {
		fclose(file);
		LoadBlocksFromFile("level_blocks.txt");
	}
}

void Game::Draw()
{
	camera.Begin();

	backgroundManager.Draw();

	player.Draw();
	for (auto& Soldier : soldiers) {
		Soldier.Draw();
	}
	backgroundManager.Drawfrontground();

	

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

	//if (editorMode) {
	//	for (float x = fmod(gridOffset.x, gridSize); x < (float)GetScreenWidth(); x += gridSize) {
	//		DrawLineV({ x, 0 }, { x, (float)GetScreenHeight() }, GRAY);
	//	}
	//	for (float y = fmod(gridOffset.y, gridSize); y < (float)GetScreenHeight(); y += gridSize) {
	//		DrawLineV({ 0, y }, { (float)GetScreenWidth(), y }, GRAY);
	//	}

	//	DrawText("EDITOR MODE - F1:Salir | Click:Suelo | Right:Plataforma | Mid:Borrar | R:RampaUP | T:RampaDOWN | Y:Techo | 1:Soldado1 | 2:Soldado2 | B:Caja | G:MachinegunItem | F5:Guardar",
	//		10, 10, 12, RED);

	//	Vector2 pPos = player.GetPosition();
	//	DrawText(TextFormat("Player: (%.0f, %.0f)", pPos.x, pPos.y),
	//		10, 50, 15, GREEN);

	//	Vector2 mousePos = GetMousePosition();
	//	Vector2 worldPos = camera.GetScreenToWorld(mousePos);
	//	DrawText(TextFormat("World: (%.0f, %.0f)", worldPos.x, worldPos.y), 10, 30, 15, YELLOW);
	//}
}

void Game::Update() {

	if (sceneManager.GetGamestate() == SceneManager::INTRO) {
		BeginDrawing();
		ClearBackground(BLACK);
		sceneManager.DrawTexts();
	}
	else if (sceneManager.GetGamestate() == SceneManager::TITLE) {
		BeginDrawing();
		ClearBackground(BLACK);
		sceneManager.DrawTexts();

		// ========== INSERTAR CRÉDITO EN MENÚ ==========
		if (IsKeyPressed(KEY_C) && creditCooldown <= 0.0f) {
			if (UiManager.GetCredits() < 99) {
				UiManager.SetCredits(1);
				creditCooldown = creditDelay;
				TraceLog(LOG_INFO, "Crédito insertado en menú. Total: %d", UiManager.GetCredits());
			}
		}

		// Actualizar cooldown de crédito
		if (creditCooldown > 0.0f) {
			creditCooldown -= GetFrameTime();
		}

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

			if (UiManager.GetCredits() > 0) {
				DrawText("Press R to respawn (costs 1 credit)",
					GetScreenWidth() / 2 - 200, GetScreenHeight() / 2, 20, WHITE);
			}
			else {
				DrawText("NO CREDITS! Press C to insert coin",
					GetScreenWidth() / 2 - 200, GetScreenHeight() / 2, 20, RED);
			}

			if (IsKeyPressed(KEY_R) && UiManager.GetCredits() > 0) {
				UiManager.SetCredits(-1);  // Gasta 1 crédito
				player.Respawn();
				TraceLog(LOG_INFO, "Respawn. Créditos restantes: %d", UiManager.GetCredits());
			}
			if (IsKeyPressed(KEY_C) && creditCooldown <= 0.0f) {
				if (UiManager.GetCredits() < 99) {
					UiManager.SetCredits(1);
					creditCooldown = creditDelay;
					TraceLog(LOG_INFO, "Crédito insertado mientras muerto. Total: %d", UiManager.GetCredits());
				}
			}
		}

		player.SavePreviousPosition();
		HandleInput();
		player.Update(camera.GetLeftLimit());
		ResolveCollisions();

		// ===== WIN ZONE =====
		Rectangle winZone = { 16190.0f, -9999.0f, 200.0f, 99999.0f }; 
		if (CheckCollisionRecs(player.GetHitBox(), winZone))
		{
			audioManager.StopMusic(audioManager.GetGameMusic());
			musicStarted = false;
			shouldRestart = true;
			sceneManager.SetGameState(SceneManager::TITLE);
		}

		for (auto& item : items) {
			item.Update();
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
		if (creditCooldown > 0.0f) creditCooldown -= GetFrameTime();

		grenades.erase(std::remove_if(grenades.begin(), grenades.end(),
			[](const Grenade& g) { return !g.IsActive(); }), grenades.end());

		// ========== RÁFAGA MACHINEGUN (strzaly) ==========
		if (machinegunBurst) {
			machinegunBurstTimer += GetFrameTime();
			if (machinegunBurstTimer >= machinegunBurstDelay) {
				machinegunBurstTimer = 0.0f;
				if (player.GetAmmo() > 0) {
					ShootMachinegun(burstOffsets[machinegunBurstCount]);
					player.UseAmmo();
					UiManager.UseAmmo(); // sync HUD ammo counter
				}
				machinegunBurstCount++;
				if (machinegunBurstCount >= MACHINEGUN_BURST_SIZE || player.GetAmmo() <= 0) {
					machinegunBurst = false;
					machinegunBurstCount = 0;
					if (!IsKeyDown(KEY_UP)) {
						player.StopAimingUp();
					}
					// ===== JEZELI BRAK AMMO - WRÓC DO PISTOLETU =====
					if (player.GetAmmo() <= 0) {
						UiManager.SetAmmo(0);
						UiManager.SetWeaponDisplay(UiManager::WeaponDisplay::PISTOL);
					}
				}
			}
		}

		// ========== DZWIEKI MACHINEGUN (4 dzwieki co 0.25s, osobny timer) ==========
		if (machinegunSoundActive) {
			machinegunSoundTimer += GetFrameTime();
			if (machinegunSoundTimer >= MACHINEGUN_SOUND_DELAY) {
				machinegunSoundTimer = 0.0f;
				audioManager.PlaySound(audioManager.GetMachinegunShootSound());
				machinegunSoundCount++;
				if (machinegunSoundCount >= MACHINEGUN_SOUND_SHOTS) {
					machinegunSoundActive = false;
					machinegunSoundCount = 0;
				}
			}
		}

		BeginDrawing();
		ClearBackground(BGCOLOR);
		Draw();
		//Timers();
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
		if (IsKeyPressed(KEY_R) && UiManager.GetCredits() > 0) {
			UiManager.SetCredits(-1);
			player.Respawn();
		}
		return;
	}

	// ========== CAMBIO DE ESCENA ==========
	if (IsKeyPressed(KEY_ENTER)) {
		if (sceneManager.currentState == SceneManager::TITLE) {
			// Solo iniciar si hay créditos
			if (UiManager.GetCredits() > 0) {
				UiManager.SetCredits(-1);  // Gasta 1 crédito
				audioManager.StopMusic(audioManager.GetTitleMusic());
				audioManager.PlaySound(audioManager.GetGameSound());
				sceneManager.SetGameState(SceneManager::GAME);
				musicStarted = false;
			}
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
			float bX = blockX;
			float bY = blockY;
			auto it = std::remove_if(blocks.begin(), blocks.end(),
				[bX, bY](const Block& b) {
					return b.GetRect().x == bX && b.GetRect().y == bY;
				});
			blocks.erase(it, blocks.end());

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
	if (IsKeyPressed(KEY_C) && creditCooldown <= 0.0f) {
		if (UiManager.GetCredits() < 99) {
			UiManager.SetCredits(1); 
			creditCooldown = creditDelay;
		}
	}

	// ========== MOVIMIENTO ==========
	if (machinegunBurst) {
		if (machinegunBurstDir == PlayerDirection::LEFT) {
			if (IsKeyDown(KEY_LEFT)) { player.MoveLeft(); UiManager.NotifyPlayerMoved(); }
			else player.StopMovingHorizontal();
		}
		else if (machinegunBurstDir == PlayerDirection::RIGHT) {
			if (IsKeyDown(KEY_RIGHT)) { player.MoveRight(); UiManager.NotifyPlayerMoved(); }
			else player.StopMovingHorizontal();
		}
		if (IsKeyPressed(KEY_SPACE)) { player.Jump(); UiManager.NotifyPlayerMoved(); }
		if (IsKeyDown(KEY_DOWN)) player.StartCrouching();
		else player.StopCrouching();
		return;
	}

	// ========== GO! IDLE DETECTION ==========
	bool playerActing = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) ||
		IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN) ||
		IsKeyDown(KEY_SPACE) || IsKeyPressed(KEY_D) ||
		IsKeyPressed(KEY_S);
	if (playerActing) UiManager.NotifyPlayerMoved();

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

				// ===== START DZWIEKOW MACHINEGUN (4 x 0.25s) =====
				machinegunSoundActive = true;
				machinegunSoundCount = 0;
				machinegunSoundTimer = 0.0f;
				// Pierwszy dzwiek od razu
				audioManager.PlaySound(audioManager.GetMachinegunShootSound());
				machinegunSoundCount = 1;
			}
		}
		else {
			player.Shoot();
			Shoot(1, { 0.0f, 0.0f }, true);
			shootTimer = shootDelayPistol;
			audioManager.PlaySound(audioManager.GetShootSound());
		}
	}

	if (IsKeyPressed(KEY_S) && player.IsAlive() && grenadeCooldown <= 0.0f && UiManager.HasBombs()) {
		ThrowGrenade();
		UiManager.UseGrenade();
		UiManager.NotifyPlayerMoved();
		grenadeCooldown = grenadeDelay;
	}
}

void Game::ResolveCollisions() {

}

void Game::BulletsCollision() {
	// Calcular límites de cámara para filtrar soldados fuera de pantalla
	
}

void Game::GrenadesCollision() {
X
}

void Game::SoldierBlockCollision()
{
	
}

void Game::BlockCollisions() {
	
}





std::vector<Bullet> Game::CreateBullets()
{

}

std::vector<Block> Game::CreateBlocks()
{
	
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
		fprintf(file, "B %.0f,%.0f,%.0f,%.0f,%d\n",
			rect.x, rect.y, rect.width, rect.height, typeValue);
	}

	for (const auto& soldier : soldiers) {
		fprintf(file, "S %.0f,%.0f,%d\n",
			soldier.GetX(), soldier.GetY(), const_cast<Soldier&>(soldier).GetType());
	}

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
	MergeBlocks();
	TraceLog(LOG_INFO, "Nivel cargado: %d bloques, %d soldados, %d items",
		(int)blocks.size(), (int)soldiers.size(), (int)items.size());
}

std::vector<Item> Game::CreateItems() {

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
	Camera2D cam = camera.GetCamera();
	float screenWidth = (float)GetScreenWidth();
	float screenHeight = (float)GetScreenHeight();

	float leftBound = cam.target.x - screenWidth / 2.0f;
	float rightBound = cam.target.x + screenWidth / 2.0f;
	float topBound = cam.target.y - screenHeight / 2.0f;
	float bottomBound = cam.target.y + screenHeight / 2.0f;

	float margin = 200.0f;
	leftBound -= margin;
	rightBound += margin;
	topBound -= margin;
	bottomBound += margin;

	auto bIt = bullets.begin();
	while (bIt != bullets.end()) {
		Vector2 bulletPos = bIt->GetPosition();
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

void Game::MergeBlocks() {
	if (blocks.empty()) return;

	std::vector<Block> merged;
	const float EPS = 0.5f;  // tolerancia para comparar floats

	std::sort(blocks.begin(), blocks.end(), [](const Block& a, const Block& b) {
		if (fabsf(a.GetRect().y - b.GetRect().y) > 0.5f) return a.GetRect().y < b.GetRect().y;
		return a.GetRect().x < b.GetRect().x;
		});

	size_t i = 0;
	while (i < blocks.size()) {
		Rectangle r = blocks[i].GetRect();
		BlockType t = blocks[i].GetType();
		bool g = blocks[i].IsGround();

		if (t != BlockType::NORMAL) {
			merged.push_back(blocks[i]);
			i++;
			continue;
		}

		size_t j = i + 1;
		while (j < blocks.size()) {
			Rectangle rj = blocks[j].GetRect();
			bool sameRow = fabsf(rj.y - r.y) < EPS && fabsf(rj.height - r.height) < EPS;
			bool adjacent = fabsf(rj.x - (r.x + r.width)) < EPS;
			bool sameType = blocks[j].GetType() == t && blocks[j].IsGround() == g;
			if (sameRow && adjacent && sameType) {
				r.width += rj.width;
				j++;
			}
			else break;
		}

		if (g) merged.emplace_back(r.x, r.y, r.width, r.height, true);
		else   merged.emplace_back(r.x, r.y, r.width, r.height, false);
		i = j;
	}

	blocks = merged;
	TraceLog(LOG_INFO, "MergeBlocks: %d bloques tras fusionar", (int)blocks.size());
}