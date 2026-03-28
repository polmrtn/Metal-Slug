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

void Bullet::Draw() {
	DrawCircle(position.x, position.y, 5, WHITE);
}