#include "game.hpp"   
#include "InputManager.hpp"

InputManager::InputManager(Game* game) : game(game) 
{
}

InputManager::~InputManager()
{
}

void InputManager::InputUpdate()
{
}

void InputManager::InputCreditsPlayer()
{
	// Insertar crédito en cualquier momento
	if (IsKeyPressed(KEY_C) && timerManager.IsReady(TimerType::CREDIT_COOLDOWN)) {
		if (uiManager.GetCredits() < 99) {
			uiManager.SetCredits(1);
			timerManager.StartTimer(TimerType::CREDIT_DELAY);
			audioManager.PlaySound(audioManager.GetCreditSound());
		}
	}
}

void InputManager::InputChangeScene()
{
	if (IsKeyPressed(KEY_ENTER)) {
		if (sceneManager.currentState == SceneManager::TITLE) {
			// Solo iniciar si hay créditos
			if (uiManager.GetCredits() > 0) {
				uiManager.SetCredits(-1);  // Gasta 1 crédito
				audioManager.StopMusic(audioManager.GetTitleMusic());
				sceneManager.SetGameState(SceneManager::HOWTOPLAY);
			}
		}
		// INTRO -> TITLE obsługiwane w Game::Update() żeby można było zatrzymać muzykę
	}
}

void InputManager::InputPlayer()
{
	bool playerActing = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) ||
		IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN) ||
		IsKeyDown(KEY_SPACE) || IsKeyPressed(KEY_D) ||
		IsKeyPressed(KEY_S);
	if (playerActing) uiManager.NotifyPlayerMoved();

	if (IsKeyDown(KEY_LEFT)) player.MoveLeft();
	else if (IsKeyDown(KEY_RIGHT)) player.MoveRight();
	else player.StopMovingHorizontal();

	if (IsKeyPressed(KEY_UP)) player.StartAimingUp();
	if (IsKeyReleased(KEY_UP)) player.StopAimingUp();

	if (IsKeyDown(KEY_DOWN)) player.StartCrouching();
	else player.StopCrouching();

	if (IsKeyPressed(KEY_SPACE)) player.Jump();

	if (IsKeyDown(KEY_SPACE) && !player.GetIsGrounded() && player.HasJetpack()) {
		player.JetpackThrust();
	}

	// Debug
	if (IsKeyDown(KEY_RIGHT) && IsKeyDown(KEY_UP))
		TraceLog(LOG_INFO, "RIGHT+UP held, D pressed=%d", IsKeyPressed(KEY_D));

	if (IsKeyPressed(KEY_D) && timerManager.IsReady(TimerType::SHOOT_TIMER) && player.IsAlive()) {
		TraceLog(LOG_INFO, "D pressed, aimingUp=%d, velX=%.1f", player.IsAimingUp(), player.GetVelocityX());

		bool meleeTriggered = false;
		for (auto& soldier : creationManager.GetSoldiers()) {
			if (soldier.GetisAlive() &&
				CheckCollisionRecs(player.GetMeleeHitBox(), soldier.GetHurtBox())) {
				player.StartMelee();
				soldier.TriggerDeath(audioManager);
				Rectangle hurtBox = soldier.GetHurtBox();
				Vector2 bloodPos = { hurtBox.x + hurtBox.width / 2.0f, hurtBox.y + hurtBox.height / 2.0f };
				creationManager.GetBloodEffects().emplace_back(bloodPos);
				uiManager.AddScore(200);
				timerManager.StartTimer(TimerType::DELAY_PISTOL);
				meleeTriggered = true;
				break;
			}
		}

		if (!meleeTriggered) {
			for (auto& item : creationManager.GetItems()) {
				if (item.IsActive() && item.GetType() == ItemType::BOX &&
					!item.IsDestroyed() &&
					CheckCollisionRecs(player.GetMeleeHitBox(), item.GetHitBox())) {
					player.StartMelee();
					item.Destroy();
					timerManager.StartTimer(TimerType::DELAY_PISTOL);
					meleeTriggered = true;
					break;
				}
			}
		}

		if (!meleeTriggered) {
			for (auto& p : creationManager.GetPrisoners()) {
				if (!p.IsFreed() && CheckCollisionRecs(player.GetMeleeHitBox(), p.GetHitBox())) {
					p.TakeDamage();
					meleeTriggered = true;
					player.StartMelee();
					timerManager.StartTimer(TimerType::DELAY_PISTOL);
					break;
				}
			}
		}

		if (!meleeTriggered) {
			if (player.GetCurrentWeapon() == WeaponType::MACHINEGUN) {
				if (player.GetAmmo() > 0) {
					player.Shoot();
					game->StartMachinegunBurst();
					timerManager.ResetTimer(TimerType::MACHINEGUN_BURST_TIMER);
					timerManager.StartTimer(TimerType::DELAY_MACHINEGUN);
					audioManager.PlaySound(audioManager.GetMachinegunShootSound());
				}
			}
			else {
				player.Shoot();
				game->Shoot(1, {}, false);
				timerManager.StartTimer(TimerType::DELAY_PISTOL);
				audioManager.PlaySound(audioManager.GetShootSound());
			}
		}
	}

	if (IsKeyPressed(KEY_S) && player.IsAlive() &&
		timerManager.IsReady(TimerType::GRENADE_COOLDOWN) && uiManager.HasBombs()) {
		game->ThrowGrenade();
		uiManager.UseGrenade();
		uiManager.NotifyPlayerMoved();
		timerManager.StartTimer(TimerType::DELAY_GRENADE);
	}
}

