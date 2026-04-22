#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>

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

private:
    // Textury
    Texture2D texRedBg, texBlueBg;
    Texture2D texCannon, texCannonExplosion;
    Texture2D texBoom, texBullets;
    Texture2D texExplodingPixels, texExplo2sprites;
    Texture2D texTrees, texTankShit;
    Texture2D texCapsuleCannon, texCapsuleLoad;
    Texture2D texMetalBig, texSlugTM, texMetalSmall;
    Texture2D texLogoTop, texBrrrt;

    // Stan animacji
    float introTimer = 0.0f;
    int   introPhase = 0;

    // tankX = lewa krawedz armaty (najdalsza lewa czesc calego czolgu)
    float tankX = 99999.0f;
    float bulletX = -999.0f;
    float metalX = -9999.0f;
    float slugX = 99999.0f;

    float boomAlpha = 0.0f;
    float bgAlpha = 0.0f;
    float shakeTime = 0.0f;
    float shakeStrength = 0.0f;
    float flashAlpha = 0.0f;
    float trackAnim = 0.0f;
    float capsuleAlpha = 0.0f;

    bool  bulletVisible = false;
    bool  bulletsSpawned = false;
    bool  pixelsSpawned = false;

    struct Bullet2D {
        float x, y, vx, vy, alpha, rot, rotSpeed;
    };
    std::vector<Bullet2D> flyingBullets;

    struct ExPixel {
        float x, y, vx, vy, life;
        Color col;
    };
    std::vector<ExPixel> explodingPixels;

    void ResetIntro();
    void DrawIntro();
    void DrawTank(float baseX, float groundY, float tankScale,
        int trackFrame, float ox, float oy) const;
};