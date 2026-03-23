#include "bullet.hpp"

Bullet::Bullet(Vector2 position, int speed)
{
	this->position = position;
	this->speed = speed; 
}

void Bullet::Draw() {
	DrawCircle(position.x, position.y, 5, WHITE);
}