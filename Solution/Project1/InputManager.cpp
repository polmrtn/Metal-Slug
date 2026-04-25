#include "InputManager.hpp"

InputManager::InputManager()
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
				// These variables belong to Game; keep them as application-wide flags if needed.
				// If you want them in Globals, move them there. For now, assume Game still owns them.
				// Example: gameMachinegunBurst = true; (optional)
				// reset burst timer and start shoot delay for machinegun
				timerManager.ResetTimer(TimerType::MACHINEGUN_BURST_TIMER);
				timerManager.StartTimer(TimerType::DELAY_MACHINEGUN);

				// determine burst direction
				// note: InputManager doesn't own machinegunBurstDir; caller (Game) should react accordingly.

				// ===== START MACHINEGUN SOUNDS =====
				// If you centralize machinegun sound state, move those counters into TimerManager or Globals.
				audioManager.PlaySound(audioManager.GetMachinegunShootSound());
			}
		}
		else {
			player.Shoot();
			// Shoot(1, ...) is a function of Game; InputManager should notify Game to spawn bullets.
			// If Shoot is global, call it via  or make Game expose a method.
			// Here we assume there's a global function or Game will handle this input in its update.
			timerManager.StartTimer(TimerType::DELAY_PISTOL);
			audioManager.PlaySound(audioManager.GetShootSound());
		}
	}

	// GRENADE: use TimerManager to check/set cooldown
	if (IsKeyPressed(KEY_S) && player.IsAlive() && timerManager.IsReady(TimerType::GRENADE_COOLDOWN) && uiManager.HasBombs()) {
		// Again: spawning grenades is Game responsibility; notify Game or call a global method.
		// For now call a function in Globals if implemented, or set a flag.
		uiManager.UseGrenade();
		uiManager.NotifyPlayerMoved();
		timerManager.StartTimer(TimerType::DELAY_GRENADE);
	}
}

void InputManager::InputUi()
{
	if (!player.IsAlive()) {
		BeginDrawing();
		Vector2 deathPos = player.GetDeathPosition();
		DrawText(TextFormat("YOU DIED at (%.0f, %.0f)", deathPos.x, deathPos.y),
			GetScreenWidth() / 2 - 200, GetScreenHeight() / 2 - 50, 20, RED);

		if (uiManager.GetCredits() > 0) {
			DrawText("Press R to respawn (costs 1 credit)",
				GetScreenWidth() / 2 - 200, GetScreenHeight() / 2, 20, WHITE);
		}
		else {
			DrawText("NO CREDITS! Press C to insert coin",
				GetScreenWidth() / 2 - 200, GetScreenHeight() / 2, 20, RED);
		}

		if (IsKeyPressed(KEY_R) && uiManager.GetCredits() > 0) {
			uiManager.SetCredits(-1);  // Gasta 1 crédito
			player.Respawn();
			TraceLog(LOG_INFO, "Respawn. Créditos restantes: %d", uiManager.GetCredits());
		}
		if (IsKeyPressed(KEY_C) && timerManager.GetTimer(TimerType::CREDIT_COOLDOWN) <= 0.0f) {
			if (uiManager.GetCredits() < 99) {
				uiManager.SetCredits(1);
				timerManager.SetTimerValue(TimerType::CREDIT_COOLDOWN, timerManager.GetTimer(TimerType::CREDIT_DELAY));
				TraceLog(LOG_INFO, "Crédito insertado mientras muerto. Total: %d", uiManager.GetCredits());
			}
		}
	}
}
