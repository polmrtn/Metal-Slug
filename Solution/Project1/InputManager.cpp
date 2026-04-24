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

	if (IsKeyPressed(KEY_S) && player.IsAlive() && grenadeCooldown <= 0.0f && uiManager.HasBombs()) {
		ThrowGrenade();
		uiManager.UseGrenade();
		uiManager.NotifyPlayerMoved();
		grenadeCooldown = grenadeDelay;
	}
}
