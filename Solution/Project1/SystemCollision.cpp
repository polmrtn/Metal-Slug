#include "SystemCollision.hpp"
#include "item.hpp"
#include <cmath>
#include <algorithm>

SystemCollision::SystemCollision() {}
SystemCollision::~SystemCollision() {}

void SystemCollision::CollisionUpdate()
{
    PlayerBlockCollision();
    PlayerBoxCollision();
    SoldierBlockCollision();
    BulletCollision();
    BulletBlockCollision();
    GrenadesCollision();
    ItemBlockCollision();
    ItemPlayerCollision();
    BossAttackPlayerCollision();
    PrisonerBlockCollision();
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
            bool wasAbove = player.IsDyingInAir()
                ? prevFeetY <= br.y + 200.0f   // más tolerante cuando cae muerto
                : prevFeetY <= br.y + 15.0f;
            bool  overlapX = (pr.x + pr.width > br.x + 2.0f) &&
                (pr.x < br.x + br.width - 2.0f);
            float penetration = feetY - br.y;

            float maxPen = player.IsDyingInAir() ? 200.0f : 60.0f;

            if (overlapX && wasAbove &&
                (player.GetVelocityY() >= 0 || player.IsDyingInAir()) &&
                penetration >= 0.0f && penetration <= maxPen)
            {
                if (player.IsDyingInAir()) {
                    Vector2 spawnPos = { player.GetPosition().x, br.y - player.GetNormalHeight() };
                    player.SetDeathPosition(spawnPos);
                    player.SetDyingInAir(false);
                    player.SetVelocityY(0.0f);
                    // ← Sin SetY: el sprite se queda donde está visualmente
                }
                else {
                    // Comportamiento normal para jugador vivo
                    player.SetY(br.y - player.GetHeight());
                    player.SetVelocityY(0.0f);
                    onGround = true;
                    pr = player.GetHitBox();
                }
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
                if (player.IsAlive() || !player.IsDyingInAir()) {
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

                        // ← check adicional con hitbox completa para bloques en L
                        // solo cuando no está en rampa y está en el suelo
                        if (player.GetRampGroundedFrames() == 0 && player.GetIsGrounded()) {
                            if (CheckCollisionRecs(pr, br)) {
                                float overlapLeft = (br.x + br.width) - pr.x;
                                float overlapRight = (pr.x + pr.width) - br.x;
                                if (overlapLeft < overlapRight) {
                                    player.SetX(player.GetX() + overlapLeft);
                                    player.SetLeftCollision(true);
                                }
                                else {
                                    player.SetX(player.GetX() - overlapRight);
                                    player.SetRightCollision(true);
                                }
                                pr = player.GetHitBox();
                            }
                        }
                    }
                }
            }
        }
        break;

        case TileType::PLATFORM:
        {
            float feetY = pr.y + pr.height;
            float prevFeetY = player.GetPreviousY() + player.GetHeight();
            bool  wasAbove = prevFeetY <= br.y + 1.0f;
            bool  overlapX = (pr.x + pr.width > br.x + 2.0f) &&
                (pr.x < br.x + br.width - 2.0f);
            float penetration = feetY - br.y;

            if (player.IsDyingInAir())
            {
                // Mismo tratamiento que SOLID para muerte en el aire
                bool wasAboveDying = prevFeetY <= br.y + 200.0f;
                float penetrationDying = feetY - br.y;
                if (overlapX && wasAboveDying && penetrationDying >= 0.0f && penetrationDying <= 99999.0f)
                {
                    Vector2 spawnPos = { player.GetPosition().x, br.y - player.GetNormalHeight() };
                    player.SetDeathPosition(spawnPos);
                    player.SetDyingInAir(false);
                    player.SetVelocityY(0.0f);
                }
            }
            else if (overlapX && wasAbove && player.GetVelocityY() >= 0 &&
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
                player.SetRampGroundedFrames(6);  // ← 3 frames de gracia
                pr = player.GetHitBox();
            }
            break;
        }
        }
    }
    // ── LOOKAHEAD DE RAMPA ──────────────────────────────────────
