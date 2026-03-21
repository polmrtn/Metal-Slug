#pragma once
#include <raylib.h>
class Player
{
public:
	Player();
	~Player();
	void Draw();
	void MoveLeft();
	void MoveRight();
	void Shoot();
	void Jump();
private:
	Texture
};

Player::Player()
{
}

Player::~Player()
{
}
