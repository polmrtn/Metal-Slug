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
    switch (type) {
    case 1:
    default:
        image = soldierAnim.GetSheet();

        break;
    }
    SetTextureFilter(image, TEXTURE_FILTER_POINT);
}

Soldier::~Soldier() {
    
}

Rectangle Soldier::GetHurtBox()
{
    return Rectangle{ position.x + GetWidth() / 3, position.y + GetHeight() / 2 , GetWidth() / 3, GetHeight()/ 2};
}
Rectangle Soldier::GetHitBox() {
    float rayLength = 50.f;
    float rayX ;
    if (!facingRight) {
        rayX = position.x - rayLength + 48 ;
    }
    else {
        rayX = position.x + rayLength * 2;
    }
    return Rectangle{ rayX , position.y + GetHeight() / 2, rayLength, GetHeight() / 2};
}
void Soldier::SetSoliderState(SoldierState newState)
{
    if (currentState == SoldierState::DEAD) return;
    if (currentState == newState) return;
    currentState = newState;
    
}
bool Soldier::IsVisionRay(Player& player)
{
    // Si el jugador no está vivo o está desaparecido, no detectarlo
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
     if (facingRight) src.width = -src.width;;
    float destW;
    float offsetX;
    if (soldierAnim.GetCurrentAnim() == SoldierState::ATTACKING) {
        destW = (34.f + 34.f) * scale;
        if (!facingRight) {
            offsetX = -138.f;
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
    // Solo atacar si el jugador está vivo
    if (!player.IsAlive()) return;

    if (IsVisionRay(player)) {
        if (!player.IsInvincible()) {  // ← Si implementaste invencibilidad
            TraceLog(LOG_INFO, "Soldier attacked and HIT the player!");
            player.TakeDamage();
        }
        else {
            TraceLog(LOG_INFO, "Soldier attacked but missed");
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
            SetSoliderState(SoldierState::ATTACKING);
            soldierAnim.ForceAnimation(SoldierState::ATTACKING);
            attackTriggered = false; 
        }
        attackTimer = 0.0f;
    }
    else if(!isAlive && currentState != SoldierState::DEAD)
    {

        SetSoliderState(SoldierState::DEAD);
        
    }
    

    if (currentState == SoldierState::IDLE && isGrounded && isAlive) {
       
        velocity.x = 0;
    }
    else if (currentState == SoldierState::WALKING && isGrounded && isAlive) {
        
        if (direction == 1) {
            velocity.x = 5;
            facingRight = true;
        }
        else
        {
            velocity.x = -5;
            facingRight = false;
        }
    }
    else if (currentState == SoldierState::ATTACKING && isAlive && isGrounded) {
        velocity.x = 0;

        //primero verificar si esta en el pico del ataque
        if (soldierAnim.IsAttackPeak() && !attackTriggered) {
            attackTriggered = true;
            Attack(player);
        }
        //después verificar si la animación terminó
        if (soldierAnim.IsAnimationFinished()) {
            SetSoliderState(SoldierState::IDLE);
            attackTriggered = false;
        }
        return;
    }
    else if(currentState == SoldierState::DEAD && !isAlive)
    {
        soldierAnim.SetAnimation(SoldierState::DEAD);
        velocity.x = 0;
        position.x = position.x;

    }
    else if (currentState == SoldierState::SNEAK && isAlive && isGrounded) {
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
}
void Soldier::TriggerDeath() {
    if (!isAlive) return;  // ya está muerto, no hacer nada
    isAlive = false;
    currentState = SoldierState::DEAD;
    soldierAnim.ForceAnimation(SoldierState::DEAD);
    velocity.x = 0;
}
