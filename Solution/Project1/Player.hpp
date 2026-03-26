#pragma once
#include "raylib.h"

enum class PlayerDirection {
	LEFT,
	RIGHT,
	UP,
	DOWN,
};


class Player {
	public:
		Player();
		~Player();
		void Draw();
		void MoveLeft();
		void MoveRight();
		void StopMoving();
		void Jump();
		void Update();
		void Shoot();

		Vector2 GetPosition() const { return position; }
		float GetWidth() const { return (float)image.width; }
		float GetHeight() const { return (float)image.height; }
		int GetDirection() const { return direction; } // Getter para la dirección

	private:
		Texture2D image;
		Vector2 position;
		Vector2 velocity;
		bool isGrounded;
		float gravity;
		float jumpForce;
		float moveSpeed;
		float groundLevel;
		int direction;
	};

