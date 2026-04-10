#pragma once
#include "raylib.h"

class UiManager {
public:
    UiManager();
    ~UiManager();

    void Update();
    void DrawCredits(Camera2D camera);
    void DrawMissionIntro();
    bool IsMissionIntroOver() const;

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

    float introTimer;
    float blinkAccum;       // akumulator do migania
    bool  blinkVisible;     // czy napis jest teraz widoczny

    static const float INTRO_DURATION;
    static const float BLINK_INTERVAL;
};