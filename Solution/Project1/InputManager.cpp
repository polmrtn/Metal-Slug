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
		}
	}

	// Respawn
	if (!player.IsAlive()) {
		if (IsKeyPressed(KEY_R) && uiManager.GetCredits() > 0) {
			uiManager.SetCredits(-1);
			player.Respawn();
		}
		return;
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
				audioManager.PlaySound(audioManager.GetGameSound());
				sceneManager.SetGameState(SceneManager::GAME);
			}
		}
		else if (sceneManager.currentState == SceneManager::INTRO) {
			sceneManager.SetGameState(SceneManager::TITLE);
		}
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

	// SHOOTING: use TimerManager methods instead of assigning to GetTimer(...) result
	if (IsKeyPressed(KEY_D) && timerManager.IsReady(TimerType::SHOOT_TIMER)) {
		if (player.GetCurrentWeapon() == WeaponType::MACHINEGUN) {
			if (player.GetAmmo() > 0) {
				player.Shoot();
				game->StartMachinegunBurst();  // ← método nuevo en Game
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

	// GRENADE: use TimerManager to check/set cooldown
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
    // Obslugiwane przez CONTINUE_SCREEN state w game.cpp
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
	if (IsKeyPressed(KEY_SPACE)) { player.Jump(); uiManager.NotifyPlayerMoved(); }
	if (IsKeyDown(KEY_DOWN)) player.StartCrouching();
	else player.StopCrouching();
}
