#pragma once
#include "raylib.h"
#include <vector>

class SceneManager {
public:
    enum Gamestates
    {
        INTRO,
        TITLE,
        GAME
    };
    SceneManager();
    ~SceneManager();
    void DrawTexts();


    Gamestates GetGamestate();
    void SetGameState(Gamestates gamestate);
    Gamestates currentState;



private:

    Texture2D introBackground;


};