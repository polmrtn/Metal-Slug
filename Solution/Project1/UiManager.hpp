#pragma once
#include "raylib.h"
#include <string>

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
    int  GetScore()    const;

    int  GetTimeLeft() const;
    bool IsTimeUp()    const;

    void NextLevel();
    int  GetLevel()    const;

private:
    int   credits;
    int   score;
    int   level;
    int   timeLeft;
    int bombs;
    float timeAccum;
    float introTimer;
    float blinkAccum;
    bool  blinkVisible;

    static const float INTRO_DURATION;
    static const float BLINK_INTERVAL;

    Texture2D texMetalNumbers;
    Texture2D texTimeNumbers;
    Texture2D texYellowLetters;

    Font  slugFont;
    bool  fontLoaded;

    static const int METAL_W = 44;
    static const int METAL_H = 48;
    static const int TIME_W = 16;
    static const int TIME_H = 16;
    static const int YELLOW_W = 32;
    static const int YELLOW_H = 32;

    void  DrawMetalDigit(int digit, Vector2 pos, float scale = 1.0f) const;
    void  DrawMetalNumber(int value, int digits, Vector2 pos, float scale = 1.0f) const;
    void  DrawTimeDigit(char c, Vector2 pos, float scale = 1.0f) const;
    void  DrawTimeString(const char* str, Vector2 pos, float scale = 1.0f) const;
    void  DrawYellowChar(char c, Vector2 pos, float scale = 1.0f, Color tint = WHITE) const;
    void  DrawYellowText(const char* str, Vector2 pos, float scale = 1.0f, float spacing = 0.78f, Color tint = WHITE) const;
    float MeasureYellowText(const char* str, float scale = 1.0f, float spacing = 0.78f) const;
    float MeasureMetalNumber(int digits, float scale = 1.0f) const;
    float MeasureTimeString(const char* str, float scale = 1.0f) const;

    // rysuje TTF bez zadnego tinta - zachowuje wbudowane kolory COLR fonta
    void  DrawSlugText(const char* str, Vector2 pos, float fontSize) const;
    float MeasureSlugText(const char* str, float fontSize) const;
    void DrawSlugTextShadow(const char* str, Vector2 pos, float fontSize) const;
};