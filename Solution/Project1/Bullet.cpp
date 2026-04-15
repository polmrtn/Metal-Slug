#include "bullet.hpp"

Bullet::Bullet(Vector2 position, int speed, int directionX, int directionY) {
	image = LoadTexture("Graphics/bullet.png");
	this->position = position;
	this->speed = speed;
	this->directionX = directionX;
	this->directionY = directionY;
}

void Bullet::Update() {
	position.x += speed * directionX;
	position.y += speed * directionY;
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
