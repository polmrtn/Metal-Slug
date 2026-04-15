#include "bullet.hpp"

Bullet::Bullet(Vector2 position, int speed, int directionX, int directionY, int type) {
	image = LoadTexture("Graphics/bullet.png");
	this->position = position;
	this->speed = speed;
	this->directionX = directionX;
	this->directionY = directionY;
	this->type = type;
	switch (type)
	{
	case 1:
		image = LoadTexture("Graphics/bullet.png");

		break;
	case 2:
		image = LoadTexture("Graphics/grenade_enemy.png");
		this->directionY += gravity * GetFrameTime();
		
		break;
	}
	
}
Bullet::Bullet(const Bullet& other) {
	image = LoadTexture("Graphics/bullet.png");
	position = other.position;
	speed = other.speed;
	directionX = other.directionX;
	directionY = other.directionY;
	type = other.type;
	switch (type)
	{
	case 1:
		image = LoadTexture("Graphics/bullet.png");

		break;
	case 2:
		image = LoadTexture("Graphics/grenade_enemy.png");
		directionY += other.gravity * GetFrameTime();

		break;
	}

}

void Bullet::Update() {
	if (GetType() == 1) {
		position.x += speed * directionX;
		position.y += speed * directionY;
	}
	else if (GetType() == 2) {
		
		this->position.x += this->directionX * this->speed * GetFrameTime();
		this->position.y += this->directionY * this->speed * GetFrameTime();

	}

}


Rectangle Bullet::GetHitbox()
{
	return Rectangle{ position.x , position.y , GetWidth() , GetHeight() };

}
void Bullet::DrawHitBox()
{
	DrawRectangleLinesEx(GetHitbox(), 2, WHITE);
}
void Bullet::Draw() {

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
