#pragma once
#include "raylib.h"

class UiManager {
public:
    UiManager();
    ~UiManager();
    void Init();


    void Update();
    void DrawHUD(Camera2D camera);
    void DrawMissionIntro();
    bool IsMissionIntroOver() const;

    void DrawCredits(Camera2D camera) { DrawHUD(camera); }

    void SetCredits(int amount);
    int  GetCredits() const;

    void AddScore(int amount);
    int  GetScore() const;

    int  GetTimeLeft() const;
    bool IsTimeUp()    const;

    void NextLevel();
    int  GetLevel() const;

    int  GetBombs() const { return bombs; }
    bool HasBombs() const { return bombs > 0; }
    void UseGrenade();

    void SetAmmo(int amount);
    int  GetAmmo() const { return ammo; }
    void UseAmmo();

    enum class WeaponDisplay { PISTOL, MACHINEGUN };
    void SetWeaponDisplay(WeaponDisplay w) { weaponDisplay = w; }

    void NotifyPlayerMoved();
    void DrawCreditsOnly();

    void SetJetpackActive(bool val) { jetpackActive = val; }
    bool GetJetpackActive() const { return jetpackActive; }
    void SetJetpackFuel(float ratio) { jetpackFuelRatio = ratio; }

private:
    int credits;
    int score;
    int level;
    int timeLeft;
    int bombs;
    int ammo;

    WeaponDisplay weaponDisplay = WeaponDisplay::PISTOL;

    float timeAccum;
    float introTimer;
    float blinkAccum;
    bool  blinkVisible;

    static const float INTRO_DURATION;
    static const float BLINK_INTERVAL;

    // GO! system
    float idleTimer = 0.0f;
    float goBlinkAccum = 0.0f;
    bool  goVisible = false;
    bool  goBlinkOn = false;
    static constexpr float IDLE_THRESHOLD = 5.0f;
    static constexpr float GO_BLINK_RATE = 0.25f;

    // Tekstury
    Texture2D texArms, texBomb, texCannon, texTimeLevel;
    Texture2D texHudFont2Big, texHudFont2Num, texHudFont2Small;
    Texture2D texHighScore, texHighScoreSmall;
    Texture2D texHpBarLeft, texHpBarRight, texHpBarParts;
    Texture2D texGo;
    Texture2D texTimeNum;
    Texture2D texMetalBigNum;
    Texture2D texGameOver;

    // Metody pomocnicze
    void  DrawHudDigit(char c, Vector2 pos, float scale, Color tint) const;
    void  DrawHudNumber(int value, int digits, Vector2 pos, float scale, Color tint) const;
    float MeasureHudNumber(int digits, float scale) const;

    void  DrawBigLetter(char c, Vector2 pos, float scale) const;
    float MeasureBigText(const char* str, float scale) const;

    void  DrawScoreChar(char c, Vector2 pos, float scale) const;
    void  DrawScoreText(const char* str, Vector2 pos, float scale) const;
    float MeasureScoreText(const char* str, float scale) const;

    void  DrawTimeDigit(char c, Vector2 pos, float scale) const;
    void  DrawTimeNumber(int value, Vector2 pos, float scale) const;

    void  DrawInnerChar(char c, Vector2 pos, float scale) const;
    void  DrawInnerText(const char* str, Vector2 pos, float scale) const;
    float MeasureInnerText(const char* str, float scale) const;

    // POPRAWIONA DEKLARACJA (3 argumenty, zgodna z .cpp)
    void  DrawHpBar(Vector2 pos, int segs, float scale) const;

    void  UpdateGoTimer(float dt);
    void  DrawMissionIntroInternal();

    // Continue screen
    float continueElapsed    = 0.0f;
    float continueBlinkAccum = 0.0f;
    bool  continueBlinkOn    = true;
    float continueLabelX     = 10.0f;
    float continueLabelY     = 10.0f;
    bool  continueScreenActive = false;

    bool  jetpackActive = false;
    float jetpackFuelRatio = 0.0f;


public:
    void StartContinue();
    void StopContinue();
    void UpdateContinue(float dt);
    void DrawContinueScreen();
    bool IsContinueOver() const;

    void DrawGameOverOverlay(float t);   // czerwony filtr + fade to black
    void DrawGameOverSprite(float t);    // sprite game over (t=czas od wejscia w faze 3)

private:
};