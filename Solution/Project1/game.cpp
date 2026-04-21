#define _CRT_SECURE_NO_WARNINGS
#include "game.hpp"
#include "LevelMap.hpp" 
#include <raymath.h>


bool musicStarted = false;
Color BGCOLOR = { 195, 195, 170 };
Game::Game() : camera({ 1280.0f/2 , 896/2  })
{
	FILE* file = fopen("level_blocks.txt", "r");
	if (file) {
		fclose(file);
		LoadBlocksFromFile("level_blocks.txt");
	}
	else {
		blocks = CreateBlocks();  // Solo si no hay archivo
	}
	//soldiers = CreateSoldiers();
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
	backgroundManager.Drawfrontground();
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
			if (item.IsActive() && CheckCollisionRecs(item.GetHitBox(), player.GetHitBox())) {
				if (item.GetType() == ItemType::SHOTGUN) {
					player.EquipMachinegun();  
					TraceLog(LOG_INFO, "Machinegun collected! Ammo: 20");
				}
				item.Collect();
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
	// Calcular el suelo buscando en los bloques
	float groundY = 650.0f;  // Valor por defecto
	Rectangle playerRect = player.GetHitBox();

	for (const auto& block : blocks) {
		Rectangle blockRect = block.GetRect();
		// Buscar bloques que estén debajo del jugador
		if (blockRect.x < playerRect.x + playerRect.width &&
			blockRect.x + blockRect.width > playerRect.x &&
			blockRect.y > playerRect.y) {
			// Encontrar el bloque más cercano por debajo
			if (groundY == 650.0f || blockRect.y < groundY) {
				groundY = blockRect.y;
			}
		}
	}
	TraceLog(LOG_INFO, "Grenade groundY: %.2f", groundY);  // ← Debug

	// Obtener datos del lanzamiento
	GrenadeThrowData data = player.ThrowGrenade();
	data.targetPos.y = groundY;  // ← Actualizar con el suelo real

	if (data.valid) {
		grenades.emplace_back(data.startPos, data.targetPos, data.power);
		TraceLog(LOG_INFO, "Grenade thrown! GroundY: %.2f", groundY);
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
	if (IsKeyPressed(KEY_UP) && !player.GetAnim().IsMachinegunShootingUp()) {
		player.StartAimingUp();
	}
	if (IsKeyReleased(KEY_UP) && !player.GetAnim().IsMachinegunShootingUp()) {
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

	// ========== CHANGE SCENE ==========
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
		// ========== DAÑO AL JUGADOR POR BALA TIPO 2 (GRANADA) ==========
		if (bIt->GetType() == 2 && bIt->IsExploding()) {
			Rectangle explosionBox = bIt->GetHitbox();

			if (player.IsAlive() && !player.IsInvincible()) {
				if (CheckCollisionRecs(explosionBox, player.GetHitBox())) {
					player.TakeDamage();
					TraceLog(LOG_INFO, "Player killed by grenade explosion");
				}
			}
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

void Game::BlockCollisions() {
	bool onGround = false;
	const float GROUND_TOLERANCE = 5.0f;

	player.SetLeftCollision(false);
	player.SetRightCollision(false);

	for (const auto& block : blocks) {
		Rectangle playerRect = player.GetHitBox();
		Rectangle blockRect = block.GetRect();

		// ========== COLISIÓN CON RAMPAS ==========
		if (block.IsRamp()) {
			// Centro X del jugador
			float playerCenterX = playerRect.x + playerRect.width / 2;

			// Solo aplicar rampa si el jugador está dentro del rango X de la rampa
			if (playerCenterX >= blockRect.x && playerCenterX <= blockRect.x + blockRect.width) {
				float rampHeight = block.GetHeightAtX(playerCenterX);
				float playerFeetY = playerRect.y + playerRect.height;

				// Si los pies están en o cerca de la rampa
				if (playerFeetY >= rampHeight && playerFeetY <= rampHeight + 20.0f) {
					float newY = rampHeight - playerRect.height;
					player.SetY(newY);
					player.SetVelocityY(0);
					onGround = true;
				}
			}
			continue;
		}

		// ========== COLISIÓN SOLDADOS CON BLOQUES ==========
		auto It = soldiers.begin();
		while (It != soldiers.end()) {
			if (CheckCollisionRecs(It->GetHurtBox(), blockRect)) {
				if (It->GetVelocityY() >= 0) {
					It->SetY(blockRect.y - It->GetHeight());
					It->SetVelocityY(0);
					It->SetGrounded(true);
					break;
				}
			}
			++It;
		}

		// ========== COLISIÓN JUGADOR CON SUELO NORMAL ==========
		float feetY = playerRect.y + playerRect.height;
		float blockTopY = blockRect.y;
		float previousFeetY = player.GetPreviousY() + player.GetHeight();
		bool wasBelow = (player.GetVelocityY() < 0 && previousFeetY <= blockTopY);
		float verticalDistance = feetY - blockTopY;
		bool isVerticalNear = (verticalDistance >= 0 && verticalDistance <= 50.0f);
		bool isOverBlock = (playerRect.x + playerRect.width > blockRect.x + GROUND_TOLERANCE &&
			playerRect.x < blockRect.x + blockRect.width - GROUND_TOLERANCE);

		if (isOverBlock && isVerticalNear && player.GetVelocityY() >= 0 && !wasBelow && verticalDistance <= 20.0f) {
			float newY = blockTopY - playerRect.height;
			player.SetY(newY);
			player.SetVelocityY(0);
			onGround = true;
		}

		// ========== COLISIONES LATERALES ==========
		if (block.IsGround()) {
			Rectangle leftHitBox = player.GetLeftHitBox();
			if (CheckCollisionRecs(leftHitBox, blockRect)) {
				float newX = blockRect.x + blockRect.width;
				if (player.GetX() < newX) {
					player.SetX(newX);
					player.SetLeftCollision(true);
				}
				else {
					player.SetLeftCollision(true);
				}
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
		else {
			blocks.emplace_back(x, y, w, h, typeValue == 1);
		}
	}

	fclose(file);
	TraceLog(LOG_INFO, "Bloques cargados desde %s (%d bloques)", filename, (int)blocks.size());
}

std::vector<Item> Game::CreateItems() {
	std::vector<Item> items;
	// X=800, Y=600 (asumiendo que el suelo está en Y=600-650)
	items.emplace_back(Vector2{ 800.0f, 600.0f }, ItemType::SHOTGUN);
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
