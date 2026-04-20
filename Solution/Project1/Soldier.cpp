#include "Soldier.hpp"
#include "Player.hpp"
#include <raylib.h>
int direction = 1;

Soldier::Soldier(int type, Vector2 position)
{
    this->type = type;
    this->position = position;
    this->scale = 4.3f;
    this->isGrounded = false;
    this->velocity.y = 0;
    this->velocity.x = 0;
    this->gravity = 0.8f;
    this->isAlive = true;
    this->facingRight = true;
    this->currentState = SoldierState::IDLE;
    this->stateTimer = 0;
    this->hasShot = false;
    this->wantsToShoot = false;

    switch (type)
    {
    case 1:
        soldierAnim.LoadTexture();
        break;
    default:
        soldierAnim.LoadTexture();
        break;
    }
    SetTextureFilter(soldierAnim.GetSheet(), TEXTURE_FILTER_POINT);
}

Soldier::Soldier(const Soldier& other)
{
    type = other.type;
    position = other.position;
    isGrounded = other.isGrounded;
    velocity = other.velocity;
    gravity = other.gravity;
    scale = other.scale;
    currentState = other.currentState;
    stateTimer = other.stateTimer;
    isAlive = other.isAlive;
    facingRight = other.facingRight;
    hasShot = other.hasShot;
    wantsToShoot = other.wantsToShoot;
    image = soldierAnim.GetSheet();
    SetTextureFilter(image, TEXTURE_FILTER_POINT);
}

Soldier::~Soldier() {}

Rectangle Soldier::GetHurtBox()
{
    return Rectangle{ position.x + GetWidth() / 3, position.y + GetHeight() / 2, GetWidth() / 3, GetHeight() / 2 };
}

Rectangle Soldier::GetHitBox() {
    float rayLength = 0;
    float rayX = 0;

    if (GetType() == 1) {
        rayLength = 50.f;
        if (!facingRight) {
            rayX = position.x - rayLength + rayLength;
        }
        else {
            rayX = position.x + rayLength * 2;
        }
    }
    else if (GetType() == 2) {
        rayLength = 300.f;
        if (!facingRight) {
            rayX = position.x - rayLength + 50;
        }
        else {
            rayX = position.x + 100;
        }
    }

    return Rectangle{ rayX, position.y + GetHeight() / 2, rayLength, GetHeight() / 2 };
}

void Soldier::SetSoliderState(SoldierState newState)
{
    if (currentState == SoldierState::DEAD) return;
    if (currentState == newState) return;
    currentState = newState;
}

bool Soldier::IsVisionRay(Player& player)
{
    if (!player.IsAlive()) return false;
    return CheckCollisionRecs(GetHitBox(), player.GetHitBox());
}

void Soldier::DrawHitBox()
{
    DrawRectangleLinesEx(GetHitBox(), 2, RED);
    DrawRectangleLinesEx(GetHurtBox(), 2, WHITE);
}

void Soldier::Draw() {
    Rectangle src = soldierAnim.GetSourceRect();
    if (facingRight) src.width = -src.width;
    float destW;
    float offsetX;

    if (soldierAnim.GetCurrentAnim() == SoldierState::ATTACKING) {
        destW = (34.f + 34.f) * scale;
        if (!facingRight) {
            if (GetType() == 1) offsetX = -138.f;
            else if (GetType() == 2) offsetX = -250;
        }
        else {
            offsetX = 0.f;
        }
    }
    else if (soldierAnim.GetCurrentAnim() == SoldierState::DEAD) {
        destW = 34.f * scale;
        offsetX = 0.f;
    }
    else if (soldierAnim.GetCurrentAnim() == SoldierState::BOMB && GetType() == 2) {
        destW = 44.f * scale;
        offsetX = 0.f;
    }
    else {
        destW = 34.f * scale;
        offsetX = 0.f;
    }

    Rectangle dest = { position.x + offsetX, position.y, destW, 68.f * scale };
    DrawTexturePro(soldierAnim.GetSheet(), src, dest, { 0, 0 }, 0.f, WHITE);
    DrawHitBox();
}

int Soldier::GetType() {
    return type;
}

