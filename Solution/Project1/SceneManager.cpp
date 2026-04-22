#include "SceneManager.hpp"
#include "raymath.h"
#include <cstdlib>
#include <cmath>

SceneManager::SceneManager()
{
    currentState = INTRO;

    texRedBg = LoadTexture("Graphics/intro/newintroredbg.png");
    texBlueBg = LoadTexture("Graphics/intro/newintrobluebg.png");
    texCannon = LoadTexture("Graphics/intro/newintrocannon.png");
    texCannonExplosion = LoadTexture("Graphics/intro/NEWINTROCANNONEXPLOSION.png");
    texBoom = LoadTexture("Graphics/intro/newintroboom.png");
    texBullets = LoadTexture("Graphics/intro/newintrobulletsflippin.png");
    texExplodingPixels = LoadTexture("Graphics/intro/newintroexplodingpixels.png");
    texExplo2sprites = LoadTexture("Graphics/intro/newintro2explosprites.png");
    texTrees = LoadTexture("Graphics/intro/newintrotrees.png");
    texTankShit = LoadTexture("Graphics/intro/newintrotankshit.png");
    texCapsuleCannon = LoadTexture("Graphics/intro/newintrocapsulecannon.png");
    texCapsuleLoad = LoadTexture("Graphics/intro/newintrocapsuleload.png");
    texMetalBig = LoadTexture("Graphics/intro/NEWintroMETALSLUG1.png");
    texSlugTM = LoadTexture("Graphics/intro/NEWINTROmetalslugTM.png");
    texMetalSmall = LoadTexture("Graphics/intro/newintrometalslugggtiny.png");
    texLogoTop = LoadTexture("Graphics/intro/newintroagainmetalslug.png");

    introTimer = 0.0f;
    introPhase = 0;
    cannonX = -600.0f;
    bulletX = 2000.0f;
    logoY = 950.0f;
    metalX = -800.0f;
    slugX = 2000.0f;
    boomAlpha = 0.0f;
    boomScale = 0.1f;
    bgAlpha = 0.0f;
    bulletsSpawned = false;
}

SceneManager::~SceneManager()
{
    UnloadTexture(texRedBg);
    UnloadTexture(texBlueBg);
    UnloadTexture(texCannon);
    UnloadTexture(texCannonExplosion);
    UnloadTexture(texBoom);
    UnloadTexture(texBullets);
    UnloadTexture(texExplodingPixels);
    UnloadTexture(texExplo2sprites);
    UnloadTexture(texTrees);
    UnloadTexture(texTankShit);
    UnloadTexture(texCapsuleCannon);
    UnloadTexture(texCapsuleLoad);
    UnloadTexture(texMetalBig);
    UnloadTexture(texSlugTM);
    UnloadTexture(texMetalSmall);
    UnloadTexture(texLogoTop);
}

SceneManager::Gamestates SceneManager::GetGamestate()
{
    return currentState;
}

void SceneManager::SetGameState(Gamestates gamestate)
{
    currentState = gamestate;

    if (gamestate == INTRO)
    {
        introTimer = 0.0f;
        introPhase = 0;
        cannonX = -600.0f;
        bulletX = 2000.0f;
        logoY = 950.0f;
        metalX = -800.0f;
        slugX = 2000.0f;
        boomAlpha = 0.0f;
        boomScale = 0.1f;
        bgAlpha = 0.0f;
        bulletsSpawned = false;
        flyingBullets.clear();
    }
}

