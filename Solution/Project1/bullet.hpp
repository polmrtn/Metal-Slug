#pragma once
#include <raylib.h>

class Bullet {
	public:
		Bullet(Vector2 position, int speed);
		void Update();
		void Draw();
	private:
		Vector2 position; 
		int speed; 
};