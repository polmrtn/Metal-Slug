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
    // Inicializar IA
    this->currentState = SoldierState::IDLE;
    this->stateTimer = 0;

    switch (type)
    {
    case 1:
        soldierAnim.LoadTexture();
        image = soldierAnim.GetSheet();
        break;
    default:
        soldierAnim.LoadTexture();
        image = soldierAnim.GetSheet();
        break;
    }
    SetTextureFilter(image, TEXTURE_FILTER_POINT);
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
    switch (type) {
    case 1:
    default:
        image = soldierAnim.GetSheet();

        break;
    }
    SetTextureFilter(image, TEXTURE_FILTER_POINT);
}

Soldier::~Soldier() {
    UnloadTexture(image);
}

Rectangle Soldier::GetHurtBox()
{
    return Rectangle{ position.x + GetWidth() / 3, position.y + GetHeight() / 2 , GetWidth() / 3, GetHeight()/ 2};
}
Rectangle Soldier::GetHitBox() {
    float rayLength = 100.f;
    float rayX ;
    if (!facingRight) {
        rayX = position.x - rayLength + rayLength/2;
    }
    else {
        rayX = position.x + rayLength;
    }
    return Rectangle{ rayX , position.y + GetHeight() / 2, rayLength, GetHeight() / 2};
}
void Soldier::SetSoliderState(SoldierState newState)
{
    if (currentState == newState) return;
    currentState = newState;
    // Start the animation immediately when state changes
    soldierAnim.SetAnimation(newState);
}
bool Soldier::IsVisionRay(Player& player)
{
    return CheckCollisionRecs(GetHitBox(), player.GetHitBox());
}
void Soldier::DrawHitBox()
{
    DrawRectangleLinesEx(GetHitBox(), 2, RED);
    DrawRectangleLinesEx(GetHurtBox(), 2, WHITE);
}
void Soldier::Draw() {
    Rectangle src = soldierAnim.GetSourceRect();
     if (facingRight) src.width = -src.width;;
    float destW;
    float offsetX;
    if (soldierAnim.GetCurrentAnim() == SoldierState::ATTACKING) {
        destW = (34.f + 38.f) * scale;
        if (!facingRight) {
            offsetX = -128.f;
        }
        else
        {
            offsetX = 0.f;
        }
        
    }
    else if (soldierAnim.GetCurrentAnim() == SoldierState::DEAD) {
        destW = 34.f * scale;
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
    else if (isGrounded) {
        velocity.y = 0;
    }

    // Aplicar Movimiento
    position.y += velocity.y;
    position.x += velocity.x;
    soldierAnim.Update();
}
void Soldier::Attack(Player& player) {
    if (IsVisionRay(player)) {
        TraceLog(LOG_INFO, "Soldier attacked ");
    }
    else {
        TraceLog(LOG_INFO, "Soldier attacked but missed");
    }
    
}
void Soldier::UpdateAI(Player& player)
{
    const bool inVision = IsVisionRay(player);

    // If player is in vision and we haven't triggered an attack for this entry, trigger once
    if (inVision && !attackTriggered) {
        SetSoliderState(SoldierState::ATTACKING);
        attackTriggered = true;
        attackTimer = 0.0f;
    }

    // If player left vision, allow re-trigger on next entry
    if (!inVision) {
        attackTriggered = false;
    }


    stateTimer += GetFrameTime();
    if (stateTimer >= 3.0f) {
        stateTimer = 0.0f;
        direction = (GetRandomValue(0, 1) == 0) ? -1 : 1;
        int randomBehaviour = GetRandomValue(0, 1);

        switch (randomBehaviour)
        {
        case 0:
            SetSoliderState(SoldierState::IDLE);
            break;

        case 1:
            SetSoliderState(SoldierState::WALKING);
            break;
        }
    }

    if (currentState == SoldierState::IDLE && isGrounded && isAlive) {
        soldierAnim.SetAnimation(SoldierState::IDLE);
        velocity.x = 0;
    }
    else if (currentState == SoldierState::WALKING && isGrounded && isAlive) {
        soldierAnim.SetAnimation(SoldierState::WALKING);
        if (direction == 1) {
            velocity.x = 2;
            facingRight = true;
        }
        else
        {
            velocity.x = -2;
            facingRight = false;
        }
    }
    else if  (currentState == SoldierState::ATTACKING && isAlive && isGrounded) {
        soldierAnim.SetAnimation(SoldierState::ATTACKING); // ensure correct anim
        velocity.x = 0;

        // Option A: call Attack() when animation finishes (you already set that flag in SoldierAnim)
        if (soldierAnim.IsAnimationFinished()) {
            Attack(player);
            // keep attackTriggered true so re-entry is required to attack again
        }
        return; // while attacking do not process other AI
    }
}