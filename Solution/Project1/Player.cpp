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
	direction = 1; 

}
Player::~Player() {   //class and destructor
	UnloadTexture(image);
}
void Player::Draw() {

	if (direction == -1){
		Rectangle sourceRect = { (float)image.width, 0, (float)-image.width, (float)image.height };
		DrawTextureRec(image, sourceRect, position, WHITE);
	}
	else {
		DrawTextureV(image, position, WHITE);
	}
}

void Player::Update()
{
	velocity.y += gravity;	//aplies gravity 
	position.y += velocity.y;	//updates position Y

	//Y position starts at top 0, the end is in the bottom, if it's bigger or equal than ground value, bool true, otherwise false. 
	if (position.y + image.height >= groundLevel)
	{
		position.y = groundLevel - image.height;
		velocity.y = 0;
		isGrounded = true;
	}
	else
	{
		isGrounded = false;
	}

	position.x += velocity.x; //updates position X 

	if (position.x < 0) //lateral limits, the right side will be changed following camera
	{
		position.x = 0;

	}
	if (position.x + image.width > GetScreenWidth())
	{
		position.x = GetScreenWidth() - image.width;

	}
	// Actualizar dirección basada en el movimiento
	if (velocity.x > 0) {
		direction = 1; // Mirando a la derecha
	}
	else if (velocity.x < 0) {
		direction = -1; // Mirando a la izquierda
	}
	// Si velocity.x == 0, mantenemos la dirección anterior
}

void Player::MoveLeft()
{
	velocity.x = -moveSpeed;
}

void Player::MoveRight()
{
	velocity.x = moveSpeed;
}

//void Player::Aim()
//{
//	if (direction = PlayerDirection::LEFT) {
//
//	}
//}

void Player::StopMoving()
{
	velocity.x = 0;
}

void Player::Jump()
{
	// Can only jump if its on the ground
	if (isGrounded)
	{
		velocity.y = jumpForce;
		isGrounded = false;
	}
}

void Player::Shoot()
{
	 
}