void InputManager::InputUi()
{

}

void InputManager::InputMachinegunBurst()
{
	if (!game->IsMachinegunBurst()) return;

	PlayerDirection dir = game->GetMachinegunBurstDir();
	if (dir == PlayerDirection::LEFT) {
		if (IsKeyDown(KEY_LEFT)) { player.MoveLeft(); uiManager.NotifyPlayerMoved(); }
		else player.StopMovingHorizontal();
	}
	else if (dir == PlayerDirection::RIGHT) {
		if (IsKeyDown(KEY_RIGHT)) { player.MoveRight(); uiManager.NotifyPlayerMoved(); }
		else player.StopMovingHorizontal();
	}
	else if (dir == PlayerDirection::UP) {  // ← añade
		if (IsKeyDown(KEY_LEFT)) { player.MoveLeft(); uiManager.NotifyPlayerMoved(); }
		else if (IsKeyDown(KEY_RIGHT)) { player.MoveRight(); uiManager.NotifyPlayerMoved(); }
		else player.StopMovingHorizontal();
	}
	if (IsKeyPressed(KEY_SPACE)) { player.Jump(); uiManager.NotifyPlayerMoved(); }
	if (IsKeyDown(KEY_DOWN)) player.StartCrouching();
	else player.StopCrouching();
}

void InputManager::InputContinueScreen()
{
	// C -> insertar crédito
	if (IsKeyPressed(KEY_C) && timerManager.IsReady(TimerType::CREDIT_COOLDOWN)) {
		if (uiManager.GetCredits() < 99) {
			uiManager.SetCredits(1);
			timerManager.StartTimer(TimerType::CREDIT_DELAY);
			audioManager.PlaySound(audioManager.GetCreditSound());
		}
	}

	// R -> respawnear gastando 1 crédito
	if (IsKeyPressed(KEY_R) && uiManager.GetCredits() > 0) {
		uiManager.SetCredits(-1);
		uiManager.StopContinue();
		player.Respawn();

		// Resetea a pistola primero
		player.ResetWeapon();

		// Random machinegun si estás cerca del boss
		if (player.GetX() > 14000.0f && GetRandomValue(0, 1) == 1) {
			player.EquipMachinegunFresh();  // ← siempre 200 balas fijas
			uiManager.SetAmmo(player.GetAmmo());
			uiManager.SetWeaponDisplay(UiManager::WeaponDisplay::MACHINEGUN);
		}
		else {
			uiManager.SetAmmo(0);
			uiManager.SetWeaponDisplay(UiManager::WeaponDisplay::PISTOL);
		}

		game->SetContinueStarted(false);
		sceneManager.SetGameState(SceneManager::GAME);
	}
}

void InputManager::InputEnding()
{
	if (!uiManager.IsEndingActive()) return;

	if (IsKeyPressed(KEY_ENTER))
		uiManager.TriggerEndingFadeOut();

	if (uiManager.IsEndingFinished()) {
		game->SetShouldRestart(true);
		sceneManager.SetGameState(SceneManager::TITLE);
	}
}