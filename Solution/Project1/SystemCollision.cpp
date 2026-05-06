#include "SystemCollision.hpp"
#include <cmath>
#include <algorithm>

SystemCollision::SystemCollision() {}
SystemCollision::~SystemCollision() {}

void SystemCollision::CollisionUpdate()
{
    PlayerBlockCollision();
    SoldierBlockCollision();
    BulletCollision();
    BulletBlockCollision();
    GrenadesCollision();
    ItemBlockCollision();
    ItemPlayerCollision();
}

// ═════════════════════════════════════════════════════════════
//  PLAYER ↔ BLOQUES
// ═════════════════════════════════════════════════════════════
void SystemCollision::PlayerBlockCollision()
{
    bool onGround = false;
    player.SetLeftCollision(false);
    player.SetRightCollision(false);

    Rectangle pr = player.GetHitBox();
    const auto& colliders = creationManager.GetTileMap().GetColliders();

    for (const auto& col : colliders)
    {
        const Rectangle& br = col.rect;
        if (br.x > pr.x + 500.0f || br.x + br.width < pr.x - 500.0f) continue;

        switch (col.type)
        {
        case TileType::SOLID:
        {
            // -- Suelo --
            float feetY = pr.y + pr.height;
            float prevFeetY = player.GetPreviousY() + player.GetHeight();
            // ← más permisivo si viene de rampa
            bool  wasAbove = prevFeetY <= br.y + 8.0f;
            bool  overlapX = (pr.x + pr.width > br.x + 2.0f) &&
                (pr.x < br.x + br.width - 2.0f);
            float penetration = feetY - br.y;

            if (overlapX && wasAbove && player.GetVelocityY() >= 0 &&
                penetration >= 0.0f && penetration <= 45.0f)
            {
                player.SetY(br.y - player.GetHeight());
                player.SetVelocityY(0.0f);
                onGround = true;
                pr = player.GetHitBox();
            }

            // -- Cabeza --
            float headY = pr.y;
            float prevHeadY = player.GetPreviousY();
            bool  wasBelow = prevHeadY >= br.y + br.height - 1.0f;
            bool  overlapX2 = (pr.x + pr.width > br.x + 2.0f) &&
                (pr.x < br.x + br.width - 2.0f);
            float headPen = (br.y + br.height) - headY;

            if (overlapX2 && wasBelow && player.GetVelocityY() < 0 &&
                headPen >= 0.0f && headPen <= 45.0f)
            {
                player.SetY(br.y + br.height);
                player.SetVelocityY(0.0f);
                pr = player.GetHitBox();
            }

            // -- Laterales --
            {
                bool vertOverlap = (pr.y + pr.height > br.y + 4.0f) &&
                    (pr.y < br.y + br.height - 4.0f);
                if (vertOverlap)
                {
                    Rectangle leftBox = player.GetLeftHitBox();
                    if (CheckCollisionRecs(leftBox, br))
                    {
                        float ov = (br.x + br.width) - leftBox.x;
                        player.SetX(player.GetX() + ov);
                        player.SetLeftCollision(true);
                        pr = player.GetHitBox();
                    }
                    Rectangle rightBox = player.GetRightHitBox();
                    if (CheckCollisionRecs(rightBox, br))
                    {
                        float ov = (rightBox.x + rightBox.width) - br.x;
                        player.SetX(player.GetX() - ov);
                        player.SetRightCollision(true);
                        pr = player.GetHitBox();
                    }
                }
            }
            break;
        }

        case TileType::PLATFORM:
        {
            float feetY = pr.y + pr.height;
            float prevFeetY = player.GetPreviousY() + player.GetHeight();
            bool  wasAbove = prevFeetY <= br.y + 1.0f;
            bool  overlapX = (pr.x + pr.width > br.x + 2.0f) &&
                (pr.x < br.x + br.width - 2.0f);
            float penetration = feetY - br.y;

            if (overlapX && wasAbove && player.GetVelocityY() >= 0 &&
                penetration >= -1.0f && penetration <= 45.0f)
            {
                player.SetY(br.y - player.GetHeight());
                player.SetVelocityY(0.0f);
                onGround = true;
                pr = player.GetHitBox();
            }
            break;
        }

        case TileType::CEILING:
        {
            float headY = pr.y;
            float prevHeadY = player.GetPreviousY();
            bool  wasBelow = prevHeadY >= br.y + br.height - 1.0f;
            bool  overlapX = (pr.x + pr.width > br.x + 2.0f) &&
                (pr.x < br.x + br.width - 2.0f);
            float headPen = (br.y + br.height) - headY;

            if (overlapX && wasBelow && player.GetVelocityY() < 0 &&
                headPen >= 0.0f && headPen <= 45.0f)
            {
                player.SetY(br.y + br.height);
                player.SetVelocityY(0.0f);
                pr = player.GetHitBox();
            }
            break;
        }
        case TileType::RAMP_UP:
        {
            float centerX = pr.x + pr.width / 2.0f;
            if (centerX < br.x || centerX > br.x + br.width) break;

            float surfaceY = col.GetRampSurfaceY(centerX);
            float feetY = pr.y + pr.height;

            if (feetY >= surfaceY - 10.0f && feetY <= surfaceY + 45.0f)
            {
                player.SetY(surfaceY - player.GetHeight());
                player.SetVelocityY(3.0f);
                onGround = true;
                player.SetRampGroundedFrames(3);  // ← 3 frames de gracia
                pr = player.GetHitBox();
            }
            break;
        }
        }
    }

    if (onGround)
        player.SetGrounded(true);
    else if (player.GetRampGroundedFrames() > 0)
        player.SetGrounded(true);  // los frames de gracia los decrementa Player::Update
    else
        player.SetGrounded(false);
}

