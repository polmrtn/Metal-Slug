#pragma once
#include "raylib.h"

class Soldier {
public:
	Soldier(int type, Vector2 position);
	Soldier(const Soldier& other);   // constructor de copia
	~Soldier();
	Texture2D image;
	Vector2 position;
	int type;
	int GetType();
	void Draw();
	void Update();
	bool isGrounded;
	float groundLevel;
private:
	float gravity;
	Vector2 velocity;
	
	
};

