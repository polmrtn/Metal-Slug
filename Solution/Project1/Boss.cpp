#include "Boss.hpp"
#include "GlobalManagers.hpp"

Boss::Boss() {}
Boss::~Boss() {}

void Boss::Init()
{
    Image img = LoadImage("Graphics/boss/cannon56x96.png");
    cannonSheet = LoadTextureFromImage(img);
    UnloadImage(img);
    SetTextureFilter(cannonSheet, TEXTURE_FILTER_POINT);

    Image imgFlash = LoadImage("Graphics/boss/explo64x64.png");
    flashSheet = LoadTextureFromImage(imgFlash);
    UnloadImage(imgFlash);
    SetTextureFilter(flashSheet, TEXTURE_FILTER_POINT);

    cannonFrame = 0;
    cannonGoingUp = true;
    cannonFrameTimer = 0.0f;
    cannonState = CannonState::MOVING;
    stateTimer = 0.0f;
    openFrame = 0;
    closeFrame = 0;
    flashActive = false;
    flashFrame = 0;
}

void Boss::Update(float playerX)
{
    if (!active) {
        if (playerX >= 14000.0f) active = true;
        else return;
    }

    // Hit flash
    if (isFlashing) {
        hitFlashTimer += GetFrameTime();
        if (hitFlashTimer >= HIT_FLASH_DURATION) {
            hitFlashTimer = 0.0f;
            hitFlashCount++;
            if (hitFlashCount >= HIT_FLASH_COUNT) {
                isFlashing = false;
                hitFlashCount = 0;
            }
        }
    }

    UpdatePlasma(GetFrameTime());
    UpdateCannon(GetFrameTime());
}

void Boss::UpdateCannon(float dt)
{
    cannonFrameTimer += dt;

    switch (cannonState)
    {
        // ── MOVING ───────────────────────────────────────────────
    case CannonState::MOVING:
        flashActive = false;
        if (cannonFrameTimer < cannonFrameDelay) return;
        cannonFrameTimer = 0.0f;

        if (cannonGoingUp) {
            cannonFrame++;
            if (cannonFrame >= CANNON_FRAMES - 1) {
                cannonFrame = CANNON_FRAMES - 1;
                cannonState = CannonState::OPENING_UP;
                openFrame = 0;
                cannonFrameTimer = 0.0f;
            }
        }
        else {
            cannonFrame--;
            if (cannonFrame <= 0) {
                cannonFrame = 0;
                cannonState = CannonState::OPENING_DOWN;
                openFrame = 0;
                cannonFrameTimer = 0.0f;
            }
        }
        break;

        // ── ABAJO ────────────────────────────────────────────────
    case CannonState::OPENING_DOWN:
        flashActive = false;
        if (cannonFrameTimer < cannonFrameDelay) return;
        cannonFrameTimer = 0.0f;
        openFrame++;
        if (openFrame >= OPEN_DOWN_FRAMES - 1) {
            openFrame = 0;
            cannonState = CannonState::SHOOTING_DOWN;
            cannonFrameTimer = 0.0f;
        }
        break;

    case CannonState::SHOOTING_DOWN:
        if (cannonFrameTimer < cannonFrameDelay) return;
        cannonFrameTimer = 0.0f;
        openFrame++;

        // Flash sincronizado: frame 0-5 del disparo = frame 0-5 del flash
        if (openFrame < FLASH_FRAMES) {
            flashActive = true;
            flashFrame = openFrame;
        }
        else {
            flashActive = false;
        }

        if (openFrame >= SHOOT_DOWN_FRAMES - 1) {
            if (shootRepeatCount == 0)
                capturedPlayerX = player.GetPosition().x;
            FirePlasma();
            shootRepeatCount++;
            openFrame = 0;
            if (shootRepeatCount >= 3) {
                shootRepeatCount = 0;
                openFrame = OPEN_DOWN_FRAMES - 1;
                cannonState = CannonState::CLOSING_DOWN;
                flashActive = false;
            }
            cannonFrameTimer = 0.0f;
        }
        break;

    case CannonState::CLOSING_DOWN:
        flashActive = false;
        if (cannonFrameTimer < cannonFrameDelay) return;
        cannonFrameTimer = 0.0f;
        openFrame--;
        if (openFrame <= 0) {
            openFrame = 0;
            cannonGoingUp = true;
            cannonFrame = 0;
            cannonState = CannonState::MOVING;
            cannonFrameTimer = 0.0f;
        }
        break;

        // ── ARRIBA ───────────────────────────────────────────────
    case CannonState::OPENING_UP:
        flashActive = false;
        if (cannonFrameTimer < cannonFrameDelay) return;
        cannonFrameTimer = 0.0f;
        openFrame++;
        if (openFrame >= OPEN_UP_FRAMES - 1) {
            openFrame = 0;
            cannonState = CannonState::SHOOTING_UP;
            cannonFrameTimer = 0.0f;
        }
        break;

    case CannonState::SHOOTING_UP:
        if (cannonFrameTimer < cannonFrameDelay) return;
        cannonFrameTimer = 0.0f;
        openFrame++;

        // Flash sincronizado
        if (openFrame < FLASH_FRAMES) {
            flashActive = true;
            flashFrame = openFrame;
        }
        else {
            flashActive = false;
        }

        if (openFrame >= SHOOT_UP_FRAMES - 1) {
            if (shootRepeatCount == 0)
                capturedPlayerX = player.GetPosition().x;
            FirePlasma();
            shootRepeatCount++;
            openFrame = 0;
            if (shootRepeatCount >= 3) {
                shootRepeatCount = 0;
                openFrame = OPEN_UP_FRAMES - 1;
                cannonState = CannonState::CLOSING_UP;
                flashActive = false;
            }
            cannonFrameTimer = 0.0f;
        }
        break;

    case CannonState::CLOSING_UP:
        flashActive = false;
        if (cannonFrameTimer < cannonFrameDelay) return;
        cannonFrameTimer = 0.0f;
        openFrame--;
        if (openFrame <= 0) {
            openFrame = 0;
            cannonGoingUp = false;
            cannonFrame = CANNON_FRAMES - 1;
            cannonState = CannonState::MOVING;
            cannonFrameTimer = 0.0f;
        }
        break;
    }
}

