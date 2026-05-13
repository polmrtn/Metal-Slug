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

    //debug boss
    player.SetX(15800.0f);
    player.SetY(100.0f);
    player.SetGrounded(true);

    while (WindowShouldClose() == false)
    {
        game->HandleInput();
        game->Update();

        if (game->ShouldRestart())
        {
            delete game;
            game = new Game();
            game->GetSceneManager().SetUiManager(&game->GetUiManager());
        }

        EndDrawing();
    }

    delete game;
    CloseWindow();
    return 0;
}
