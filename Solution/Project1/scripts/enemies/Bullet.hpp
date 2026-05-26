#pragma once
#include <raylib.h>
#include "BulletAnim.hpp"

class Bullet {
public:
	Bullet(Vector2 position, int speed, int directionX, int directionY, int type);
	Bullet(const Bullet& other);
	void Update();
	void Draw();
	float GetWidth() {
		if (type == 2) {
			// Si es tipo 2 (Granada) y est� en estado GRENADESOLDIER
			if (bulletAnim.GetCurrentAnim() == BulletState::GRENADESOLDIER) {
				return 34.0f * scale;
			}
			// Si es la explosi�n u otro estado, puedes mantener el tama�o del clip (34)
			return 34.0f * scale;
		}
		if (type == 3) return 34.0f * scale;
		// Para el tipo 1 (Bala normal)
		return (float)bulletAnim.GetBulletPlayerImg().width * scale;
	}

	float GetHeight() {
		if (type == 2) {
			if (bulletAnim.GetCurrentAnim() == BulletState::GRENADESOLDIER) {
				return 34.0f * scale;
			}
			return 34.0f * scale;
		}
		if (type == 3) return 34.0f * scale;

		return (float)bulletAnim.GetBulletPlayerImg().height * scale;
	}
	void StartExplosion() {
		bulletAnim.SetAnimation(BulletState::EXPLOSIONSOLDIER);
		isExploding = true;
		this->speed = 0;      // Detenemos movimiento horizontal
		this->directionY = 0;
	}

	void SetExploding(bool val) { isExploding = val; }
	bool IsExploding() const { return isExploding; }
	float GetX() const { return position.x; }  // Getter para la posici�n X
	Vector2 GetPosition() const { return position; }
	void SetPosition(Vector2 newPosition) {
		position = newPosition;
	}// Getter para toda la posici�n
	Rectangle GetHitbox();
	bool active = true;
	void DrawHitBox();
	int GetType() { return type; }
	BulletAnim& GetAnim() { return bulletAnim; }
	bool IsMachinegunBullet() const { return type == 3; }
	float GetDirectionY() const { return directionY; }
	
	
private:
	BulletAnim bulletAnim;
	Vector2 position;
	float gravity = 9.8f;
	bool isExploding = false;
	int type;
	float speed;       
	float directionX;   
	float directionY;
	float scale = 3.0f;
};