#include "SystemCollision.hpp"


SystemCollision::SystemCollision()
{
	
}

SystemCollision::~SystemCollision()
{
}

void SystemCollision::CollisionUpdate()
{
	SoldierBlockCollision();
	GrenadesCollision();
	BulletCollision();
	PlayerBlockCollision();
	ItemBlockCollision();
	BulletBlockCollision();
	ItemPlayerCollision();
}

void SystemCollision::SoldierBlockCollision() {
	for (auto& soldier : creationManager.GetSoldiers()) {
		soldier.SetLeftCollision(false);
		soldier.SetRightCollision(false);

		float distToPlayer = fabsf(player.GetPosition().x - soldier.GetPosition().x);
		bool playerNearby = distToPlayer < 800.0f;

		if (!playerNearby) {
			soldier.SetGrounded(false);
			for (const auto& block : creationManager.GetBlocks()) {
				if (block.GetType() == BlockType::CEILING) continue;

				Rectangle blockRect = block.GetRect();
				Rectangle hurtBox = soldier.GetHurtBox();

				// ===== RAMPAS =====
				if (block.IsRamp()) {
					float soldierCenterX = hurtBox.x + hurtBox.width / 2.0f;
					if (soldierCenterX >= blockRect.x && soldierCenterX <= blockRect.x + blockRect.width) {
						float rampSurfaceY = block.GetHeightAtX(soldierCenterX);
						float feetY = hurtBox.y + hurtBox.height;
						if (soldier.GetVelocityY() >= 0 && feetY >= rampSurfaceY) {
							soldier.SetY(rampSurfaceY - soldier.GetHeight());
							soldier.SetVelocityY(0);
							soldier.SetGrounded(true);
						}
					}
					continue;
				}

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

		for (const auto& block : creationManager.GetBlocks()) {
			if (block.GetType() == BlockType::CEILING) continue;

			Rectangle blockRect = block.GetRect();

			if (blockRect.x > soldier.GetPosition().x + 300.0f ||
				blockRect.x + blockRect.width < soldier.GetPosition().x - 300.0f) continue;

			Rectangle hurtBox = soldier.GetHurtBox();

			// ===== RAMPAS =====
			if (block.IsRamp()) {
				float soldierCenterX = hurtBox.x + hurtBox.width / 2.0f;
				if (soldierCenterX >= blockRect.x && soldierCenterX <= blockRect.x + blockRect.width) {
					float rampSurfaceY = block.GetHeightAtX(soldierCenterX);
					float feetY = hurtBox.y + hurtBox.height;
					if (soldier.GetVelocityY() >= 0 && feetY >= rampSurfaceY) {
						soldier.SetY(rampSurfaceY - soldier.GetHeight());
						soldier.SetVelocityY(0);
						soldier.SetGrounded(true);
					}
				}
				continue;
			}

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

void SystemCollision::GrenadesCollision() {
	for (auto& grenade : creationManager.GetGrenades()) {
		if (grenade.HasExploded()) {
			if (!grenade.HasPlayedSound()) {
				audioManager.PlaySound(audioManager.GetGrenadeSound());
				grenade.SetSoundPlayed(true);
			}
			Rectangle explosionBox = grenade.GetExplosionHitBox();
			for (auto& soldier : creationManager.GetSoldiers()) {
				if (soldier.GetisAlive() && CheckCollisionRecs(soldier.GetHurtBox(), explosionBox)) {
					soldier.TriggerDeath(audioManager);
					uiManager.AddScore(100);
				}
			}
		}
	}
}

void SystemCollision::BulletCollision() {

	auto bIt = creationManager.GetBullets().begin();
	while (bIt != creationManager.GetBullets().end()) {
		bool bulletHit = false;

		// 1. DAÑO A SOLDADOS (tipo 1 y 3)
		if (bIt->GetType() == 1 || bIt->GetType() == 3) {
			auto sIt = creationManager.GetSoldiers().begin();
			while (sIt != creationManager.GetSoldiers().end()) {

				if (sIt->GetisAlive() && CheckCollisionRecs(sIt->GetHurtBox(), bIt->GetHitbox())) {
					sIt->TriggerDeath(audioManager);
					uiManager.AddScore(100);
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
				}
			}
		}

		// 3. COLISIÓN CON CAJAS
		if (!bulletHit) {
			for (auto& item : creationManager.GetItems()) {
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

		if (bulletHit) bIt = creationManager.GetBullets().erase(bIt);
		else ++bIt;
	}

	// 4. LIMPIEZA DE SOLDADOS MUERTOS
	auto sIt = creationManager.GetSoldiers().begin();
	while (sIt != creationManager.GetSoldiers().end()) {
		if (!sIt->GetisAlive() && sIt->IsDeadAnimFinished())
			sIt = creationManager.GetSoldiers().erase(sIt);
		else ++sIt;
	}
}

void SystemCollision::PlayerBlockCollision()
{
	bool onGround = false;
	const float GROUND_TOLERANCE = 5.0f;

	player.SetLeftCollision(false);
	player.SetRightCollision(false);

	Rectangle playerRect = player.GetHitBox();

	for (const auto& block : creationManager.GetBlocks()) {
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
				float rampSurfaceY = block.GetHeightAtX(playerCenterX);
				float playerFeetY = playerRect.y + playerRect.height;

				// Si los pies están por encima de la superficie o bajando hacia ella
				if (player.GetVelocityY() >= 0 && playerFeetY >= rampSurfaceY) {
					player.SetY(rampSurfaceY - playerRect.height);
					player.SetVelocityY(0);
					onGround = true;
				}
			}
			continue;
		}




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

		for (const auto& block : creationManager.GetBlocks()) {
			Rectangle blockRect = block.GetRect();
			if (CheckCollisionRecs(playerRect, blockRect)) {
				if (player.GetVelocityY() >= 0 &&
					(player.GetY() + player.GetHeight() - player.GetVelocityY() <= blockRect.y + 5.0f)) {
					player.SetY(blockRect.y - player.GetHeight());
					player.SetVelocityY(0);
					onGround = true;
				}
			}
		}
		player.SetGrounded(onGround);

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
}

void SystemCollision::ItemBlockCollision()
{
	for (auto& item : creationManager.GetItems()) {
		if (item.IsGrounded() || item.GetType() != ItemType::SHOTGUN) continue;

		Rectangle itemRect = item.GetHitBox();
		for (const auto& block : creationManager.GetBlocks()) {
			if (block.IsRamp() || block.GetType() == BlockType::CEILING) continue;

			Rectangle blockRect = block.GetRect();
			float feetY = itemRect.y + itemRect.height;
			float blockTopY = blockRect.y;
			bool isOver = (itemRect.x + itemRect.width > blockRect.x &&
				itemRect.x < blockRect.x + blockRect.width);

			if (isOver && feetY >= blockTopY && feetY <= blockTopY + 30.0f) {
				// Ajusta posición para que quede encima del bloque
				float correction = blockTopY - itemRect.height;
				// GetHitBox está centrado, así que position.y = correction + height/2
				float hitboxOffsetY = item.GetPosition().y - itemRect.y;
				item.SetPositionY(correction + hitboxOffsetY);
				item.SetGrounded(true);
				item.SetGravity(false);
				break;
			}
		}
	}
}

void SystemCollision::BulletBlockCollision() {
	auto bIt = creationManager.GetBullets().begin();
	while (bIt != creationManager.GetBullets().end()) {
		bool bulletJustHit = false;

		if (!bIt->IsExploding()) {
			// Tipo 2 (bala soldado): colisiona con bloques sólidos y techo
			if (bIt->GetType() == 2 && bIt->GetDirectionY() > 0) {
				for (const auto& block : creationManager.GetBlocks()) {
					bool isSolid = (block.GetType() == BlockType::NORMAL && block.IsGround())
						|| block.GetType() == BlockType::CEILING;
					if (isSolid && CheckCollisionRecs(bIt->GetHitbox(), block.GetRect())) {
						bulletJustHit = true;
						break;
					}
				}
			}
			// Tipo 1 (pistola) y 3 (machinegun): colisiona con bloques sólidos y techo
			else if (bIt->GetType() == 1 || bIt->GetType() == 3) {
				for (const auto& block : creationManager.GetBlocks()) {
					bool isSolid = (block.GetType() == BlockType::NORMAL && block.IsGround())
						|| block.GetType() == BlockType::CEILING;
					if (isSolid && CheckCollisionRecs(bIt->GetHitbox(), block.GetRect())) {
						bulletJustHit = true;
						break;
					}
				}
			}
		}

		if (bulletJustHit) {
			if (bIt->GetType() == 1 || bIt->GetType() == 3) {
				bIt = creationManager.GetBullets().erase(bIt);
				continue;
			}
			else if (bIt->GetType() == 2) {
				bIt->SetExploding(true);
				bIt->GetAnim().SetAnimation(BulletState::EXPLOSIONSOLDIER);
			}
		}

		if (bIt->GetType() == 2 && bIt->IsExploding()) {
			if (bIt->GetAnim().IsAnimationFinished()) {
				bIt = creationManager.GetBullets().erase(bIt);
				continue;
			}
		}

		++bIt;
	}
}

void SystemCollision::ItemPlayerCollision()
{
	for (auto& item : creationManager.GetItems()) {
		if (item.IsActive() && CheckCollisionRecs(item.GetHitBox(), player.GetHitBox())) {
			if (item.GetType() == ItemType::SHOTGUN) {
				player.EquipMachinegun();
				item.Collect();
				audioManager.PlaySound(audioManager.GetMachinegunEquipSound());
				uiManager.SetAmmo(player.GetAmmo());
				uiManager.SetWeaponDisplay(UiManager::WeaponDisplay::MACHINEGUN);
			}
		}
		if (item.ShouldSpawnMachinegun()) {
			item.ConsumeSpawn();
			Vector2 spawnPos = { item.GetHitBox().x + 60.0f, item.GetHitBox().y - 20.0f };
			Item newItem(spawnPos, ItemType::SHOTGUN);
			newItem.SetGravity(true);
			creationManager.GetItems().push_back(newItem);
		}
	}
}
