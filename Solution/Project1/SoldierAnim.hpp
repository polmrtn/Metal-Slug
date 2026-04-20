#pragma once
#include <raylib.h>
#include "SoldierState.hpp"

struct AnimClipSoldier {
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
	void UnloadTextures();
	bool IsShooting() const { return isShooting; }
	void Update();
	void SetAnimation(SoldierState animation);
	void ForceAnimation(SoldierState animation);
	bool IsAttackPeak() const { return attackPeakReached; }
	SoldierState GetCurrentAnim() const { return currentAnim; }
	bool CheckShootTrigger(){
		if (shootTriggered) {
			shootTriggered = false; // Se resetea al leerla (consumo)
			return true;
		}
		return false;
	}
	int GetFrame() const { return frame; }
	int GetCurrentClipFrames() const { return CLIPS[(int)currentAnim].frames; }


	void SetAttackAnimFps(float Fps) {
		CLIPS[3].fps = Fps;
	}
	
	// Devuelve true cuando la animación actual alcanzó su "fin" significativo.
	// Para ATTACKING significa: forward → peak → back → vuelta a 0.
	bool IsAnimationFinished() const;

	Rectangle GetSourceRect();

private:
	SoldierState currentAnim;
	Texture2D spriteSheet;
	bool isShooting;
	int frame;
	float timer;
	bool animForward;
	bool shootTriggered;
	bool attackPeakReached;
	// Flags para detectar final de animación (especialmente ATTACKING ping-pong)
	
	bool animCompleted;

	  AnimClipSoldier CLIPS[6] = {
		//  rowY   cellW  cellH  frames  fps    loop  
			{ 0,   34,    68,    3,      6,    true},  // [0] IDLE
			{ 68,   34,    68,    12,     12,   true },  // [2] WALKING
			{ 68 *2,  34,    68,    3,      8,    true  },  // [1] SNEAK
			{ 68 * 3,  68,    68,    12,     15,   false },  // [3] ATTACKING
			{ 68 * 4,  34,    68,    10,     8,    false },  // [4] DEAD
		    { 68 * 5,  44,     68,    16,   10,  false  } // [5] BOMB
	};

};

