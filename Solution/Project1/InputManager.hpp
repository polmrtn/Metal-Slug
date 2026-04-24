#pragma once
#include <raylib.h>
#include "GlobalManagers.hpp"

class InputManager
{
public:
	InputManager();
	~InputManager();
	void InputUpdate();
	void InputCreditsPlayer();
	void InputChangeScene();
	void InputPlayer();
	void InputUi();

private:
	
};

