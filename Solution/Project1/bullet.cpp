#include "bullet.hpp"

Bullet::Bullet(Vector2 position, int speed, int direction){
	this->position = position;
	this->speed = speed; 
	this->direction = direction; 
}

void Bullet::Update() {
	position.x += speed * direction; //para saber la direccion en x
}

void Bullet::Draw() {
	DrawCircle(position.x, position.y, 5, WHITE);
}