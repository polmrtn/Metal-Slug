#include "Player.hpp"

Player::Player() //class and constructor
{
	image = LoadTexture("Graphics/Marco_Sprites - Neutral 1.png");
	position.x = 100;
	position.y = 100;

	velocity.x = 0; 
	velocity.y = 0; 
	isGrounded = false;
	gravity = 0.8f;
	jumpForce = -12.0f; 
	moveSpeed = 5.0f; 
	groundLevel = 800.0f;

}
Player::~Player() {   //class and destructor
	UnloadTexture(image);
}
void Player::Draw() {
	DrawTextureV(image, position, WHITE);
}

void Player::Update()
{
	velocity.y += gravity;	//aplicar gravedad
	position.y += velocity.y;	//actualizar posicion Y	


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

	position.x += velocity.x; //actualizar posicion X con velocidad

	if (position.x < 0) //limites laterales
	{
		position.x = 0; 
		
	}
	if (position.x + image.width > GetScreenWidth())
	{
		position.x = GetScreenWidth() - image.width;
		
	}
}

void Player::MoveLeft()
{
	velocity.x = -moveSpeed;
}

void Player::MoveRight()
{
	velocity.x = moveSpeed;
}

void Player::StopMoving()
{
	velocity.x = 0; 
}

void Player::Jump()
{
	// Solo puede saltar si está en el suelo
	if (isGrounded)
	{
		velocity.y = jumpForce;
		isGrounded = false;
	}
}
