#include "SceneManager.hpp"

SceneManager::SceneManager()
{
    currentState = INTRO;
    introBackground = LoadTexture("Graphics/MetalSlug_title.png");
}
SceneManager::~SceneManager()
{
    UnloadTexture(introBackground);
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
        ClearBackground(BLACK);
    }
    else if (currentState == GAME) {
        DrawText("FUCK YOU", 420, 400, 20, GRAY);
        ClearBackground(BLACK);
    }
    else if (currentState == INTRO) {
        DrawTextureEx(introBackground, { 0, 0 }, 0.0f, 4.0f, WHITE);
        DrawText("Press ENTER to start", 520, 800, 20, GRAY);
        ClearBackground(BLACK);
    }
}