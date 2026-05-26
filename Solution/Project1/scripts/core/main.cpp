#include <raylib.h>
#include "game.hpp"

int main()
{
    InitWindow(1200, 896, "Metal Slug");
    InitAudioDevice();
    SetTargetFPS(30);

    audioManager.Init();
    player.Init();
    uiManager.Init();
    sceneManager.Init();
    backgroundManager.Init();
    boss.Init();

    Game* game = new Game();
    game->GetSceneManager().SetUiManager(&game->GetUiManager());

    while (WindowShouldClose() == false)
    {
        game->HandleInput();
        game->Update();

        if (game->ShouldRestart())
        {
            SceneManager::Gamestates stateAfterReset = sceneManager.GetGamestate();

            delete game;

            player.FullReset();
            uiManager.FullReset();
            sceneManager.FullReset();
            timerManager.FullReset();
            boss.FullReset();

            game = new Game();
            game->GetSceneManager().SetUiManager(&game->GetUiManager());

            if (stateAfterReset == SceneManager::TITLE)
                sceneManager.SetGameState(SceneManager::TITLE);
        }
        EndDrawing();
    }
    delete game;
    CloseWindow();
    return 0;
}
