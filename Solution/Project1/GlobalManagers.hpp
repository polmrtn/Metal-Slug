
#pragma once

#include "Player.hpp"
#include "UiManager.hpp"
#include "SceneManager.hpp"
#include "AudioManager.hpp"
#include "TimerManager.hpp"
#include "CameraManager.hpp"
#include "BackgroundManager.hpp"

// Contenedor de instancias globales. Usar como: Globals::player, Globals::timerManager, ...
namespace Globals
{
    extern Player player;
    extern UiManager uiManager;
    extern SceneManager sceneManager;
    extern AudioManager audioManager;
    extern TimerManager timerManager;
    extern CameraManager cameraManager;
    extern BackgroundManager backgroundManager;
}