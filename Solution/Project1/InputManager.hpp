#pragma once
#include <raylib.h>
#include "Player.hpp"
#include "UiManager.hpp"
#include "SceneManager.hpp"
#include "AudioManager.hpp"
#include "TimerManager.hpp"

class InputManager
{
public:
	InputManager();
	~InputManager();
	void InputUpdate();
	void InputCreditsPlayer();
	void InputChangeScene();
	void InputPlayer();

private:
	Player player;
	UiManager uiManager;
	SceneManager sceneManager;
	AudioManager audioManager;
	TimerManager timerManager;
};

