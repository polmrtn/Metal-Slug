#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>

class SceneManager {
public:
    enum Gamestates {
        INTRO,
        TITLE,
        GAME
    };
    SceneManager();
    ~SceneManager();
    void DrawTexts();
    void UpdateIntro();
    Gamestates GetGamestate();
    void SetGameState(Gamestates gamestate);
    Gamestates currentState;

private:
    Texture2D texRedBg;
    Texture2D texBlueBg;
    Texture2D texCannon;
    Texture2D texCannonExplosion;
    Texture2D texBoom;
    Texture2D texBullets;
    Texture2D texExplodingPixels;
    Texture2D texExplo2sprites;
    Texture2D texTrees;
    Texture2D texTankShit;
    Texture2D texCapsuleCannon;
    Texture2D texCapsuleLoad;
    Texture2D texMetalBig;
    Texture2D texSlugTM;
    Texture2D texMetalSmall;
    Texture2D texLogoTop;

    float introTimer = 0.0f;
    int   introPhase = 0;

    float cannonX = -300.0f;
    float bulletX = 1400.0f;
    float logoY = 950.0f;
    float metalX = -600.0f;
    float slugX = 1400.0f;
    float boomAlpha = 0.0f;
    float boomScale = 0.1f;
    float bgAlpha = 0.0f;
    float shakeTime = 0.0f;
    float shakeStrength = 0.0f;
    float flashAlpha = 0.0f;
    float bulletT = 0.0f;

    struct Bullet2D {
        float x, y, vx, vy, alpha;
        float rot;
        float rotSpeed;
    };
    std::vector<Bullet2D> flyingBullets;
    bool bulletsSpawned = false;
};