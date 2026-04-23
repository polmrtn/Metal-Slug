#include <raylib.h>
#include "game.hpp"




int main()
{
    InitWindow(1200, 896, "Metal Slug");
    InitAudioDevice();
    SetTargetFPS(30);

    Game game;

    game.GetSceneManager().SetUiManager(&game.GetUiManager());

    while (!WindowShouldClose())
    {
        game.HandleInput();
        game.Update();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
