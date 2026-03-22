#pragma once
#include "raylib.h"

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
	private:
		Texture2D image;
		Vector2 position;
		Vector2 velocity;
		bool isGrounded;
		float gravity;
		float jumpForce;
		float moveSpeed;
		float groundLevel;
	};

