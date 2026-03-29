#include "bullet.hpp"

Bullet::Bullet(Vector2 position, int speed, int directionX, int directionY) {
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
	return Rectangle{ position.x, position.y, 10, 10 };
	
	 

}
void Bullet::Draw() {
	Rectangle hitbox = GetHitbox();
	
	DrawRectangleLines(position.x - hitbox.width/2, position.y - hitbox.height/2, hitbox.width, hitbox.height, WHITE);
	DrawCircle(position.x, position.y, 5, RED);
}