void SceneManager::UpdateIntro()
{
    float dt = GetFrameTime();
    introTimer += dt;

    if (introTimer < 0.6f)  introPhase = 0;
    else if (introTimer < 1.4f)  introPhase = 1;
    else if (introTimer < 2.0f)  introPhase = 2;
    else if (introTimer < 2.8f)  introPhase = 3;
    else if (introTimer < 3.8f)  introPhase = 4;
    else if (introTimer < 5.0f)  introPhase = 5;
    else                         introPhase = 6;

    float SW = (float)GetScreenWidth();
    float SH = (float)GetScreenHeight();

    bgAlpha = Clamp(introTimer / 0.5f, 0.0f, 1.0f);

    // Cannon slides in from left, stops at ~15% of screen
    if (introPhase >= 1)
    {
        float cannonScale = 1.4f;
        float cannonSpriteW = (float)(texCannon.width / 2) * cannonScale;
        float targetX = SW * 0.02f;
        float t = Clamp((introTimer - 0.6f) / 0.5f, 0.0f, 1.0f);
        float ease = 1.0f - (1.0f - t) * (1.0f - t);
        cannonX = -cannonSpriteW + ease * (targetX + cannonSpriteW);
    }

    // Capsule flies right from cannon tip
    if (introPhase >= 2)
    {
        float t = Clamp((introTimer - 1.4f) / 0.35f, 0.0f, 1.0f);
        bulletX = SW * 0.28f + t * SW * 1.2f;

        float boomIn = Clamp((introTimer - 1.45f) / 0.12f, 0.0f, 1.0f);
        float boomOut = Clamp((introTimer - 1.75f) / 0.3f, 0.0f, 1.0f);
        boomAlpha = boomIn * (1.0f - boomOut);
        boomScale = 0.3f + boomIn * 0.5f;
    }

    // Spawn bullet casings
    if (introPhase >= 3 && !bulletsSpawned)
    {
        bulletsSpawned = true;
        for (int i = 0; i < 6; i++)
        {
            Bullet2D b;
            b.x = SW * 0.28f;
            b.y = SH * 0.55f;
            b.vx = (float)(rand() % 200) + 80.0f;
            b.vy = (float)(rand() % 150) - 220.0f;
            b.alpha = 1.0f;
            flyingBullets.push_back(b);
        }
    }

    for (auto& b : flyingBullets)
    {
        b.x += b.vx * dt;
        b.y += b.vy * dt;
        b.vy += 500.0f * dt;
        b.alpha = Clamp(b.alpha - dt * 1.5f, 0.0f, 1.0f);
    }

    // METAL from left, SLUG from right
    if (introPhase >= 5)
    {
        float t = Clamp((introTimer - 3.8f) / 0.6f, 0.0f, 1.0f);
        float ease = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);

        // METAL target: left edge with small margin
        float targetMetalX = SW * 0.02f;
        metalX = -800.0f + ease * (targetMetalX + 800.0f);

        // SLUG target: center-right
        float scaleS = 2.5f;
        float slugW = (float)texSlugTM.width * scaleS;
        float targetSlugX = SW * 0.5f - slugW * 0.5f + SW * 0.1f;
        slugX = SW + ease * (targetSlugX - SW);
    }
}

