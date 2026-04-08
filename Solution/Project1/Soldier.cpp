#include "Soldier.hpp"

Soldier::Soldier(int type, Vector2 position)//constructor depending on each type of soldier
{
	this->type = type;
	this->position = position;
	this->scale = 4.0f;
	this->isGrounded = false;
	this->velocity.y = 0;
	this->gravity = 0.8f;
	switch (type)//switch cases for each type of soldier / enemy (can be updated later to add more types of soldiers)
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
	return Rectangle{ position.x + GetHeight() / 2 , position.y + GetWidth() / 2, GetWidth() / 2 , GetHeight() / 2 };
}
void Soldier::DrawHitBox()
{
	DrawRectangleLinesEx(GetHitBox(), 2, WHITE);
}
void Soldier::Draw() {
	Rectangle sourceRect = { 0, 0, (float)image.width, (float)image.height };


	// Definimos el tamaño en pantalla (Ancho original * escala)
	Rectangle destRect = {
		position.x,
		position.y,
		GetWidth(),
		GetHeight()
	};

	Vector2 origin = { 0, 0 };

	// Dibujamos con escalado
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
	else if (velocity.y > 0) {
		velocity.y = 0;
	}

	// Aplicar Movimiento
	position.y += velocity.y;

	// Límites laterales del mapa
	float currentWidth = GetWidth();
	if (position.x < 0) position.x = 0;
	if (position.x + currentWidth > 7300) position.x = 7300 - currentWidth;
}
