#include "GlobalManagers.hpp"

// Definición de las instancias globales
namespace Globals
{
    Player player;
    UiManager uiManager;
    SceneManager sceneManager;
    AudioManager audioManager;
    TimerManager timerManager;
    CameraManager cameraManager;
    BackgroundManager backgroundManager;
    CreationManager creationManager;
    Boss boss;
}

// Definición de los aliases (referencias) vinculados a las instancias en Globals
Player& player = Globals::player;
UiManager& uiManager = Globals::uiManager;
SceneManager& sceneManager = Globals::sceneManager;
AudioManager& audioManager = Globals::audioManager;
TimerManager& timerManager = Globals::timerManager;
CameraManager& cameraManager = Globals::cameraManager;
BackgroundManager& backgroundManager = Globals::backgroundManager;
CreationManager& creationManager = Globals::creationManager;
Boss& boss = Globals::boss;