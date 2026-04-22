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
    // --- Textury ---
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
    Texture2D texBrrrt;

    // --- Stan animacji ---
    float introTimer = 0.0f;
    int   introPhase = 0;
    float cannonX = -900.0f;
    float bulletX = -999.0f;
    float logoY = 0.0f;
    float metalX = -900.0f;
    float slugX = 9999.0f;
    float boomAlpha = 0.0f;
    float boomScale = 1.0f;
    float bgAlpha = 0.0f;
    float shakeTime = 0.0f;
    float shakeStrength = 0.0f;
    float flashAlpha = 0.0f;
    float bulletT = 0.0f;
    bool  bulletVisible = false;
    float trackAnim = 0.0f;   // timer animacji gasienica czolgu

    // --- Lusksi (lecace po strzale) ---
    struct Bullet2D {
        float x, y, vx, vy, alpha;
        float rot;
        float rotSpeed;
    };
    std::vector<Bullet2D> flyingBullets;
    bool bulletsSpawned = false;

    // --- Exploding pixels (iskry) ---
    struct ExPixel {
        float x, y, vx, vy, life;
        Color col;
    };
    std::vector<ExPixel> explodingPixels;
    bool pixelsSpawned = false;

    // --- Metody pomocnicze ---
    void ResetIntro();
    void DrawIntro();
};