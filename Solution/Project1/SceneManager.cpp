#include "SceneManager.hpp"

SceneManager::SceneManager()
{
	currentState = TITLE;
}
SceneManager::~SceneManager()
{
}
SceneManager::Gamestates SceneManager::GetGamestate()
{
	return currentState;
}
void SceneManager::SetGameState(Gamestates gamestate)
{
    currentState = gamestate;
}
void SceneManager::DrawTexts() 
{
    if (currentState == TITLE) {
        DrawText("METAL SLUG", 450, 300, 50, WHITE);
        DrawText("Press ENTER to start", 420, 400, 20, GRAY);
    }
    else if (currentState == GAME) {
        DrawText("FUCK YOU", 420, 400, 20, GRAY);
    }
}