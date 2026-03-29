#include "Player.hpp"

Player::Player() //class and constructor
{
	image = LoadTexture("Graphics/Marco_Sprites - Neutral 1.png");

	//setting the following variables
	position.x = 100;
	position.y = 100;
	velocity.x = 0;
	velocity.y = 0;
	isGrounded = false; //player starting position is falling from sky, bool is false he's not on the ground
	gravity = 0.8f;
	jumpForce = -12.0f;
	moveSpeed = 5.0f;
	groundLevel = 800.0f;
	direction = PlayerDirection::RIGHT; //inicializamos mirando a la derecha  
	aimingUp = false;
	isCrouching = false;

	normalHeight = (float)image.height;
	crouchHeight = normalHeight * 0.6f;

}
Player::~Player() {   //class and destructor
	UnloadTexture(image);
}
void Player::Draw() {
	Rectangle sourceRect = { 0, 0, (float)image.width, (float)image.width };

	//flip horizontal para Left
	if (direction == PlayerDirection::LEFT) {
		sourceRect.width = -(float)image.width;
	}

	// Por implementar: Sprites diferentes para:
	// - aimingUp == true (mirando arriba)
	// - isCrouching == true (agachado)
	// - normal (izquierda/derecha)

	DrawTextureRec(image, sourceRect, position, WHITE);
}

void Player::Update()
{
	velocity.y += gravity;
	position.y += velocity.y;

	// Colisión con suelo (usando altura actual)
	float currentHeight = isCrouching ? crouchHeight : normalHeight;
	if (position.y + currentHeight >= groundLevel) {
		position.y = groundLevel - currentHeight;
		velocity.y = 0;
		isGrounded = true;
	}
	else {
		isGrounded = false;
	}

	// Movimiento horizontal
	position.x += velocity.x;

	// Límites laterales
	if (position.x < 0) {
		position.x = 0;
	}
	if (position.x + image.width > GetScreenWidth()) {
		position.x = GetScreenWidth() - image.width;
	}
}

Rectangle Player::GetHitBox()
{
	return Rectangle{ position.x, position.y, float(image.width),float(image.height) };
}
void Player::Shoot()
{
	
	
}
void Player::MoveLeft() {
	velocity.x = isCrouching ? -crouchSpeed : -moveSpeed;
	if (!aimingUp) {
		direction = PlayerDirection::LEFT;
	}
}

void Player::MoveRight() {
	velocity.x = isCrouching ? crouchSpeed : moveSpeed;
	if (!aimingUp) {
		direction = PlayerDirection::RIGHT;
	}
}


void Player::StopMovingHorizontal() {
	velocity.x = 0;
}
void Player::StartAimingUp() {
	aimingUp = true;
	if (isCrouching) {
		StopCrouching();
	}

}
void Player::StopAimingUp() {
	aimingUp = false;
}
void Player::StartCrouching() {
	if (isGrounded) {
		isCrouching = true;
		// Ajustar posición Y para mantener los pies en el suelo
		position.y += (normalHeight - crouchHeight);
		aimingUp = false;
	}
}
void Player::StopCrouching()
{
	if (isCrouching) {
		isCrouching = false;
		position.y -= (normalHeight - crouchHeight);
	}
}

void Player::Jump() {
	if (isGrounded && !isCrouching) {
		velocity.y = jumpForce;
		isGrounded = false;
	}
}

PlayerDirection Player::GetAimDirection() const {
	if (aimingUp) {
		return PlayerDirection::UP;
	}
	if (isCrouching) {
		return direction; // Agachado dispara hacia izquierda o derecha
	}
	return direction; // Normal: izquierda o derecha
}


void Player::DrawHitBox(bool isColliding) {
	Color outlineColor = isColliding ? WHITE : RED;
	DrawRectangleLinesEx(GetHitBox(), 3, outlineColor);
}
