#include "Soldier.hpp"

Soldier::Soldier(int type , Vector2 position)//constructor depending on each type of soldier
{
	this-> type = type;
	this->position = position;
	isGrounded = false;
	velocity.y = 0;
	gravity = 0.8f;
	groundLevel = 800.0f;
	switch (type)//switch cases for each type of soldier / enemy (can be updated later to add more types of soldiers)
	{
	case 1:
		image = LoadTexture("Graphics/Rebel Soldier_Sprites - Neutral 1.png");
		break;
	default:
		image = LoadTexture("Graphics/Rebel Soldier_Sprites - Neutral 1.png");	
		break;
	}
}
Soldier::Soldier(const Soldier& other)
{
	type = other.type;
	position = other.position;
	isGrounded = other.isGrounded;
	velocity = other.velocity;
	gravity = other.gravity;
	groundLevel = other.groundLevel;

	switch (type) {
	case 1:
	default:
		image = LoadTexture("Graphics/Rebel Soldier_Sprites - Neutral 1.png");
		break;
	}

}

Soldier::~Soldier() {
	UnloadTexture(image);
}
void Soldier::Draw() {
	DrawTextureV(image, position, WHITE);
}

int Soldier::GetType() {
	return type;
}

void Soldier::Update()
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
