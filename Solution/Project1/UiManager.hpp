#pragma once
#include "raylib.h"

class UiManager {
public:
    UiManager();
    ~UiManager();

    void Update();
    void DrawCredits(Camera2D camera);

    void SetCredits(int amount);
    int  GetCredits() const;

    void AddScore(int amount);
    int  GetScore() const;

    int  GetTimeLeft() const;
    bool IsTimeUp() const;

    void NextLevel();
    int  GetLevel() const;

private:
    int   credits;
    int   score;
    int   level;

    int   timeLeft;
    float timeAccum;
};