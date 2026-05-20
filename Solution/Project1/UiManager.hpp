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
	void  SetBombs(int amount) { bombs += amount; }
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

    bool IsDelayActive() const { return continueDelayActive; }

    void DrawInsertCoin(float y, float scale) const;
    void DrawFooter(float y, float scale) const;

    bool IsExitPhase() const { return exitPhase; }

    void StartMissionComplete();
    bool IsMissionCompleteActive() const { return missionCompleteActive; }

    // Mission intro animation
    struct MissionLetter {
        char c;
        float targetX, targetY;  // posición final
        float currentX, currentY; // posición actual
        bool arrived;             // ha llegado a su posición
        bool isYellow;            // ya se flipó a amarillo
        float flipTimer;          // timer para el flip
        bool isStart;
        float flipProgress = 0.0f;  // 0.0 = blue normal, 0.5 = estrecho, 1.0 = yellow normal
    };

    void StartMissionIntro();
    void UpdateMissionIntro(float dt);

    void StartEnding();
    void UpdateEnding(float dt);
    void DrawEnding() const;
    bool IsEndingActive() const { return endingActive; }
    void  TriggerEndingFadeOut() { endingFadeOut = true; }
    bool  IsEndingFinished() const { return endingFinished; }

    void FullReset();
    void ResetScore() { score = 0; }


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
    static constexpr float IDLE_THRESHOLD = 2.0f;
    static constexpr float GO_BLINK_RATE = 0.25f;

    // Tekstury
    Texture2D texArms, texBomb, texCannon, texTimeLevel;
    Texture2D texHudFont2Big, texHudFont2Num, texHudFont2Small, texHudFont3Num; 
    Texture2D texHighScore, texHighScoreSmall;
    Texture2D texHpBarLeft, texHpBarRight, texHpBarParts;
    Texture2D texGo;
    Texture2D texTimeNum;
    Texture2D texMetalBigNum;
    Texture2D texGameOver;
    Texture2D texBlueLetters;
    Texture2D texYellowLetters; 
    Texture2D texHighScoreYellow;

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

    float continueDelay = 0.0f;
    static constexpr float CONTINUE_DELAY = 2.0f;  // ← ajusta este valor
    bool continueDelayActive = false;

    static constexpr float MISSION_CHAR_W = 32.0f;
    static constexpr float MISSION_CHAR_H = 32.0f;
    static constexpr float MISSION_SCALE = 3.0f;
    static constexpr float MISSION_TOTAL_DURATION = 5.0f;

    float missionTimer = 0.0f;
    bool  missionActive = false;
    float startBlinkTimer = 0.0f;
    int   startBlinkCount = 0;
    bool  startBlinkVisible = true;
    bool  exitPhase = false;
    float exitTimer = 0.0f;

    static constexpr int MAX_MISSION_LETTERS = 20; // "MISSION 1"
    MissionLetter missionLetters[MAX_MISSION_LETTERS];
    int   missionLetterCount = 0;
    int   missionLettersSpawned = 0;
    float letterSpawnTimer = 0.0f;
    bool missionCompleteActive = false;
    bool slowExit = false;  // false = salida rápida, true = salida lenta con fade
    float exitFadeAlpha = 0.0f;

    Texture2D texEnding;
    Texture2D texStars;
    bool endingActive = false;
    float endingTimer = 0.0f;

    struct EndingStar {
        float x, y;
        int   row;    // 0 = fila 1 (2 frames), 1 = fila 2 (10 frames)
        int   frame;
        float timer;
        float delay;
    };
    static constexpr int MAX_ENDING_STARS = 30;
    EndingStar endingStars[MAX_ENDING_STARS];
    float endingTextScale = 4.0f;
    bool  endingFadeOut = false;
    float endingFadeAlpha = 0.0f;
    bool  endingFinished = false;

    // GO animation
    Texture2D texGoAnim;
    int   goAnimFrame = 0;
    float goAnimTimer = 0.0f;
    float goAnimDelay = 0.035f;
    int   goAnimCycle = 0;      // 0 o 1 (dos animaciones consecutivas)
    float goAnimPauseTimer = 0.0f;
    bool  goAnimPausing = false;
    float goIdleTimer = 0.0f;   // timer de 5 segundos entre pares
    bool  goAnimActive = false;
    static constexpr int   GO_ANIM_FRAMES = 15;
    static constexpr float GO_ANIM_W = 32.0f;
    static constexpr float GO_ANIM_H = 31.0f;
    static constexpr float GO_PAUSE = 0.5f;
    static constexpr float GO_IDLE_INTERVAL = 5.0f;
    bool initialized = false;


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