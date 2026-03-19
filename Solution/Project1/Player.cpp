#include "Player.hpp"

Player::Player()
{
	image = LoadTexture("Graphics/Marco_Sprites - Neutral 1.png");
	position.x = 100;
	position.y = 100;
}
Player::~Player() {
	UnloadTexture(image);
}
void Player::Draw() {
	DrawTextureV(image, position, WHITE);
}