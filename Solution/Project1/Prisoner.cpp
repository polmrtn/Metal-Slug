#include "Prisoner.hpp"
#include "GlobalManagers.hpp"
#include <cmath>

Texture2D Prisoner::texture = { 0 };
bool      Prisoner::textureLoaded = false;

Prisoner::Prisoner(Vector2 position, PrisonerType type)
    : position(position), type(type)
{
    if (!textureLoaded) {
        texture = LoadTexture("Graphics/prisoner.png");
        SetTextureFilter(texture, TEXTURE_FILTER_POINT);
        textureLoaded = true;
    }
}

Prisoner::~Prisoner() {}

Rectangle Prisoner::GetHitBox() const {
    float w, h;
    switch (state) {
    case PrisonerState::IDLE:
        w = (type == PrisonerType::GROUND) ? 43.0f : 44.0f;
        h = (type == PrisonerType::GROUND) ? 35.0f : 48.0f;
        break;
    case PrisonerState::FREEING:
        w = (type == PrisonerType::GROUND) ? 43.0f : 46.0f;
        h = (type == PrisonerType::GROUND) ? 35.0f : 48.0f;
        break;
    case PrisonerState::WALKING:
        w = 33.0f; h = 41.0f;
        break;
    case PrisonerState::REWARD:
    case PrisonerState::SALUTE:
    case PrisonerState::RUNNING:
        w = 44.0f; h = 41.0f;
        break;
    default:
        w = 43.0f; h = 35.0f;
        break;
    }
    return { position.x, position.y, w * scale, h * scale };
}

void Prisoner::TakeDamage() {
    if (state != PrisonerState::IDLE) return;
    state = PrisonerState::FREEING;
    frame = 0;
    timer = 0.0f;
    pingpongForward = true;
}

void Prisoner::DrawFrame(float srcX, float srcY, float srcW, float srcH) const {
    float drawX = facingRight ? srcW : -srcW;
    Rectangle src = { srcX, srcY, drawX, srcH };
    Rectangle dst = { position.x, position.y, srcW * scale, srcH * scale };
    DrawTexturePro(texture, src, dst, { 0,0 }, 0, WHITE);
}

void Prisoner::SpawnRewardItem() {
    // Spawna heavy machine gun
    Vector2 itemPos = { position.x, position.y - 30.0f };
    creationManager.GetItems().emplace_back(itemPos, ItemType::SHOTGUN);
    auto& items = creationManager.GetItems();
    items.back().SetGravity(true);
}

void Prisoner::UpdateIdle(float dt) {
    timer += dt;
    if (timer < FRAME_DELAY) return;
    timer = 0.0f;

    int maxFrames = (type == PrisonerType::GROUND) ? 5 : 9;

    if (pingpongForward) {
        frame++;
        if (frame >= maxFrames - 1) pingpongForward = false;
    }
    else {
        frame--;
        if (frame <= 0) pingpongForward = true;
    }
}

void Prisoner::UpdateFreeing(float dt) {
    timer += dt;
    if (timer < FRAME_DELAY) return;
    timer = 0.0f;

    if (type == PrisonerType::GROUND) {
        // frames 5-8 una vez
        frame++;
        if (frame >= 4) {
            frame = 0;
            state = PrisonerState::WALKING;
        }
    }
    else {
        // POLE: fila 2, 10 frames una vez
        frame++;
        if (frame >= 10) {
            frame = 0;
            state = PrisonerState::WALKING;
        }
    }
}

void Prisoner::UpdateWalking(float playerX, float dt) {
    // Inicializar targets la primera vez
    if (!walkInitialized) {
        walkTarget = position.x + WALK_RANGE;
        walkInitialized = true;
    }

    float dir = (walkTarget > position.x) ? 1.0f : -1.0f;
    facingRight = (dir > 0);
    position.x += dir * WALK_SPEED * dt;

    // Cambiar target cuando llega
    if (fabsf(position.x - walkTarget) < 5.0f) {
        walkTarget = (dir > 0) ? position.x - WALK_RANGE : position.x + WALK_RANGE;
    }

    timer += dt;
    if (timer >= FRAME_DELAY) {
        timer = 0.0f;
        frame = (frame + 1) % 12;
    }

    // Colisión con player
    if (CheckCollisionRecs(GetHitBox(), player.GetHitBox())) {
        state = PrisonerState::REWARD;
        frame = 0;
        pingpongForward = true;
        timer = 0.0f;
    }
}

