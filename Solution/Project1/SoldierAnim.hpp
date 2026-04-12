#pragma once
#include <raylib.h>
#include "SoldierState.hpp"

struct AnimClip {
	float rowY;      // Y en el spritesheet
	float cellW;     // ancho de celda (34 o 68 para die)
	float cellH;     // alto de celda
	int   frames;    // número de frames
	float fps;       // velocidad
	bool  loop;  
 //looping anim
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
	void SetAnimation(SoldierState animation);
	void ForceAnimation(SoldierState animation);

	SoldierState GetCurrentAnim() const { return currentAnim; }
	

	Rectangle GetSourceRect();

private:
	SoldierState currentAnim;
	Texture2D spriteSheet;
	bool isShooting;
	int frame;
	float timer;
	bool animForward;

	static constexpr AnimClip CLIPS[5] = {
		//  rowY   cellW  cellH  frames  fps    loop  
			{ 0,   34,    68,    3,      6,    true},  // [0] IDLE
			{ 68,   34,    68,    12,     12,   true },  // [2] WALKING
			{ 68 *2,  34,    68,    3,      8,    true  },  // [1] SNEAK
			{ 68 * 3,  68,    68,    12,     10,   false },  // [3] ATTACKING
			{ 68 * 4,  34,    68,    10,     8,    false },  // [4] DEAD
	};

};

