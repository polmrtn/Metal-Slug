#include "Boss.hpp"

Boss::Boss() {}
Boss::~Boss() {}

void Boss::Init()
{
    Image img = LoadImage("Graphics/boss/cannon56x96.png");
    cannonSheet = LoadTextureFromImage(img);
    UnloadImage(img);
    SetTextureFilter(cannonSheet, TEXTURE_FILTER_POINT);

    cannonFrame = 0;
    cannonGoingUp = true;
    cannonFrameTimer = 0.0f;
    cannonState = CannonState::MOVING;
    stateTimer = 0.0f;
    openFrame = 0;
    closeFrame = 0;
}

void Boss::Update(float playerX)
{
    if (!active) {
        if (playerX >= 14000.0f) active = true;
        else return;
    }

    UpdateCannon(GetFrameTime());
}

void Boss::UpdateCannon(float dt)
{
    cannonFrameTimer += dt;

    switch (cannonState)
    {
    case CannonState::MOVING:
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

        // ── ABAJO ─────────────────────────────────────────────────
    case CannonState::OPENING_DOWN:
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
        if (openFrame >= SHOOT_DOWN_FRAMES - 1) {
            // TODO: disparar
            openFrame = OPEN_DOWN_FRAMES - 1;  // empieza desde el último
            cannonState = CannonState::CLOSING_DOWN;
            cannonFrameTimer = 0.0f;
        }
        break;

    case CannonState::CLOSING_DOWN:
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

        // ── ARRIBA ────────────────────────────────────────────────
    case CannonState::OPENING_UP:
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
        if (openFrame >= SHOOT_UP_FRAMES - 1) {
            // TODO: disparar
            openFrame = OPEN_UP_FRAMES - 1;  // empieza desde el último
            cannonState = CannonState::CLOSING_UP;
            cannonFrameTimer = 0.0f;
        }
        break;

    case CannonState::CLOSING_UP:
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

    Rectangle src;

    switch (cannonState)
    {
    case CannonState::OPENING_DOWN:
    case CannonState::CLOSING_DOWN:
        src = { openFrame * CANNON_FRAME_W, OPEN_DOWN_ROW_Y, CANNON_FRAME_W, CANNON_FRAME_H };
        break;
    case CannonState::SHOOTING_DOWN:
        src = { openFrame * CANNON_FRAME_W, SHOOT_DOWN_ROW_Y, CANNON_FRAME_W, CANNON_FRAME_H };
        break;
    case CannonState::OPENING_UP:
    case CannonState::CLOSING_UP:
        src = { openFrame * CANNON_FRAME_W, OPEN_UP_ROW_Y, CANNON_FRAME_W, CANNON_FRAME_H };
        break;
    case CannonState::SHOOTING_UP:
        src = { openFrame * CANNON_FRAME_W, SHOOT_UP_ROW_Y, CANNON_FRAME_W, CANNON_FRAME_H };
        break;
    default:
        src = { cannonFrame * CANNON_FRAME_W, 0.0f, CANNON_FRAME_W, CANNON_FRAME_H };
        break;
    }

    Rectangle dst = {
        posX, posY,
        CANNON_FRAME_W * CANNON_SCALE,
        CANNON_FRAME_H * CANNON_SCALE
    };
    DrawTexturePro(cannonSheet, src, dst, { 0, 0 }, 0.0f, WHITE);
}