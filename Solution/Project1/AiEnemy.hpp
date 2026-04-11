#pragma once
#include <raylib.h>
enum class State {
	IDLE,
	WALKING,
	ATTACKING,
	HURT,
	DYING
};

class AiEnemy
{
public:
	AiEnemy();
	~AiEnemy();
	State GetState() { return CurrentState; }
	void SetState(State newState, State currentState) { currentState = newState; }
	
private:
	State CurrentState;
	bool IsAlive;

};