void Boss::Draw()
{
    if (!active) return;

    // ── Cañón ─────────────────────────────────────────────────
    Rectangle src;
    switch (cannonState)
    {
    case CannonState::OPENING_DOWN:
    case CannonState::CLOSING_DOWN:
        src = { openFrame * CANNON_FRAME_W, OPEN_DOWN_ROW_Y,  CANNON_FRAME_W, CANNON_FRAME_H };
        break;
    case CannonState::SHOOTING_DOWN:
        src = { openFrame * CANNON_FRAME_W, SHOOT_DOWN_ROW_Y, CANNON_FRAME_W, CANNON_FRAME_H };
        break;
    case CannonState::OPENING_UP:
    case CannonState::CLOSING_UP:
        src = { openFrame * CANNON_FRAME_W, OPEN_UP_ROW_Y,    CANNON_FRAME_W, CANNON_FRAME_H };
        break;
    case CannonState::SHOOTING_UP:
        src = { openFrame * CANNON_FRAME_W, SHOOT_UP_ROW_Y,   CANNON_FRAME_W, CANNON_FRAME_H };
        break;
    default:
        src = { cannonFrame * CANNON_FRAME_W, 0.0f, CANNON_FRAME_W, CANNON_FRAME_H };
        break;
    }

    Rectangle dst = { posX, posY, CANNON_FRAME_W * CANNON_SCALE, CANNON_FRAME_H * CANNON_SCALE };
    Color tint = (isFlashing && hitFlashCount % 2 == 0) ? Color{ 255, 220, 100, 255 } : WHITE;
    DrawTexturePro(cannonSheet, src, dst, { 0, 0 }, 0.0f, tint);

    // ── Destello ──────────────────────────────────────────────
    if (flashActive) {
        Rectangle flashSrc = {
            flashFrame * FLASH_FRAME_W,
            FLASH_ROW_Y,
            FLASH_FRAME_W,
            FLASH_FRAME_H
        };

        bool isDown = (cannonState == CannonState::SHOOTING_DOWN);

        float fx, fy;
        if (isDown) {
            fx = 16220.0f + 50.0f - (FLASH_FRAME_W * FLASH_SCALE) / 2.0f;  // igual de derecha
            fy = 285.0f - (FLASH_FRAME_H * FLASH_SCALE) / 2.0f;    // más arriba
        }
        else {
            fx = 16220.0f + 70.0f - (FLASH_FRAME_W * FLASH_SCALE) / 2.0f;  // más a la derecha
            fy = posY + 30 - (FLASH_FRAME_H * FLASH_SCALE) / 2.0f;               // igual
        }

        Rectangle flashDst = { fx, fy, FLASH_FRAME_W * FLASH_SCALE, FLASH_FRAME_H * FLASH_SCALE };
        DrawTexturePro(flashSheet, flashSrc, flashDst, { 0, 0 }, 0.0f, WHITE);
    }

    DrawPlasma();
}

