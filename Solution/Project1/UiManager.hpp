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
    // --- dane gry ---
    int   credits;
    int   score;
    int   level;
    int   timeLeft;
    float timeAccum;
    float introTimer;
    float blinkAccum;
    bool  blinkVisible;

    static const float INTRO_DURATION;
    static const float BLINK_INTERVAL;

    // --- tekstury sprite sheetów ---
    Texture2D texMetalNumbers;      // cyfry score/credits  (1234567890), 44x48 kazda
    Texture2D texTimeNumbers;       // cyfry timera         (0123456789:.), 16x16 kazda
    Texture2D texYellowLetters;     // litery i cyfry etykiet (A-Z 0-9 ! ?), 32x32 kazda

    // --- wymiary klatek ---
    static const int METAL_W = 44;
    static const int METAL_H = 48;
    static const int TIME_W = 16;
    static const int TIME_H = 16;
    static const int YELLOW_W = 32;
    static const int YELLOW_H = 32;

    // --- pomocnicze metody rysujace ---

    // Rysuje pojedyncza cyfre ze sprite sheeta metal_numbers
    // digit: 0-9, pos: lewy gorny rog na ekranie, scale: skalowanie
    void DrawMetalDigit(int digit, Vector2 pos, float scale = 1.0f) const;

    // Rysuje liczbe calkowita cyframi metal (np. score 7-cyfrowy)
    // digits: ile cyfr wymusic (0 = minimalna liczba)
    void DrawMetalNumber(int value, int digits, Vector2 pos, float scale = 1.0f) const;

    // Rysuje pojedyncza cyfre ze sprite sheeta time_numbers
    void DrawTimeDigit(char c, Vector2 pos, float scale = 1.0f) const;

    // Rysuje string cyframi time_numbers (obsluguje '0'-'9', ':', '.')
    void DrawTimeString(const char* str, Vector2 pos, float scale = 1.0f) const;

    // Rysuje pojedynczy znak ze sprite sheeta yellow_letters
    // Obsluguje 'A'-'Z', '0'-'9', '!', '?', spacja (pomijana)
    void DrawYellowChar(char c, Vector2 pos, float scale = 1.0f) const;

    // Rysuje caly string znakami yellow_letters
    void DrawYellowText(const char* str, Vector2 pos, float scale = 1.0f) const;

    // Zwraca szerokosc stringa w pikselach dla yellow_letters (po skalowaniu)
    float MeasureYellowText(const char* str, float scale = 1.0f) const;

    // Zwraca szerokosc liczby w pikselach dla metal_numbers (po skalowaniu)
    float MeasureMetalNumber(int digits, float scale = 1.0f) const;

    // Zwraca szerokosc stringa w pikselach dla time_numbers (po skalowaniu)
    float MeasureTimeString(const char* str, float scale = 1.0f) const;
};