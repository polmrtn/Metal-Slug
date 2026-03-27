#pragma once
#include "raylib.h"

enum class PlayerDirection {
	LEFT,
	RIGHT,
	UP,
	DOWN,
};


class Player {
private:
	Texture2D image;
	Vector2 position;
	Vector2 velocity;
	PlayerDirection direction;

	float gravity;
	float jumpForce;
	float moveSpeed;
	float crouchSpeed;
	float groundLevel;

	
	bool isGrounded;
	bool aimingUp;
	bool isCrouching; // estado de agachado

	float crouchHeight; //altura para cuando esta agachado
	float normalHeight; //altura normal
	
public:
	Player();
	~Player();
	void Draw();
		
	//movement
	void MoveLeft();
	void MoveRight();
	void StopMovingHorizontal();
		
		//aiming stats

	void StartAimingUp();
	void StopAimingUp();
	void StartCrouching();
	void StopCrouching();
		
	void Jump();
	void Update();
	void Shoot();

	Vector2 GetPosition() const { return position; }
	float GetWidth() const { return (float)image.width; }
	float GetHeight() const { return isCrouching ? crouchHeight : (float)image.height; }
	PlayerDirection GetDirection() const { return direction; } // Getter para la dirección
	PlayerDirection GetAimDirection() const;
	bool IsAimingUp() const { return aimingUp; }
	bool isCrounching() const { return isCrouching; }
	};

