#include <raylib.h>
#include "Player.hpp"

enum GameState {
    TITLE,
    GAME
};

int main() 
{
    InitWindow(1280, 896, "Metal Slug");
    SetTargetFPS(60);

    // AUDIO
    InitAudioDevice();

    Music titleMusic = LoadMusicStream("OST/03. Main Theme from Metal Slug (Stage 1).ogg");
    Music gameMusic = LoadMusicStream("OST/FX AUDIO/file002 mission 1 start.ogg");

    PlayMusicStream(titleMusic);

    GameState state = TITLE;
    Player player;

    while (!WindowShouldClose())
    {
        // UPDATE
        if (state == TITLE)
        {
            UpdateMusicStream(titleMusic);

            if (IsKeyPressed(KEY_ENTER))
            {
                StopMusicStream(titleMusic);
                PlayMusicStream(gameMusic);
                state = GAME;
            }
        }
        else if (state == GAME)
        {
            UpdateMusicStream(gameMusic);
            player.Update();
        }

        // DRAW
        BeginDrawing();
        ClearBackground(BLACK);

        if (state == TITLE)
        {
            DrawText("METAL SLUG", 450, 300, 50, WHITE);
            DrawText("Press ENTER to start", 420, 400, 20, GRAY);
        }
        else if (state == GAME)
        {
            player.Draw();
        }

        EndDrawing();
    }

    // CLEANUP
    UnloadMusicStream(titleMusic);
    UnloadMusicStream(gameMusic);
    CloseAudioDevice();

    CloseWindow();
    return 0;
}