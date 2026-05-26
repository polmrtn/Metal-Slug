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
            // Zapamietaj stan przed resetem (np. TITLE po ekranie THE END)
            SceneManager::Gamestates stateAfterReset = sceneManager.GetGamestate();

            delete game;

            // Reset completo de todos los globals sin recargar texturas
            player.FullReset();
            uiManager.FullReset();
            sceneManager.FullReset();
            timerManager.FullReset();
            boss.FullReset();
            // creationManager se recarga en Game::Game() via LoadFromFile
            // backgroundManager, audioManager, boss no necesitan reset de estado

            game = new Game();
            game->GetSceneManager().SetUiManager(&game->GetUiManager());

            // Przywroc stan jezeli powrot do TITLE (np. po The End)
            if (stateAfterReset == SceneManager::TITLE)
                sceneManager.SetGameState(SceneManager::TITLE);
        }

        EndDrawing();
    }

    delete game;
    CloseWindow();
    return 0;
}
