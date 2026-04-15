#pragma once
#include <raylib.h>

class Bullet {
public:
	Bullet(Vector2 position, int speed, int directionX, int directionY);
	void Update();
	void Draw();
	float GetWidth() { return (float)image.width * scale; }
	float GetHeight() { return (float)image.height * scale; }
	float GetX() const { return position.x; }  // Getter para la posición X
	Vector2 GetPosition() const { return position; }  // Getter para toda la posición
	Rectangle GetHitbox();
	bool active = true;
	void DrawHitBox();
	
private:
	Texture2D image;
	Vector2 position;
	int speed;
	int directionX;
	int directionY;
	float scale = 3.0f;
};