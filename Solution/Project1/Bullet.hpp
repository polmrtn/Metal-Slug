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
			// Si es tipo 2 (Granada) y está en estado GRENADESOLDIER
			if (bulletAnim.GetCurrentAnim() == BulletState::GRENADESOLDIER) {
				return 34.0f * scale;
			}
			// Si es la explosión u otro estado, puedes mantener el tamaño del clip (34)
			return 34.0f * scale;
		}

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

		return (float)bulletAnim.GetBulletPlayerImg().height * scale;
	}
	void StartExplosion() {
		isExploding = true;
		this->speed = 0;      // Detenemos movimiento horizontal
		this->directionY = 0;
		bulletAnim.SetAnimation(BulletState::EXPLOSIONSOLDIER);
	}
	void SetExploding(bool val) { isExploding = val; }
	bool IsExploding() const { return isExploding; }
	float GetX() const { return position.x; }  // Getter para la posición X
	Vector2 GetPosition() const { return position; }
	void SetPosition(Vector2 newPosition) {
		position = newPosition;
	}// Getter para toda la posición
	Rectangle GetHitbox();
	bool active = true;
	void DrawHitBox();
	int GetType() { return type; }
	BulletAnim GetAnim() { return bulletAnim; }

	
	
private:
	BulletAnim bulletAnim;
	Vector2 position;
	float gravity = 9.8f;
	bool isExploding;
	int type;
	float speed;        // <--- DEBE SER FLOAT
	float directionX;   // <--- DEBE SER FLOAT
	float directionY;
	float scale = 3.0f;
};