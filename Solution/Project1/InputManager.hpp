#pragma once
#include <raylib.h>
#include "GlobalManagers.hpp"

class Game;

class InputManager
{
public:
	InputManager(Game* game);
	~InputManager();
	void InputUpdate();
	void InputCreditsPlayer();
	void InputChangeScene();
	void InputPlayer();
	void InputUi();
	void InputMachinegunBurst();
	void InputContinueScreen();
	void InputEnding();
private:
	Game* game;	
};

