#include "InputManager.hpp"
#include "GlobalManagers.hpp"

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
	if (!Globals::player.IsAlive()) {
		if (IsKeyPressed(KEY_R) && Globals::uiManager.GetCredits() > 0) {
			Globals::uiManager.SetCredits(-1);
			Globals::player.Respawn();
		}
		return;
	}
}

void InputManager::InputChangeScene()
{
	if (IsKeyPressed(KEY_ENTER)) {
		if (Globals::sceneManager.currentState == SceneManager::TITLE) {
			// Solo iniciar si hay créditos
			if (Globals::uiManager.GetCredits() > 0) {
				Globals::uiManager.SetCredits(-1);  // Gasta 1 crédito
				Globals::audioManager.StopMusic(Globals::audioManager.GetTitleMusic());
				Globals::audioManager.PlaySound(Globals::audioManager.GetGameSound());
				Globals::sceneManager.SetGameState(SceneManager::GAME);
			}
		}
		else if (Globals::sceneManager.currentState == SceneManager::INTRO) {
			Globals::sceneManager.SetGameState(SceneManager::TITLE);
		}
	}
}

void InputManager::InputPlayer()
{
	bool playerActing = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) ||
		IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN) ||
		IsKeyDown(KEY_SPACE) || IsKeyPressed(KEY_D) ||
		IsKeyPressed(KEY_S);
	if (playerActing) Globals::uiManager.NotifyPlayerMoved();

	if (IsKeyDown(KEY_LEFT)) Globals::player.MoveLeft();
	else if (IsKeyDown(KEY_RIGHT)) Globals::player.MoveRight();
	else Globals::player.StopMovingHorizontal();

	if (IsKeyPressed(KEY_UP)) Globals::player.StartAimingUp();
	if (IsKeyReleased(KEY_UP)) Globals::player.StopAimingUp();

	if (IsKeyDown(KEY_DOWN)) Globals::player.StartCrouching();
	else Globals::player.StopCrouching();

	if (IsKeyPressed(KEY_SPACE)) Globals::player.Jump();

	// SHOOTING: use TimerManager methods instead of assigning to GetTimer(...) result
	if (IsKeyPressed(KEY_D) && Globals::timerManager.IsReady(TimerType::SHOOT_TIMER)) {
		if (Globals::player.GetCurrentWeapon() == WeaponType::MACHINEGUN) {
			if (Globals::player.GetAmmo() > 0) {
				Globals::player.Shoot();
				// These variables belong to Game; keep them as application-wide flags if needed.
				// If you want them in Globals, move them there. For now, assume Game still owns them.
				// Example: Globals::gameMachinegunBurst = true; (optional)
				// reset burst timer and start shoot delay for machinegun
				Globals::timerManager.ResetTimer(TimerType::MACHINEGUN_BURST_TIMER);
				Globals::timerManager.StartTimer(TimerType::DELAY_MACHINEGUN);

				// determine burst direction
				// note: InputManager doesn't own machinegunBurstDir; caller (Game) should react accordingly.

				// ===== START MACHINEGUN SOUNDS =====
				// If you centralize machinegun sound state, move those counters into TimerManager or Globals.
				Globals::audioManager.PlaySound(Globals::audioManager.GetMachinegunShootSound());
			}
		}
		else {
			Globals::player.Shoot();
			// Shoot(1, ...) is a function of Game; InputManager should notify Game to spawn bullets.
			// If Shoot is global, call it via Globals:: or make Game expose a method.
			// Here we assume there's a global function or Game will handle this input in its update.
			Globals::timerManager.StartTimer(TimerType::DELAY_PISTOL);
			Globals::audioManager.PlaySound(Globals::audioManager.GetShootSound());
		}
	}

	// GRENADE: use TimerManager to check/set cooldown
	if (IsKeyPressed(KEY_S) && Globals::player.IsAlive() && Globals::timerManager.IsReady(TimerType::GRENADE_COOLDOWN) && Globals::uiManager.HasBombs()) {
		// Again: spawning grenades is Game responsibility; notify Game or call a global method.
		// For now call a function in Globals if implemented, or set a flag.
		Globals::uiManager.UseGrenade();
		Globals::uiManager.NotifyPlayerMoved();
		Globals::timerManager.StartTimer(TimerType::DELAY_GRENADE);
	}
}

void InputManager::InputUi()
{
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