// ═════════════════════════════════════════════════════════════
//  SOLDIER ↔ BLOQUES
// ═════════════════════════════════════════════════════════════
void SystemCollision::SoldierBlockCollision()
{
    const auto& colliders = creationManager.GetTileMap().GetColliders();

    for (auto& soldier : creationManager.GetSoldiers())
    {
        soldier.SetLeftCollision(false);
        soldier.SetRightCollision(false);
        soldier.SetGrounded(false);

        Rectangle hr = soldier.GetHurtBox();
        float distToPlayer = fabsf(player.GetPosition().x - soldier.GetPosition().x);

        for (const auto& col : colliders)
        {
            const Rectangle& br = col.rect;
            if (br.x > hr.x + 400.0f || br.x + br.width < hr.x - 400.0f) continue;
            if (col.type == TileType::CEILING || col.type == TileType::PLATFORM) continue;

            if (col.type == TileType::RAMP_UP)
            {
                float centerX = hr.x + hr.width / 2.0f;
                if (centerX < br.x || centerX > br.x + br.width) continue;
                float surfaceY = col.GetRampSurfaceY(centerX);
                float feetY = hr.y + hr.height;
                if (soldier.GetVelocityY() >= 0 && feetY >= surfaceY)
                {
                    soldier.SetY(surfaceY - soldier.GetHeight());
                    soldier.SetVelocityY(0);
                    soldier.SetGrounded(true);
                }
                continue;
            }

            // SOLID: suelo
            float feetY = hr.y + hr.height;
            float penetration = feetY - br.y;
            bool  overlapX = (hr.x + hr.width > br.x) && (hr.x < br.x + br.width);

            if (overlapX && soldier.GetVelocityY() >= 0 &&
                penetration >= 0.0f && penetration <= 25.0f)
            {
                soldier.SetY(br.y - soldier.GetHeight());
                soldier.SetVelocityY(0);
                soldier.SetGrounded(true);
            }

            // Laterales solo si el jugador está cerca
            if (distToPlayer < 800.0f)
            {
                bool vertOverlap = (hr.y + hr.height > br.y + 5.0f) &&
                    (hr.y < br.y + br.height - 5.0f);
                if (vertOverlap)
                {
                    if (CheckCollisionRecs(soldier.GetLeftHitBox(), br)) soldier.SetLeftCollision(true);
                    if (CheckCollisionRecs(soldier.GetRightHitBox(), br)) soldier.SetRightCollision(true);
                }
            }
        }
    }
}

// ═════════════════════════════════════════════════════════════
//  BULLET ↔ SOLDADOS / JUGADOR / CAJAS
// ═════════════════════════════════════════════════════════════
void SystemCollision::BulletCollision()
{
    auto& bullets = creationManager.GetBullets();
    auto& soldiers = creationManager.GetSoldiers();
    auto& items = creationManager.GetItems();

    auto bIt = bullets.begin();
    while (bIt != bullets.end())
    {
        bool hit = false;

        if (bIt->GetType() == 1 || bIt->GetType() == 3)
        {
            for (auto& s : soldiers)
            {
                if (s.GetisAlive() && CheckCollisionRecs(s.GetHurtBox(), bIt->GetHitbox()))
                {
                    s.TriggerDeath(audioManager);
                    uiManager.AddScore(100);
                    hit = true;
                    break;
                }
            }
        }

        if (!hit && bIt->GetType() == 2 && bIt->IsExploding())
        {
            if (player.IsAlive() && !player.IsInvincible() &&
                CheckCollisionRecs(bIt->GetHitbox(), player.GetHitBox()))
                player.TakeDamage();
        }

        if (!hit)
        {
            for (auto& item : items)
            {
                if (item.IsActive() && item.GetType() == ItemType::BOX &&
                    (bIt->GetType() == 1 || bIt->GetType() == 3) &&
                    CheckCollisionRecs(bIt->GetHitbox(), item.GetHitBox()))
                {
                    item.Destroy();
                    hit = true;
                    break;
                }
            }
        }
        // 4. DAÑO AL BOSS
        if (!hit && (boss.IsActive() || boss.IsInIntro()) &&
            (bIt->GetType() == 1 || bIt->GetType() == 3))
        {
            if (CheckCollisionRecs(bIt->GetHitbox(), boss.GetHitBox()))
            {
                boss.TakeDamage();
                boss.StartIntro();
                hit = true;
            }
        }
        if (hit) bIt = bullets.erase(bIt);
        else     ++bIt;
    }

    soldiers.erase(std::remove_if(soldiers.begin(), soldiers.end(),
        [](const Soldier& s) { return !s.GetisAlive() && s.IsDeadAnimFinished(); }),
        soldiers.end());
}

