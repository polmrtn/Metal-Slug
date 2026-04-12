#include "Soldier.hpp"
#include "Player.hpp"
#include <raylib.h>

Soldier::Soldier(int type, Vector2 position)
{
    
    this->type = type;
    this->position = position;
    this->scale = 4.0f;
    this->isGrounded = false;
    this->velocity.y = 0;
    this->velocity.x = 0;
    this->gravity = 0.8f;
    this->isAlive = true;

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

Rectangle Soldier::GetHitBox()
{
    return Rectangle{ position.x + GetWidth() / 3, position.y + GetHeight() / 2 , GetWidth() / 3, GetHeight()/ 2};
}

void Soldier::DrawHitBox()
{
    DrawRectangleLinesEx(GetHitBox(), 2, WHITE);
}
void Soldier::Draw() {
    Rectangle sourceRect = soldierAnim.GetSourceRect();
    if (velocity.x < 0) sourceRect.width = -sourceRect.width;

    float destW = (soldierAnim.GetCurrentAnim() == SoldierState::DYING) //solo si esta en dying se cambia el width
        ? 68.f * scale
        : 34.f * scale;
    Rectangle destRect = {
        position.x,
        position.y,
        destW,
        68.f * scale
    };

    Vector2 origin = { 0, 0 };

    DrawTexturePro(soldierAnim.GetSheet(), sourceRect, destRect, { 0, 0 }, 0.f, WHITE);
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

void Soldier::UpdateAI(Player& player)
{
    stateTimer += GetFrameTime();

    if (stateTimer >= 2.0f) {
        stateTimer = 0.0f;
        int randomBehaviour = GetRandomValue(0, 2);

        switch (randomBehaviour) {
        case 0:
            currentState = SoldierState::IDLE;
            soldierAnim.SetAnimation(SoldierState::IDLE);
            break;
        case 1:
            currentState = SoldierState::WALKING;
            soldierAnim.SetAnimation(SoldierState::IDLE);
            break;
        case 2:
            currentState = SoldierState::ATTACKING;
            soldierAnim.SetAnimation(SoldierState::IDLE);
            break;
        }
    }

    // Comportamiento según el estado
    if (currentState == SoldierState::IDLE && isGrounded && isAlive) {
        velocity.x = 0;
    }
    else if (currentState == SoldierState::WALKING && isGrounded && isAlive) {
        int direction = (GetRandomValue(0, 1) == 0) ? -2 : 2;
        velocity.x = direction;
    }
    else if (currentState == SoldierState::ATTACKING && isGrounded && isAlive) {
        velocity.x = 0;
        // Aquí puedes añadir lógica de ataque
    }
}