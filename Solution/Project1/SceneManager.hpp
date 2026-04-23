#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include "UiManager.hpp"

class SceneManager {
public:
    enum Gamestates { INTRO, TITLE, GAME };

    SceneManager();
    ~SceneManager();

    void       DrawTexts();
    void       UpdateIntro();
    Gamestates GetGamestate();
    void       SetGameState(Gamestates gamestate);

    Gamestates currentState;
    UiManager* ui = nullptr;
    void SetUiManager(UiManager* u);

private:
    Texture2D texRedBg, texBlueBg;
    Texture2D texCannon, texCannonExplosion;
    Texture2D texBoom, texBullets;
    Texture2D texExplodingPixels, texExplo2sprites;
    Texture2D texTrees, texTankShit;
    Texture2D texCapsuleCannon, texCapsuleLoad;
    Texture2D texMetalBig, texSlugTM, texMetalSmall;
    Texture2D texLogoTop, texBrrrt;

    float introTimer = 0.0f;
    int   introPhase = 0;
    float tankX = 99999.0f;
    float bulletX = -999.0f;
    float metalX = -99999.0f;
    float slugX = 99999.0f;
    float boomAlpha = 0.0f;
    float bgAlpha = 0.0f;
    float shakeTime = 0.0f;
    float shakeStrength = 0.0f;
    float flashAlpha = 0.0f;
    float trackAnim = 0.0f;
    float capsuleAlpha = 0.0f;
    float treeScrollX = 0.0f;

    // Animacja capsuleload (faza 3)
    float capsulePulse = 0.0f;  // akumulator czasu dla pulsowania
    float capsuleScanY = 0.0f;  // pozycja scanline 0..1
    float capsuleVibration = 0.0f;  // losowe drzenie silnika

    bool  bulletVisible = false;
    bool  bulletsSpawned = false;
    bool  pixelsSpawned = false;

    struct Bullet2D { float x, y, vx, vy, alpha, rot, rotSpeed; };
    std::vector<Bullet2D> flyingBullets;

    struct ExPixel { float x, y, vx, vy, life; Color col; };
    std::vector<ExPixel> explodingPixels;

    void ResetIntro();
    void DrawIntro();
    void DrawTank(float tankX, float groundY, float tankScale,
        int trackFrame, float ox, float oy) const;
};