// ═════════════════════════════════════════════════════════════
//  BULLET ↔ BLOQUES
// ═════════════════════════════════════════════════════════════
void SystemCollision::BulletBlockCollision()
{
    const auto& colliders = creationManager.GetTileMap().GetColliders();
    auto& bullets = creationManager.GetBullets();

    auto bIt = bullets.begin();
    while (bIt != bullets.end())
    {
        bool hit = false;

        if (!bIt->IsExploding())
        {
            for (const auto& col : colliders)
            {
                if (col.type != TileType::SOLID && col.type != TileType::CEILING) continue;
                if (!CheckCollisionRecs(bIt->GetHitbox(), col.rect)) continue;
                if (bIt->GetType() == 2 && bIt->GetDirectionY() <= 0) continue;
                hit = true;
                break;
            }
        }

        if (hit)
        {
            if (bIt->GetType() == 1 || bIt->GetType() == 3)
            {
                bIt = bullets.erase(bIt);
                continue;
            }
            else if (bIt->GetType() == 2)
            {
                bIt->SetExploding(true);
                bIt->GetAnim().SetAnimation(BulletState::EXPLOSIONSOLDIER);
            }
        }

        if (bIt->GetType() == 2 && bIt->IsExploding() && bIt->GetAnim().IsAnimationFinished())
        {
            bIt = bullets.erase(bIt);
            continue;
        }

        ++bIt;
    }
}

// ═════════════════════════════════════════════════════════════
//  GRANADAS
// ═════════════════════════════════════════════════════════════
void SystemCollision::GrenadesCollision()
{
    for (auto& grenade : creationManager.GetGrenades())
    {
        if (!grenade.HasExploded()) continue;
        if (!grenade.HasPlayedSound())
        {
            audioManager.PlaySound(audioManager.GetGrenadeSound());
            grenade.SetSoundPlayed(true);
        }
        Rectangle explosionBox = grenade.GetExplosionHitBox();
        for (auto& soldier : creationManager.GetSoldiers())
        {
            if (soldier.GetisAlive() &&
                CheckCollisionRecs(soldier.GetHurtBox(), explosionBox))
            {
                soldier.TriggerDeath(audioManager);
                uiManager.AddScore(100);
            }
        }
    }
}

// ═════════════════════════════════════════════════════════════
//  ITEM ↔ BLOQUES
// ═════════════════════════════════════════════════════════════
void SystemCollision::ItemBlockCollision()
{
    const auto& colliders = creationManager.GetTileMap().GetColliders();

    for (auto& item : creationManager.GetItems())
    {
        if (item.IsGrounded() || item.GetType() != ItemType::SHOTGUN) continue;

        Rectangle ir = item.GetHitBox();
        for (const auto& col : colliders)
        {
            if (col.type == TileType::CEILING || col.type == TileType::RAMP_UP) continue;
            const Rectangle& br = col.rect;
            float feetY = ir.y + ir.height;
            float penetration = feetY - br.y;
            bool  overlapX = (ir.x + ir.width > br.x) && (ir.x < br.x + br.width);

            if (overlapX && penetration >= 0.0f && penetration <= 35.0f)
            {
                float offsetY = item.GetPosition().y - ir.y;
                item.SetPositionY(br.y - ir.height + offsetY);
                item.SetGrounded(true);
                item.SetGravity(false);
                break;
            }
        }
    }
}

// ═════════════════════════════════════════════════════════════
//  ITEM ↔ JUGADOR
// ═════════════════════════════════════════════════════════════
void SystemCollision::ItemPlayerCollision()
{
    auto& items = creationManager.GetItems();
    for (auto& item : items)
    {
        if (!item.IsActive()) continue;

        if (item.GetType() == ItemType::SHOTGUN &&
            CheckCollisionRecs(item.GetHitBox(), player.GetHitBox()))
        {
            player.EquipMachinegun();
            item.Collect();
            audioManager.PlaySound(audioManager.GetMachinegunEquipSound());
            uiManager.SetAmmo(player.GetAmmo());
            uiManager.SetWeaponDisplay(UiManager::WeaponDisplay::MACHINEGUN);
        }

        if (item.ShouldSpawnMachinegun())
        {
            item.ConsumeSpawn();
            Vector2 spawnPos = { item.GetHitBox().x + 60.0f, item.GetHitBox().y - 20.0f };
            Item newItem(spawnPos, ItemType::SHOTGUN);
            newItem.SetGravity(true);
            items.push_back(newItem);
        }
    }
}