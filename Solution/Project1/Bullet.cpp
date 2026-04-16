#include "bullet.hpp"
#include "BulletAnim.hpp"

Bullet::Bullet(Vector2 position, int speed, int directionX, int directionY, int type) {
	this->position = position;
	this->speed = speed;
	this->directionX = directionX;
	this->directionY = directionY;
	this->type = type;
	switch (type)
	{
	case 1:
		bulletAnim.LoadTexture(1);

		break;
	case 2:
		bulletAnim.LoadTexture(2);
		
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

	switch (type)
	{
	case 1:
		bulletAnim.GetBulletPlayerImg();

		break;
	case 2:
		bulletAnim.GetSheet();
		directionY += gravity * GetFrameTime();

		break;
	}

}

void Bullet::Update() {
	bulletAnim.Update();
	float dt = GetFrameTime();

	if (type == 1) {
		// Bala normal: línea recta perfecta
		position.x += (directionX * speed) * dt;
		position.y += (directionY * speed) * dt;
	}
	else if (type == 2) {
		if (isExploding) return; // ? al inicio, antes de cualquier movimiento

		directionY += 15.0f * dt;
		position.x += (directionX * speed) * dt;
		position.y += (directionY * 200.0f) * dt;
	}
}


Rectangle Bullet::GetHitbox()
{
	// Si es tipo 2, forzamos el tamaño a 20x20 (multiplicado por la escala)
	if (type == 2) {
		float hW = 20.0f * scale;
		float hH = 20.0f * scale;

		// Centramos el hitbox de 20x20 dentro del frame de 34x34
		// El offset es: (AnchoTotal - AnchoHitbox) / 2
		float offsetX = (GetWidth() - hW) / 2.0f;
		float offsetY = (GetHeight() - hH) / 2.0f;

		return Rectangle{ position.x + offsetX, position.y + offsetY, hW, hH };
	}

	// Si es tipo 1 (o cualquier otro), usa el tamaño normal de la imagen
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
	else {
		textureToDraw = bulletAnim.GetSheet();
		sourceRect = bulletAnim.GetSourceRect();
	}

	Rectangle destRect = {
		position.x,
		position.y,
		GetWidth(),  // Ahora dinámico
		GetHeight()  // Ahora dinámico
	};

	Vector2 origin = { 0, 0 };
	DrawTexturePro(textureToDraw, sourceRect, destRect, origin, 0.0f, WHITE);

	// Esto ahora dibujará el recuadro exactamente sobre la textura escalada
	DrawHitBox();
}