void SceneManager::DrawTexts()
{
    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    if (currentState == TITLE)
    {
        ClearBackground(BLACK);
        DrawText("METAL SLUG", 450, 300, 50, WHITE);
        DrawText("Press ENTER to start", 420, 400, 20, GRAY);
        return;
    }
    if (currentState == GAME)
    {
        ClearBackground(BLACK);
        return;
    }

    UpdateIntro();

    // Background
    if (introPhase < 4)
    {
        float scaleW = (float)SW / (float)texRedBg.width;
        float scaleH = (float)SH / (float)texRedBg.height;
        float s = (scaleW > scaleH) ? scaleW : scaleH;
        DrawTextureEx(texRedBg, { 0.0f, 0.0f }, 0.0f, s,
            { 255, 255, 255, (unsigned char)(bgAlpha * 255.0f) });
    }
    else
    {
        float t = Clamp((introTimer - 2.8f) / 1.0f, 0.0f, 1.0f);
        float scaleRW = (float)SW / (float)texRedBg.width;
        float scaleRH = (float)SH / (float)texRedBg.height;
        float sR = (scaleRW > scaleRH) ? scaleRW : scaleRH;
        float scaleBW = (float)SW / (float)texBlueBg.width;
        float scaleBH = (float)SH / (float)texBlueBg.height;
        float sB = (scaleBW > scaleBH) ? scaleBW : scaleBH;
        DrawTextureEx(texRedBg, { 0.0f, 0.0f }, 0.0f, sR, WHITE);
        DrawTextureEx(texBlueBg, { 0.0f, 0.0f }, 0.0f, sB,
            { 255, 255, 255, (unsigned char)(t * 255.0f) });
    }

    // Trees
    if (introPhase >= 1)
    {
        float scaleW = (float)SW / (float)texTrees.width;
        float y = (float)SH - (float)texTrees.height * scaleW;
        DrawTextureEx(texTrees, { 0.0f, y }, 0.0f, scaleW, WHITE);
    }

    // Cannon — left half of spritesheet, scale 1.4, at ~55% height
    if (introPhase >= 1)
    {
        float cannonScale = 1.4f;
        int   halfW = texCannon.width / 2;
        float y = (float)SH * 0.55f - (float)texCannon.height * cannonScale * 0.5f;
        Rectangle src = { 0.0f, 0.0f, (float)halfW, (float)texCannon.height };
        Rectangle dst = { cannonX, y,
                               (float)halfW * cannonScale,
                               (float)texCannon.height * cannonScale };
        DrawTexturePro(texCannon, src, dst, { 0.0f, 0.0f }, 0.0f, WHITE);
    }

    // Capsule flies from cannon tip (use texCapsuleCannon, left half only)
    if (introPhase == 2)
    {
        float scale = 1.5f;
        int   halfW = texCapsuleCannon.width / 2;
        float y = (float)SH * 0.55f - (float)texCapsuleCannon.height * scale * 0.5f;
        Rectangle src = { 0.0f, 0.0f, (float)halfW, (float)texCapsuleCannon.height };
        Rectangle dst = { bulletX, y,
                          (float)halfW * scale,
                          (float)texCapsuleCannon.height * scale };
        DrawTexturePro(texCapsuleCannon, src, dst, { 0.0f, 0.0f }, 0.0f, WHITE);
    }

    // Boom flash at cannon tip
    if (introPhase >= 2 && boomAlpha > 0.01f)
    {
        float cannonScale = 1.4f;
        int   halfW = texCannon.width / 2;
        float tipX = cannonX + (float)halfW * cannonScale * 0.95f;
        float tipY = (float)SH * 0.55f - (float)texBoom.height * boomScale * 0.5f;
        DrawTextureEx(texBoom, { tipX, tipY }, 0.0f, boomScale,
            { 255, 255, 255, (unsigned char)(boomAlpha * 200.0f) });
    }

    // Cannon explosion sprite at tip
    if (introPhase >= 2 && boomAlpha > 0.01f)
    {
        float cannonScale = 1.4f;
        int   halfW = texCannon.width / 2;
        float scale = 0.8f;
        float tipX = cannonX + (float)halfW * cannonScale * 0.9f
            - (float)texCannonExplosion.width * scale * 0.5f;
        float tipY = (float)SH * 0.55f
            - (float)texCannonExplosion.height * scale * 0.5f;
        DrawTextureEx(texCannonExplosion, { tipX, tipY }, 0.0f, scale,
            { 255, 255, 255, (unsigned char)(boomAlpha * 230.0f) });
    }

    // Exploding pixel debris
    if (introPhase >= 3)
    {
        float t = Clamp((introTimer - 2.0f) / 0.5f, 0.0f, 1.0f);
        float scale = 1.0f + t * 0.5f;
        unsigned char a = (unsigned char)((1.0f - t) * 180.0f);
        float x = (float)SW * 0.28f - (float)texExplodingPixels.width * scale * 0.5f;
        float y = (float)SH * 0.55f - (float)texExplodingPixels.height * scale * 0.5f;
        DrawTextureEx(texExplodingPixels, { x, y }, 0.0f, scale, { 255, 255, 255, a });
    }

    // Flying bullet casings
    for (const auto& b : flyingBullets)
    {
        int sprW = texBullets.width / 4;
        int sprH = texBullets.height / 2;
        Rectangle src = { 0.0f, 0.0f, (float)sprW, (float)sprH };
        Rectangle dst = { b.x, b.y, (float)sprW * 1.2f, (float)sprH * 1.2f };
        DrawTexturePro(texBullets, src, dst, { 0.0f, 0.0f }, 0.0f,
            { 255, 255, 255, (unsigned char)(b.alpha * 255.0f) });
    }

    // METAL SLUG logo
    if (introPhase >= 5)
    {
        float scaleM = 2.5f;
        float scaleS = 2.5f;

        // Logo block vertically centered
        float totalH = (float)texMetalBig.height * scaleM
            + (float)texSlugTM.height * scaleS + 8.0f;
        float startY = (float)SH * 0.48f - totalH * 0.5f;

        // METAL row
        float my = startY;
        DrawTextureEx(texMetalBig, { metalX, my }, 0.0f, scaleM, WHITE);

        // SLUG row directly below METAL
        float sy = my + (float)texMetalBig.height * scaleM + 8.0f;
        DrawTextureEx(texSlugTM, { slugX, sy }, 0.0f, scaleS, WHITE);

        // Small header logo centered at top of logo block
        float logoHeaderScale = 1.2f;
        float hx = (float)SW * 0.5f
            - (float)texLogoTop.width * logoHeaderScale * 0.5f;
        float hy = startY - (float)texLogoTop.height * logoHeaderScale - 8.0f;
        DrawTextureEx(texLogoTop, { hx, hy }, 0.0f, logoHeaderScale, WHITE);
    }

    // Press ENTER blinking
    if (introPhase >= 6)
    {
        if ((int)(introTimer * 2.0f) % 2 == 0)
        {
            DrawText("Press ENTER to start",
                SW / 2 - MeasureText("Press ENTER to start", 20) / 2,
                (int)((float)SH * 0.88f), 20, YELLOW);
        }
    }
}