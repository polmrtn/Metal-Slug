#pragma once

#include "Player.hpp"
#include "UiManager.hpp"
#include "SceneManager.hpp"
#include "AudioManager.hpp"
#include "TimerManager.hpp"
#include "CameraManager.hpp"
#include "BackgroundManager.hpp"
#include "CreationManager.hpp"

// Contenedor de instancias globales. Usar como: Globals::player, Globals::timerManager, ...
namespace Globals
{
    extern class Player player;
    extern class UiManager uiManager;
    extern class SceneManager sceneManager;
    extern class AudioManager audioManager;
    extern class TimerManager timerManager;
    extern class CameraManager cameraManager;
    extern class BackgroundManager backgroundManager;
    extern class CreationManager creationManager;
}

// Aliases en el espacio global para no tener que escribir Globals::
// Declaraciones extern: las definiciones se realizan en GlobalManagers.cpp
extern Player& player;
extern UiManager& uiManager;
extern SceneManager& sceneManager;
extern AudioManager& audioManager;
extern TimerManager& timerManager;
extern CameraManager& cameraManager;
extern BackgroundManager& backgroundManager;
extern class CreationManager& creationManager;