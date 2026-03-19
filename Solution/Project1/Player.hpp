#pragma once
#include "raylib.h"

class Player {
public:
	Player();
	~Player();
	void Draw();
	void MoveLeft();
	void MoveRight();
	void Jump();
	void Shoot();
private:
	Texture2D image;
	Vector2 position;
};

