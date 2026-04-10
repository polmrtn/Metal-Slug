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

    // Inicializar IA
    this->currentState = SoldierState::IDLE;
    this->stateTimer = 0;

    switch (type)
    {
    case 1:
        image = LoadTexture("Graphics/Rebel Soldier_Sprites - Neutral 1.png");
        break;
    default:
        image = LoadTexture("Graphics/Rebel Soldier_Sprites - Neutral 1.png");
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

    switch (type) {
    case 1:
    default:
        image = LoadTexture("Graphics/Rebel Soldier_Sprites - Neutral 1.png");
        break;
    }
    SetTextureFilter(image, TEXTURE_FILTER_POINT);
}

Soldier::~Soldier() {
    UnloadTexture(image);
}

Rectangle Soldier::GetHitBox()
{
    float hitboxWidth = GetWidth() / 2;
    float hitboxHeight = GetHeight() / 2;

    float hitboxX = position.x + (GetWidth() - hitboxWidth) / 2;
    float hitboxY = position.y + (GetHeight() - hitboxHeight) / 2;

    return Rectangle{ hitboxX, hitboxY, hitboxWidth, hitboxHeight };
   
}

void Soldier::DrawHitBox()
{
    DrawRectangleLinesEx(GetHitBox(), 2, WHITE);
}

void Soldier::Draw() {
    Rectangle sourceRect = { 0, 0, (float)image.width, (float)image.height };

    Rectangle destRect = {
        position.x,
        position.y,
        GetWidth(),
        GetHeight()
    };

    Vector2 origin = { 0, 0 };

    DrawTexturePro(image, sourceRect, destRect, origin, 0.0f, WHITE);
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
            break;
        case 1:
            currentState = SoldierState::WALKING;
            break;
        case 2:
            currentState = SoldierState::ATTACKING;
            break;
        }
    }

    // Comportamiento según el estado
    if (currentState == SoldierState::IDLE) {
        velocity.x = 0;
    }
    else if (currentState == SoldierState::WALKING) {
        int direction = (GetRandomValue(0, 1) == 0) ? -2 : 2;
        velocity.x = direction;
    }
    else if (currentState == SoldierState::ATTACKING) {
        velocity.x = 0;
        // Aquí puedes añadir lógica de ataque
    }
}