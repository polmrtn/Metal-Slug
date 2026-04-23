#include <raylib.h>
#include "game.hpp"

int main()
{
    InitWindow(1200, 896, "Metal Slug");
    InitAudioDevice();
    SetTargetFPS(30);

    Game* game = new Game();

    while (WindowShouldClose() == false)
    {
        game->HandleInput();
        game->Update();

        // Si volvió al título después de ganar, reinicia todo
        if (game->ShouldRestart())
        {
            delete game;
            game = new Game();
        }

        EndDrawing();
    }

    delete game;
    CloseWindow();
    return 0;
}