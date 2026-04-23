#pragma once
#include "raylib.h"

class UiManager {
public:
    UiManager();
    ~UiManager();

    void Update();
    void DrawHUD(Camera2D camera);
    void DrawMissionIntro();
    bool IsMissionIntroOver() const;

    // Alias dla kompatybilnosci z game.cpp
    void DrawCredits(Camera2D camera) { DrawHUD(camera); }

    // Credits
    void SetCredits(int amount);
    int  GetCredits() const;

    // Score
    void AddScore(int amount);
    int  GetScore() const;

    // Time
    int  GetTimeLeft() const;
    bool IsTimeUp()    const;

    // Level
    void NextLevel();
    int  GetLevel() const;

    // Granaty
    int  GetBombs() const { return bombs; }
    bool HasBombs() const { return bombs > 0; }
    void UseGrenade();

    // Ammo (machinegun)
    void SetAmmo(int amount);
    int  GetAmmo() const { return ammo; }
    void UseAmmo();

    // Wyswietlana bron w HUD
    enum class WeaponDisplay { PISTOL, MACHINEGUN };
    void SetWeaponDisplay(WeaponDisplay w) { weaponDisplay = w; }

    // GO! idle detection
    void NotifyPlayerMoved();
    void UpdateGoTimer(float dt);

private:
    // Stan gry
    int   credits, score, level, timeLeft, bombs, ammo;
    WeaponDisplay weaponDisplay = WeaponDisplay::PISTOL;

    float timeAccum, introTimer, blinkAccum;
    bool  blinkVisible;

    static const float INTRO_DURATION;
    static const float BLINK_INTERVAL;

    // GO! idle system
    float idleTimer, goBlinkAccum;
    bool  goVisible, goBlinkOn;
    static constexpr float IDLE_THRESHOLD = 5.0f;
    static constexpr float GO_BLINK_RATE = 0.25f;

    // Textury
    Texture2D texArms, texBomb, texCannon, texTimeLevel;
    Texture2D texHudFont2Big, texHudFont2Num, texHudFont2Small;
    Texture2D texHighScore, texHighScoreSmall;
    Texture2D texHpBarLeft, texHpBarRight, texHpBarParts;
    Texture2D texGo;

    // Metody rysowania pomocnicze
    // hudfont2numbers (0-9, 12px kazdy)
    void  DrawHudDigit(char c, Vector2 pos, float scale = 2.0f, Color tint = WHITE) const;
    void  DrawHudNumber(int value, int digits, Vector2 pos, float scale = 2.0f, Color tint = WHITE) const;
    float MeasureHudNumber(int digits, float scale = 2.0f) const;

    // hudfont2big (A-Z, 8px kazdy)
    void  DrawBigLetter(char c, Vector2 pos, float scale = 2.0f) const;
    float MeasureBigText(const char* str, float scale = 2.0f) const;

    // highscorefont (A-Z=0..25, 0-9=26..35, -=36, 16px kazdy)
    void  DrawScoreChar(char c, Vector2 pos, float scale = 1.5f) const;
    void  DrawScoreText(const char* str, Vector2 pos, float scale = 1.5f) const;
    float MeasureScoreText(const char* str, float scale = 1.5f) const;

    // HP bar
    void DrawHpBar(Vector2 pos, int maxHP, int currentHP, float scale = 2.0f) const;

    // Intro
    void DrawMissionIntroInternal();
};