// Solo si está en el suelo y moviéndose a la izquierda
    if ((onGround || player.GetRampGroundedFrames() > 0) && player.GetVelocityX() < 0)
    {
        float nextCenterX = pr.x + pr.width / 2.0f + player.GetVelocityX();
        bool rampFound = false;

        for (const auto& rampCol : colliders)
        {
            if (rampCol.type != TileType::RAMP_UP) continue;
            const Rectangle& rbr = rampCol.rect;
            if (nextCenterX < rbr.x || nextCenterX > rbr.x + rbr.width) continue;
            float nextSurfaceY = rampCol.GetRampSurfaceY(nextCenterX);
            float feetY = pr.y + pr.height;
            if (fabsf(feetY - nextSurfaceY) <= 60.0f)
            {
                player.SetY(nextSurfaceY - player.GetHeight());
                player.SetVelocityY(3.0f);
                player.SetRampGroundedFrames(15);
                rampFound = true;
                break;
            }
        }

        // Si salió de la rampa, busca el SOLID más cercano debajo
        if (!rampFound && player.GetRampGroundedFrames() > 0)
        {
            for (const auto& solidCol : colliders)
            {
                if (solidCol.type != TileType::SOLID) continue;
                const Rectangle& sbr = solidCol.rect;
                float feetY = pr.y + pr.height;
                bool overlapX = (pr.x + pr.width > sbr.x) && (pr.x < sbr.x + sbr.width);
                if (overlapX && feetY <= sbr.y + 20.0f && feetY >= sbr.y - 20.0f)
                {
                    player.SetY(sbr.y - player.GetHeight());
                    player.SetVelocityY(0.0f);
                    onGround = true;
                    pr = player.GetHitBox();
                    break;
                }
            }
        }
    }
    // ───────────────────────────────────────────────────────────
    if (onGround) {
        player.SetGrounded(true);
    }
    else if (player.GetRampGroundedFrames() > 0) {
        player.SetGrounded(true);
    }
    else if (!player.IsDyingInAir()) {
        player.SetGrounded(false);  // ← solo resetea si NO está muriendo en el aire
    }
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
                    Rectangle hurtBox = s.GetHurtBox();
                    Vector2 bloodPos = { hurtBox.x + hurtBox.width / 2.0f, hurtBox.y + hurtBox.height / 2.0f };
                    creationManager.GetBloodEffects().emplace_back(bloodPos);
                    uiManager.AddScore(100);
                    hit = true;
                    break;
                }
            }
        }

        // Prisioneros
        if (!hit)
        {
            for (auto& p : creationManager.GetPrisoners())
            {
                if (!p.IsFreed() && CheckCollisionRecs(bIt->GetHitbox(), p.GetHitBox()))
                {
                    p.TakeDamage();
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
                if (item.IsActive() &&
                    item.GetType() == ItemType::BOX &&
                    !item.IsDestroyed() &&
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
        if (hit) {
            TraceLog(LOG_INFO, "BULLET hit block at x=%.1f y=%.1f", bIt->GetPosition().x, bIt->GetPosition().y);
            bIt = bullets.erase(bIt);
        }

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
                Rectangle hurtBox = soldier.GetHurtBox();
                Vector2 bloodPos = { hurtBox.x + hurtBox.width / 2.0f, hurtBox.y + hurtBox.height / 2.0f };
                creationManager.GetBloodEffects().emplace_back(bloodPos);
                uiManager.AddScore(100);
            }
        }

        // Cajas
        for (auto& item : creationManager.GetItems()) {
            if (!item.IsActive() || item.GetType() != ItemType::BOX || item.IsDestroyed()) continue;
            if (CheckCollisionRecs(grenade.GetExplosionHitBox(), item.GetHitBox())) {
                item.Destroy();
            }
        }
        // Boss
        if (!grenade.HasHitBoss() &&
            !boss.IsDestroyed() &&
            CheckCollisionRecs(boss.GetHitBox(), explosionBox))
        {
            boss.TakeDamage(20);
            grenade.SetHitBoss(true);  // ← solo una vez
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
        if (item.IsGrounded() || !item.HasGravity()) continue;
        if (!item.HasGravity()) continue;

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
                if (item.GetType() == ItemType::BOX) {
                    item.SetPositionY(br.y - item.GetVisualHeight()); 
                }
                else {
                    item.SetPositionY(br.y - item.GetVisualHeight());
                }
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
        bool playerTouching = CheckCollisionRecs(item.GetHitBox(), player.GetHitBox());
        if (item.GetType() == ItemType::SHOTGUN &&
            playerTouching)
        {
            player.EquipMachinegun();
            item.Collect();
            audioManager.PlaySound(audioManager.GetMachinegunEquipSound());
            uiManager.SetAmmo(player.GetAmmo());
            uiManager.SetWeaponDisplay(UiManager::WeaponDisplay::MACHINEGUN);
            Vector2 popupPos = { item.GetPosition().x, item.GetPosition().y - 30.0f };
            creationManager.GetFloatingTexts().emplace_back(popupPos, "HEAVY MACHINE GUN");
        }
        if (playerTouching && item.GetType() != ItemType::BOX)
        {
            switch (item.GetType()) {
            case ItemType::PLUSHY:
                uiManager.AddScore(200);
                break;
            case ItemType::FISH:
                uiManager.AddScore(500);
                break;
            case ItemType::MEDAL:
                uiManager.AddScore(1000);
                break;
            case ItemType::PIG:
                uiManager.AddScore(300);
                break;
            case ItemType::BOMBS:
                uiManager.SetBombs(10);
                break;
            case ItemType::JETPACK:
                // Equipar jetpack al jugador y actualizar UI inmediatamente
                player.EquipJetpack();
                uiManager.SetJetpackActive(true);
                uiManager.SetJetpackFuel(player.GetJetpackFuel() / player.GetJetpackMaxFuel());
                break;
            default:
                break;
            }
            item.Collect();
            Vector2 popupPos = { item.GetPosition().x, item.GetPosition().y - 30.0f };
            creationManager.GetFloatingTexts().emplace_back(popupPos, "JETPACK");
            continue;
        }

        // (El resto del código permanece igual: spawn items, erase inactivos, etc.)
        if (item.ShouldSpawnItem())
        {
            item.ConsumeSpawn();
            Vector2 spawnPos = { item.GetHitBox().x + 60.0f, item.GetHitBox().y - 20.0f };
            Item newItem(spawnPos, ItemType::SHOTGUN);
            newItem.SetGravity(true);
            items.push_back(newItem);
        }

        items.erase(std::remove_if(items.begin(), items.end(),
            [](const Item& i) { return !i.IsActive(); }), items.end());
    }
}

void SystemCollision::BossAttackPlayerCollision()
{
    if (!boss.IsActive() && !boss.IsInIntro()) return;
    if (player.IsInvincible() || !player.IsAlive()) return;

    Rectangle playerBox = player.GetHitBox();

    // Bolas de plasma fase 1
    for (int i = 0; i < 3; i++) {
        if (!boss.GetPlasmaActive(i)) continue;
        Rectangle plasmaBox = {
            boss.GetPlasmaPos(i).x - 8.0f,
            boss.GetPlasmaPos(i).y - 8.0f,
            16.0f, 16.0f
        };
        if (CheckCollisionRecs(playerBox, plasmaBox)) {
            player.TakeDamage();
            return;
        }
    }
    if (boss.IsLaserBeamActive()) {
        Rectangle beamBox = boss.GetLaserBeamHitBox();
        TraceLog(LOG_INFO, "BEAM ABAJO active beamBox x=%.0f y=%.0f w=%.0f h=%.0f playerX=%.0f playerY=%.0f",
            beamBox.x, beamBox.y, beamBox.width, beamBox.height, playerBox.x, playerBox.y);
        if (CheckCollisionRecs(playerBox, beamBox)) {
            TraceLog(LOG_INFO, "BEAM ABAJO MATA");
            player.TakeDamage();
            return;
        }
    }

    if (boss.IsBeamUpActive()) {
        Rectangle beamBox = boss.GetBeamUpHitBox();
        TraceLog(LOG_INFO, "BEAM ARRIBA active beamBox x=%.0f y=%.0f w=%.0f h=%.0f playerX=%.0f playerY=%.0f",
            beamBox.x, beamBox.y, beamBox.width, beamBox.height, playerBox.x, playerBox.y);
        if (CheckCollisionRecs(playerBox, beamBox)) {
            TraceLog(LOG_INFO, "BEAM ARRIBA MATA");
            player.TakeDamage();
            return;
        }
    }
}

void SystemCollision::PlayerBoxCollision()
{
    if (!player.IsAlive()) return;

    Rectangle pr = player.GetHitBox();
    float prevFeetY = player.GetPreviousY() + player.GetHeight();

    for (auto& item : creationManager.GetItems())
    {
        if (!item.IsActive() || item.IsDestroyed()) continue;
        if (item.GetType() != ItemType::BOX) continue;

        Rectangle br = item.GetHitBox();  // hitbox amarilla

        // ── COLISIÓN VERTICAL (encima) ──────────────────
        float feetY = pr.y + pr.height;
        bool  overlapX = (pr.x + pr.width > br.x) && (pr.x < br.x + br.width);
        float penetration = feetY - br.y;
        bool  wasAbove = prevFeetY <= br.y + 2.0f;

        if (overlapX && wasAbove && player.GetVelocityY() >= 0 &&
            penetration >= 0.0f && penetration <= 45.0f)
        {
            player.SetY(br.y - player.GetHeight());
            player.SetVelocityY(0.0f);
            player.SetGrounded(true);
            continue;
        }

        // ── COLISIÓN LATERAL ────────────────────────────
        if (!CheckCollisionRecs(pr, br)) continue;

        // Viene por la izquierda
        if (pr.x + pr.width > br.x && pr.x < br.x)
        {
            player.SetX(br.x - pr.width - player.GetHitBox().x + player.GetX());
            player.SetVelocityX(0.0f);
        }
        // Viene por la derecha
        else if (pr.x < br.x + br.width && pr.x + pr.width > br.x + br.width)
        {
            player.SetX(br.x + br.width - player.GetHitBox().x + player.GetX());
            player.SetVelocityX(0.0f);
        }
    }
}

void SystemCollision::PrisonerBlockCollision()
{
    const auto& colliders = creationManager.GetTileMap().GetColliders();
    for (auto& p : creationManager.GetPrisoners())
    {
        if (!p.IsActive()) continue;
        Rectangle pr = p.GetCollisionHitBox();

        for (const auto& col : colliders)
        {
            if (col.type != TileType::SOLID && col.type != TileType::PLATFORM) continue;
            const Rectangle& br = col.rect;
            float feetY = pr.y + pr.height;
            bool overlapX = (pr.x + pr.width > br.x) && (pr.x < br.x + br.width);
            float penetration = feetY - br.y;

            if (overlapX && penetration >= 0.0f && penetration <= 30.0f)
            {
                p.SetPositionY(br.y - pr.height);
                p.SetGrounded(true);
                pr = p.GetCollisionHitBox();
            }
        }
    }
}