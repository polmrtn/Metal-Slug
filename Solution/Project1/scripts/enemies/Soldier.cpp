#include "Soldier.hpp"
#include "Player.hpp"
#include "AudioManager.hpp"
#include <raylib.h>
#include <math.h>


Soldier::Soldier(int type, Vector2 position, bool flipped)
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
    this->flippedDefault = flipped;
    this->facingRight = !flipped;

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
    float rayHeight = 0;
    float rayYpos = 0;

    if (GetType() == 1) {
        rayLength = 50.f;
        rayHeight = GetHeight() / 2;
        rayYpos = position.y + GetHeight() / 2;
        if (!facingRight) {
            rayX = position.x - rayLength + rayLength;
        }
        else {
            rayX = position.x + rayLength * 2;
        }
    }
    else if (GetType() == 2) {
        rayLength = 300.f;
        rayHeight = 800.f;
        rayYpos = position.y - 200;
        if (!facingRight) {
            rayX = position.x - rayLength + 50;
            
        }
        else {
            rayX = position.x + 100;
        }
    }

    return Rectangle{ rayX, rayYpos, rayLength, rayHeight};
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
    //DrawRectangleLinesEx(GetHitBox(), 2, RED);
    //DrawRectangleLinesEx(GetHurtBox(), 2, WHITE);
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
    if (velocity.x < 0 && !leftCollision) position.x += velocity.x;
    else if (velocity.x > 0 && !rightCollision) position.x += velocity.x;
    position.y += velocity.y;
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
    float distToPlayer = fabsf(player.GetPosition().x - position.x);
    if (distToPlayer >= 800.0f) {
     
        if (currentState != SoldierState::IDLE && currentState != SoldierState::DEAD) {
            SetSoliderState(SoldierState::IDLE);
            soldierAnim.SetAnimation(SoldierState::IDLE);
            velocity.x = 0;
        }
        return;
    }
    else if ((stateTimer >= 3.0f && !IsVisionRay(player) && isAlive) ||
        (soldierAnim.IsAnimationFinished() && currentState != SoldierState::DEAD)) {
        stateTimer = 0.0f;
        hasShot = false;
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

        // Si choca con una pared, cambiar dirección o quedarse idle
        bool blocked = (direction == 1 && rightCollision) || (direction == -1 && leftCollision);

        if (blocked) {
            int randomBehaviour = GetRandomValue(0, 1);
            if (randomBehaviour == 0) {
                // Invertir dirección
                direction = -direction;
                velocity.x = 15 * direction;
                facingRight = (direction == 1);
            }
            else {
                // Quedarse idle
                SetSoliderState(SoldierState::IDLE);
                soldierAnim.SetAnimation(SoldierState::IDLE);
                velocity.x = 0;
                stateTimer = 0.0f;
            }
        }
        else {
            velocity.x = 15 * direction;
            facingRight = (direction == 1);
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
        hasShot = false;

        bool blocked = (direction == 1 && rightCollision) || (direction == -1 && leftCollision);

        if (blocked) {
            int randomBehaviour = GetRandomValue(0, 1);
            if (randomBehaviour == 0) {
                direction = -direction;
                velocity.x = 5 * direction;
                facingRight = (direction == 1);
            }
            else {
                SetSoliderState(SoldierState::IDLE);
                soldierAnim.SetAnimation(SoldierState::IDLE);
                velocity.x = 0;
                stateTimer = 0.0f;
            }
        }
        else {
            velocity.x = 5 * direction;
            facingRight = (direction == 1);
        }
    }
}

void Soldier::TriggerDeath(AudioManager& audio) {
    if (!isAlive) return;
    isAlive = false;
    currentState = SoldierState::DEAD;
    soldierAnim.ForceAnimation(SoldierState::DEAD);
    velocity.x = 0;
    audio.PlayRandomDeathSound();
}
Rectangle Soldier::GetLeftHitBox() {
    Rectangle hurtBox = GetHurtBox();
    float w = hurtBox.width * 0.4f;
    float h = hurtBox.height * 0.6f;
    float offsetY = (hurtBox.height - h) / 2.0f;
    return Rectangle{ hurtBox.x - w, hurtBox.y + offsetY, w, h };
}

Rectangle Soldier::GetRightHitBox() {
    Rectangle hurtBox = GetHurtBox();
    float w = hurtBox.width * 0.4f;
    float h = hurtBox.height * 0.6f;
    float offsetY = (hurtBox.height - h) / 2.0f;
    return Rectangle{ hurtBox.x + hurtBox.width, hurtBox.y + offsetY, w, h };
}