void Prisoner::UpdateReward(float playerX, float dt) {
    timer += dt;
    if (timer < FRAME_DELAY) return;
    timer = 0.0f;

    if (pingpongForward) {
        frame++;
        if (frame >= 11) {
            frame = 10;
            pingpongForward = false;
            if (!rewardGiven) {
                SpawnRewardItem();
                rewardGiven = true;
            }
        }
    }
    else {
        frame--;
        if (frame <= 0) {
            frame = 0;
            state = PrisonerState::SALUTE;
            timer = 0.0f;
        }
    }
}

void Prisoner::UpdateSalute(float dt) {
    timer += dt;
    if (timer < FRAME_DELAY) return;
    timer = 0.0f;

    frame++;
    if (frame >= 14) {
        frame = 0;
        state = PrisonerState::RUNNING;
        facingRight = false;
    }
}

void Prisoner::UpdateRunning(float cameraLeft, float dt) {
    position.x -= RUN_SPEED * dt;
    timer += dt;
    if (timer >= FRAME_DELAY) {
        timer = 0.0f;
        frame = (frame + 1) % 8;
    }
    if (position.x < cameraLeft - 200.0f)
        active = false;
}

void Prisoner::Update(float playerX, float cameraLeft, float cameraRight) {
    float dt = GetFrameTime();

    // Gravedad
    if (!grounded) {
        velY += GRAVITY * dt;
        position.y += velY * dt;
    }
    else {
        velY = 0.0f;
    }
    grounded = false;

    switch (state) {
    case PrisonerState::IDLE:     UpdateIdle(dt);              break;
    case PrisonerState::FREEING:  UpdateFreeing(dt);           break;
    case PrisonerState::WALKING:  UpdateWalking(playerX, dt);  break;
    case PrisonerState::REWARD:   UpdateReward(playerX, dt);   break;
    case PrisonerState::SALUTE:   UpdateSalute(dt);            break;
    case PrisonerState::RUNNING:  UpdateRunning(cameraLeft, dt); break;
    default: break;
    }
}

void Prisoner::Draw() const {
    if (!active) return;

    float srcX, srcY, srcW, srcH;

    switch (state) {
    case PrisonerState::IDLE:
        if (type == PrisonerType::GROUND) {
            srcX = frame * 43.0f; srcY = 0.0f;   srcW = 43.0f; srcH = 35.0f;
        }
        else {
            srcX = frame * 44.0f; srcY = 48.0f;  srcW = 44.0f; srcH = 48.0f;
        }
        break;
    case PrisonerState::FREEING:
        if (type == PrisonerType::GROUND) {
            srcX = (frame + 5) * 43.0f; srcY = 0.0f;  srcW = 43.0f; srcH = 35.0f;
        }
        else {
            srcX = frame * 46.0f; srcY = 96.0f; srcW = 46.0f; srcH = 48.0f;
        }
        break;
    case PrisonerState::WALKING:
        srcX = frame * 33.0f; srcY = 41.0f * 4; srcW = 33.0f; srcH = 41.0f;
        break;
    case PrisonerState::REWARD:
        srcX = frame * 44.0f; srcY = 41.0f * 5; srcW = 44.0f; srcH = 41.0f;
        break;
    case PrisonerState::SALUTE:
        srcX = frame * 44.0f; srcY = 41.0f * 6; srcW = 44.0f; srcH = 41.0f;
        break;
    case PrisonerState::RUNNING:
        srcX = frame * 44.0f; srcY = 41.0f * 7; srcW = 44.0f; srcH = 41.0f;
        break;
    default:
        return;
    }

    bool flip = facingRight;
    Rectangle src = { srcX, srcY, flip ? -srcW : srcW, srcH };
    Rectangle dst = { position.x, position.y, srcW * scale, srcH * scale };
    DrawTexturePro(texture, src, dst, { 0,0 }, 0, WHITE);
    DrawRectangleLinesEx(GetHitBox(), 2.0f, PURPLE);
}