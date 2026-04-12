#pragma once
#include <raylib.h>
#include "SoldierState.hpp"

struct AnimClip {
	float rowY;      // Y en el spritesheet
	float cellW;     // ancho de celda (34 o 68 para die)
	float cellH;     // alto de celda
	int   frames;    // número de frames
	float fps;       // velocidad
	bool  loop;    //looping anim
};

class SoldierAnim
{
public:
	SoldierAnim();
	~SoldierAnim();
	void LoadTexture();
	Texture2D GetSheet() const { return spriteSheet; }
    
	bool IsShooting() const { return isShooting; }
	void Update();
	void SetAnimation(SoldierState animation) { currentAnim = animation; }
	SoldierState GetCurrentAnim() const { return currentAnim; }
	

	Rectangle GetSourceRect();

private:
	SoldierState currentAnim;
	Texture2D spriteSheet;
	bool isShooting;
	int frame;
	float timer;

	static constexpr AnimClip CLIPS[5] = {
		//  rowY   cellW  cellH  frames  fps    loop
			{ 0,   34,    68,    3,      6,    true  },  // [0] IDLE
			{ 68,   34,    68,    12,     12,   true  },  // [1] WALKING
			{ 152,  34,    68,    3,      8,    true  },  // [2] ATTACKING
			{ 220,  68,    68,    12,     10,   false },  // [3] DYING
			{ 288,  34,    68,    10,     8,    false },  // [4] DEAD
	};

};