void Boss::FirePlasma()
{
    for (int i = 0; i < MAX_PLASMA; i++) {
        if (plasma[i].active) continue;

        float spawnX = posX + (CANNON_FRAME_W * CANNON_SCALE) * 0.3f;
        float spawnY = (cannonState == CannonState::SHOOTING_DOWN ||
            cannonState == CannonState::OPENING_DOWN ||
            cannonState == CannonState::CLOSING_DOWN)
            ? 285.0f
            : posY;

        plasma[i].pos = { spawnX, spawnY };

        float targetX = capturedPlayerX +
            (float)GetRandomValue(-(int)plasmaSpread, (int)plasmaSpread);

        // Detectar suelo bajo targetX
        float groundY = player.GetPosition().y + 300.0f;
        const auto& colliders = creationManager.GetTileMap().GetColliders();
        for (const auto& col : colliders) {
            if (col.type == TileType::CEILING) continue;
            if (col.rect.x <= targetX && col.rect.x + col.rect.width >= targetX) {
                if (col.rect.y > spawnY && col.rect.y < groundY)
                    groundY = col.rect.y;
            }
        }

        float dy = groundY - spawnY;
        float timeToLand = 1.5f;
        float g = plasma[i].gravity;

        plasma[i].vel.x = (targetX - spawnX) / timeToLand;
        plasma[i].vel.y = (dy - 0.5f * g * timeToLand * timeToLand) / timeToLand;
        plasma[i].active = true;
        break;
    }
}

void Boss::UpdatePlasma(float dt)
{
    const auto& colliders = creationManager.GetTileMap().GetColliders();

    for (int i = 0; i < MAX_PLASMA; i++) {
        if (!plasma[i].active) continue;

        plasma[i].vel.y += plasma[i].gravity * dt;
        plasma[i].pos.x += plasma[i].vel.x * dt;
        plasma[i].pos.y += plasma[i].vel.y * dt;

        Rectangle ball = {
            plasma[i].pos.x - plasmaRadius,
            plasma[i].pos.y - plasmaRadius,
            plasmaRadius * 2,
            plasmaRadius * 2
        };

        for (const auto& col : colliders) {
            if (col.type == TileType::CEILING) continue;
            if (CheckCollisionRecs(ball, col.rect)) {
                plasma[i].active = false;
                break;
            }
        }
    }
}

void Boss::DrawPlasma() const
{
    for (int i = 0; i < MAX_PLASMA; i++) {
        if (!plasma[i].active) continue;
        DrawCircleV(plasma[i].pos, plasmaRadius, { 180, 60, 255, 255 });
        DrawCircleLines((int)plasma[i].pos.x, (int)plasma[i].pos.y,
            plasmaRadius, { 255, 180, 255, 255 });
    }
}