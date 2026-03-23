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

}
Player::~Player() {   //class and destructor
	UnloadTexture(image);
}
void Player::Draw() {
	DrawTextureV(image, position, WHITE);
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
