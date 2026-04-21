#include "Bullet.hpp"
#include "BulletAnim.hpp"

Bullet::Bullet(Vector2 position, int speed, int directionX, int directionY, int type) {
	this->position = position;
	this->speed = speed;
	this->directionX = directionX;
	this->directionY = directionY;
	this->type = type;
	this->isExploding = false;
	
	switch (type)
	{
	case 1:
		bulletAnim.LoadTexture(1);

		break;
	case 2:
		bulletAnim.LoadTexture(2);
		
		break;

	case 3:
		bulletAnim.LoadTexture(3);
		break;
	}

	
}
Bullet::Bullet(const Bullet& other) 
{
	
	position = other.position;
	speed = other.speed;
	directionX = other.directionX;
	directionY = other.directionY;
	gravity = other.gravity;
	type = other.type;
	isExploding = other.isExploding; // ← añadir
    bulletAnim = other.bulletAnim;

	

}

void Bullet::Update() {


	bulletAnim.Update();
	float dt = GetFrameTime();

	if (type == 1) {
		// Bala normal: l�nea recta perfecta
		position.x += (directionX * speed) * dt;
		position.y += (directionY * speed) * dt;
	}
	else if (type == 2) {
		if (isExploding) return; // ? al inicio, antes de cualquier movimiento

		directionY += 15.0f * dt;
		position.x += (directionX * speed) * dt;
		position.y += (directionY * 200.0f) * dt;
	}
	else if (type == 3) {
		position.x += (directionX * speed) * dt;
		position.y += (directionY * speed) * dt;
	}
}


Rectangle Bullet::GetHitbox()
{
	// Si es tipo 2, forzamos el tama�o a 20x20 (multiplicado por la escala)
	if (type == 2) {
		float hW = 20.0f * scale;
		float hH = 20.0f * scale;

		// Centramos el hitbox de 20x20 dentro del frame de 34x34
		// El offset es: (AnchoTotal - AnchoHitbox) / 2
		float offsetX = (GetWidth() - hW) / 2.0f;
		float offsetY = (GetHeight() - hH) / 2.0f;

		return Rectangle{ position.x + offsetX, position.y + offsetY, hW, hH };
	}

	// Si es tipo 1 (o cualquier otro), usa el tama�o normal de la imagen
	return Rectangle{ position.x, position.y, GetWidth(), GetHeight() };
}
void Bullet::DrawHitBox()
{
	DrawRectangleLinesEx(GetHitbox(), 2, WHITE);
}
void Bullet::Draw() {
	Rectangle sourceRect = { 0,0,0,0 };
	Texture2D textureToDraw;

	if (type == 1) {
		textureToDraw = bulletAnim.GetBulletPlayerImg();
		sourceRect = { 0, 0, (float)textureToDraw.width, (float)textureToDraw.height };
	}
	else if (type == 3) {
		textureToDraw = bulletAnim.GetBulletRifleImg();
		sourceRect = { 0, 0, 34.0f, 34.0f };
	}
	else {
		textureToDraw = bulletAnim.GetSheet();
		sourceRect = bulletAnim.GetSourceRect();
	}

	float rotation = 0.0f;
	Vector2 origin = { 0, 0 };

	if (type == 3 && directionY < 0) {
		rotation = -90.0f;
		origin = { GetWidth() / 2.0f, GetHeight() / 2.0f };
	}

	Rectangle destRect = {
		position.x + (rotation != 0.0f ? GetWidth() / 2.0f : 0.0f),
		position.y + (rotation != 0.0f ? GetHeight() / 2.0f : 0.0f),
		GetWidth(),
		GetHeight()
	};

	DrawTexturePro(textureToDraw, sourceRect, destRect, origin, rotation, WHITE);
	DrawHitBox();
}