void Soldier::Update()
{
    if (!isGrounded) {
        velocity.y += gravity;
    }
    else {
        velocity.y = 0;
    }

    // Disparar granada en el peak de la animación BOMB
    if (type == 2 && currentState == SoldierState::ATTACKING) {
        int peak = soldierAnim.GetCurrentClipFrames() - 1;
        int triggerFrame = peak - 5;
        if (soldierAnim.GetFrame() >= triggerFrame && !hasShot) {
            wantsToShoot = true;
            hasShot = true;
        }
    }

    position.y += velocity.y;
    position.x += velocity.x;
    soldierAnim.Update();
}

void Soldier::Attack(Player& player) {
    if (!player.IsAlive()) return;
    if (IsVisionRay(player)) {
        if (!player.IsInvincible()) {
            TraceLog(LOG_INFO, "Soldier attacked and HIT the player!");
            player.TakeDamage();
        }
        else {
            TraceLog(LOG_INFO, "Soldier attacked but player is invincible");
        }
    }
}

void Soldier::UpdateAI(Player& player)
{
    stateTimer += GetFrameTime();

    if ((stateTimer >= 3.0f && !IsVisionRay(player) && isAlive) ||
        (soldierAnim.IsAnimationFinished() && currentState != SoldierState::DEAD)) {
        stateTimer = 0.0f;
        direction = (GetRandomValue(0, 1) == 0) ? -1 : 1;
        int randomBehaviour = GetRandomValue(0, 2);

        switch (randomBehaviour)
        {
        case 0:
            SetSoliderState(SoldierState::IDLE);
            soldierAnim.SetAnimation(SoldierState::IDLE);
            break;
        case 1:
            SetSoliderState(SoldierState::WALKING);
            soldierAnim.SetAnimation(SoldierState::WALKING);
            break;
        case 2:
            SetSoliderState(SoldierState::SNEAK);
            soldierAnim.SetAnimation(SoldierState::SNEAK);
            break;
        }
    }
    else if (IsVisionRay(player) && isAlive) {
        if (currentState != SoldierState::ATTACKING) {
            if (GetType() == 1) {
                SetSoliderState(SoldierState::ATTACKING);
                soldierAnim.ForceAnimation(SoldierState::ATTACKING);
                attackTriggered = false;
            }
            else if (GetType() == 2) {
                SetSoliderState(SoldierState::ATTACKING);
                soldierAnim.ForceAnimation(SoldierState::BOMB);
                // ← NO wantsToShoot aquí, se activa en Update() en el peak
            }
        }
        attackTimer = 0.0f;
    }
    else if (!isAlive && currentState != SoldierState::DEAD) {
		hasShot = false;
        SetSoliderState(SoldierState::DEAD);
    }

    if (currentState == SoldierState::IDLE && isGrounded && isAlive) {
        hasShot = false;
        velocity.x = 0;
    }
    else if (currentState == SoldierState::WALKING && isGrounded && isAlive) {
        hasShot = false;
        if (direction == 1) {
            velocity.x = 5;
            facingRight = true;
        }
        else {
            velocity.x = -5;
            facingRight = false;
        }
    }
    else if (currentState == SoldierState::ATTACKING && isAlive && isGrounded) {
        velocity.x = 0;

        if (GetType() == 1) {
            if (soldierAnim.IsAttackPeak() && !attackTriggered) {
                attackTriggered = true;
                Attack(player);
            }
            if (soldierAnim.IsAnimationFinished()) {
                SetSoliderState(SoldierState::IDLE);
                attackTriggered = false;
            }
        }
        else if (GetType() == 2) {
            if (soldierAnim.IsAnimationFinished()) {
                hasShot = false; // ← resetear para el próximo ataque
                SetSoliderState(SoldierState::IDLE);
            }
        }
        return;
    }
    else if (currentState == SoldierState::DEAD && !isAlive) {
        soldierAnim.SetAnimation(SoldierState::DEAD);
        velocity.x = 0;
    }
    else if (currentState == SoldierState::SNEAK && isAlive && isGrounded) {
        if (direction == 1) {
            velocity.x = 2;
            facingRight = true;
        }
        else {
            velocity.x = -2;
            facingRight = false;
        }
    }
}

void Soldier::TriggerDeath() {
    if (!isAlive) return;
    isAlive = false;
    currentState = SoldierState::DEAD;
    soldierAnim.ForceAnimation(SoldierState::DEAD);
    velocity.x = 0;
}