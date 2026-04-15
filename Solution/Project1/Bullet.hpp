#pragma once
#include <raylib.h>

class Bullet {
public:
	Bullet(Vector2 position, int speed, int directionX, int directionY, int type);
	void Update();
	void Draw();
	float GetWidth() { return (float)image.width * scale; }
	float GetHeight() { return (float)image.height * scale; }
	float GetX() const { return position.x; }  // Getter para la posición X
	Vector2 GetPosition() const { return position; }  // Getter para toda la posición
	Rectangle GetHitbox();
	bool active = true;
	void DrawHitBox();
	int GetType() { return type; }

	
	
private:
	Texture2D image;
	Vector2 position;
	float gravity = 9.8f;
	int speed;
	int type;
	int directionX;
	int directionY;
	float scale = 3.0f;
};