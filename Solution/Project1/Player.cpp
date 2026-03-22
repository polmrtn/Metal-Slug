#include "Player.hpp"

Player::Player() //class and constructor
{
    image = LoadTexture("Graphics/Marco_Sprites - Neutral 1.png");
    position = {100, 100};
    velocity = {0, 0};
}

Player::~Player()
{
    UnloadTexture(image);
}

void Player::Update()
{
    // INPUT
    if (IsKeyDown(KEY_A)) MoveLeft();
    if (IsKeyDown(KEY_D)) MoveRight();
    if (IsKeyPressed(KEY_SPACE)) Jump();

    // GRAVITY
    velocity.y += 0.5f;

    // APPLY MOVEMENT
    position.x += velocity.x;
    position.y += velocity.y;

    // GROUND
    if (position.y >= 700)
    {
        position.y = 700;
        velocity.y = 0;
    }

    // FRICTION
    velocity.x *= 0.8f;
}

void Player::Draw()
{
    DrawTextureV(image, position, WHITE);
}

void Player::MoveLeft()
{
    velocity.x = -5;
}

void Player::MoveRight()
{
    velocity.x = 5;
}

void Player::Jump()
{
    if (velocity.y == 0)
    {
        velocity.y = -10;
    }
}
