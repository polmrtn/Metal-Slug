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


	// ========== CAMBIO DE ESCENA ==========
	

	// ========== MODO EDITOR ==========
	

	// ========== SI ESTÁ CAYENDO, NO PROCESAR INPUT DEL JUGADOR ==========
	if (player.IsFalling()) return;

	// ========== DEBUG ==========


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

