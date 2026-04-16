#pragma once
#include <raylib.h>
#include "BulletAnim.hpp"
enum class BulletState {
	GRENADESOLDIER,
	EXPLOSIONSOLDIER
};
struct AnimClipBullet {
	float rowY;      // Y en el spritesheet
	float cellW;     // ancho de celda (34 o 68 para die)
	float cellH;     // alto de celda
	int   frames;    // número de frames
	float fps;       // velocidad
	bool  loop;
	//looping anim
};

class BulletAnim
{
public:
	BulletAnim();
	~BulletAnim();
	void LoadTexture(int type);
	Texture2D GetSheet() const { return spriteSheet; }
	void UnloadTextures();
	void Update();
	void SetAnimation(BulletState animation);
	BulletState GetCurrentAnim() const { return currentAnim; }
	Texture2D GetBulletPlayerImg() const { return bulletPlayerImg; }
	

	// Devuelve true cuando la animación actual alcanzó su "fin" significativo.
	// Para ATTACKING significa: forward → peak → back → vuelta a 0.
	bool IsAnimationFinished();
	Rectangle GetSourceRect();

private:
	BulletState currentAnim;
	Texture2D bulletPlayerImg;
	Texture2D spriteSheet;
	int frame;
	float timer;
	bool animForward;
	bool animCompleted;

	AnimClipBullet CLIPS[2] = {
		//  rowY   cellW  cellH  frames  fps    loop  
			{ 0,   34,    34,    5,      10,   true},  // [0] GRENADE SOLDIER
			{ 34,   34,    34,    8,     12,   true },  // [1] EXPLOSION SOLDIER
	};

};

