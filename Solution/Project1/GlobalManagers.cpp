#include "GlobalManagers.hpp"

namespace Globals {
    Player player;
    UiManager uiManager;
    SceneManager sceneManager;
    AudioManager audioManager;
    TimerManager timerManager;
    CameraManager cameraManager;
    BackgroundManager backgroundManager;
    CreationManager creationManager;
}

Player& player = Globals::player;
UiManager& uiManager = Globals::uiManager;
SceneManager& sceneManager = Globals::sceneManager;
AudioManager& audioManager = Globals::audioManager;
TimerManager& timerManager = Globals::timerManager;
CameraManager& cameraManager = Globals::cameraManager;
BackgroundManager& backgroundManager = Globals::backgroundManager;
CreationManager& creationManager= Globals::creationManager;