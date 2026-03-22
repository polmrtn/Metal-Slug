#include <raylib.h>
#include "game.hpp"




int main() 
{
    InitWindow(1280, 896, "Metal Slug");
    InitAudioDevice();
    SetTargetFPS(30);
    
    
    Game game;
    SceneManager sceneManager;
    while (WindowShouldClose() == false)
    {
       game.HandleInput();
       game.Update();
       //game draw() ist inside game.cpp update()
        EndDrawing();
       
    }
    
    
   

    CloseWindow();
    return 0;
}
