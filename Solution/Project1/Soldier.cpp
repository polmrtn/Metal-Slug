#include "Soldier.hpp"

Soldier::Soldier(int type , Vector2 position)//constructor depending on each type of soldier
{
	this-> type = type;
	this->position = position;
	this->scale = 4.0f;
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
	SetTextureFilter(image, TEXTURE_FILTER_POINT);
}
Soldier::Soldier(const Soldier& other)
{
	type = other.type;
	position = other.position;
	isGrounded = other.isGrounded;
	velocity = other.velocity;
	gravity = other.gravity;
	groundLevel = other.groundLevel;
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
	return Rectangle{ position.x , position.y, GetWidth()/2 , GetHeight()/2};
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
		(float)image.width* scale,
		(float)image.height* scale
	};

	Vector2 origin = { 0, 0 };

	// Dibujamos con escalado
	DrawTexturePro(image, sourceRect, destRect, origin, 0.0f, WHITE);
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
	

	// Colisión con el Suelo (Considerando Escala)
	float currentHeight = GetHeight();
	if (position.y + currentHeight >= groundLevel) {
		position.y = groundLevel - currentHeight;
		velocity.y = 0;
		isGrounded = true;
	}
	else {
		isGrounded = false;
	}

	// Límites laterales del mapa
	float currentWidth = GetWidth();
	if (position.x < 0) position.x = 0;
	if (position.x + currentWidth > 7300) position.x = 7300 - currentWidth;
}
