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
    cannonX = -(float)texCannon.width * 2.2f;
    bulletX = 1400.0f;
    logoY = 950.0f;
    metalX = -800.0f;
    slugX = 1400.0f;
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
        cannonX = -(float)texCannon.width * 2.2f;
        bulletX = 1400.0f;
        logoY = 950.0f;
        metalX = -800.0f;
        slugX = 1400.0f;
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

    // Phase 0: fade in red background
    bgAlpha = Clamp(introTimer / 0.5f, 0.0f, 1.0f);

    // Phase 1: cannon slides in from left — only the LEFT half sprite
    if (introPhase >= 1)
    {
        float cannonSpriteW = (float)(texCannon.width / 2);  // left half only
        float cannonSpriteH = (float)texCannon.height;
        float cannonScale = 2.2f;
        float targetX = SW * 0.05f;
        float t = Clamp((introTimer - 0.6f) / 0.6f, 0.0f, 1.0f);
        float ease = 1.0f - (1.0f - t) * (1.0f - t);
        cannonX = -cannonSpriteW * cannonScale + ease * (targetX + cannonSpriteW * cannonScale);
    }

    // Phase 2: capsule flies right, boom flashes
    if (introPhase >= 2)
    {
        float t = Clamp((introTimer - 1.4f) / 0.4f, 0.0f, 1.0f);
        bulletX = SW * 0.35f + t * (SW * 0.8f);

        float boomIn = Clamp((introTimer - 1.55f) / 0.15f, 0.0f, 1.0f);
        float boomOut = Clamp((introTimer - 1.9f) / 0.35f, 0.0f, 1.0f);
        boomAlpha = boomIn * (1.0f - boomOut);
        boomScale = 0.25f + boomIn * 0.55f;
    }

    // Phase 3: spawn bullet casings once
    if (introPhase >= 3 && !bulletsSpawned)
    {
        bulletsSpawned = true;
        for (int i = 0; i < 6; i++)
        {
            Bullet2D b;
            b.x = SW * 0.35f;
            b.y = SH * 0.48f;
            b.vx = (float)(rand() % 300) + 50.0f;
            b.vy = (float)(rand() % 200) - 250.0f;
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

    // Phase 5: METAL slides from left, SLUG from right — ease-out cubic
    if (introPhase >= 5)
    {
        float t = Clamp((introTimer - 3.8f) / 0.7f, 0.0f, 1.0f);
        float ease = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);

        float targetMetalX = 0.0f;
        metalX = -800.0f + ease * (targetMetalX + 800.0f);

        float targetSlugX = SW * 0.25f;
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

    // INTRO
    UpdateIntro();

    // --- Background ---
    if (introPhase < 4)
    {
        float scale = (float)SW / (float)texRedBg.width;
        float scaleH = (float)SH / (float)texRedBg.height;
        float s = (scale > scaleH) ? scale : scaleH;
        DrawTextureEx(texRedBg, { 0.0f, 0.0f }, 0.0f, s,
            { 255, 255, 255, (unsigned char)(bgAlpha * 255.0f) });
    }
    else
    {
        float t = Clamp((introTimer - 2.8f) / 1.0f, 0.0f, 1.0f);
        float scaleR = (float)SW / (float)texRedBg.width;
        float scaleB = (float)SW / (float)texBlueBg.width;
        DrawTextureEx(texRedBg, { 0.0f, 0.0f }, 0.0f, scaleR, WHITE);
        DrawTextureEx(texBlueBg, { 0.0f, 0.0f }, 0.0f, scaleB,
            { 255, 255, 255, (unsigned char)(t * 255.0f) });
    }

    // --- Trees ---
    if (introPhase >= 1)
    {
        float scale = (float)SW / (float)texTrees.width;
        float y = (float)SH - (float)texTrees.height * scale;
        DrawTextureEx(texTrees, { 0.0f, y }, 0.0f, scale, WHITE);
    }

    // --- Cannon: draw only LEFT half of sprite sheet ---
    if (introPhase >= 1)
    {
        float cannonScale = 2.2f;
        int   halfW = texCannon.width / 2;
        float y = (float)SH * 0.52f - (float)texCannon.height * cannonScale * 0.5f;

        Rectangle src = { 0.0f, 0.0f, (float)halfW, (float)texCannon.height };
        Rectangle dst = { cannonX, y,
                          (float)halfW * cannonScale,
                          (float)texCannon.height * cannonScale };
        DrawTexturePro(texCannon, src, dst, { 0.0f, 0.0f }, 0.0f, WHITE);
    }

    // --- Capsule flies from cannon tip ---
    if (introPhase == 2)
    {
        float scale = 1.8f;
        float y = (float)SH * 0.47f - (float)texCapsuleCannon.height * scale * 0.5f;
        DrawTextureEx(texCapsuleCannon, { bulletX, y }, 0.0f, scale, WHITE);
    }

    // --- Boom flash at cannon tip ---
    if (introPhase >= 2 && boomAlpha > 0.01f)
    {
        int   halfW = texCannon.width / 2;
        float bx = cannonX + (float)halfW * 2.2f * 0.92f;
        float by = (float)SH * 0.47f - (float)texBoom.height * boomScale * 0.5f;
        DrawTextureEx(texBoom, { bx, by }, 0.0f, boomScale,
            { 255, 255, 255, (unsigned char)(boomAlpha * 200.0f) });
    }

    // --- Cannon explosion sprite ---
    if (introPhase >= 2 && boomAlpha > 0.01f)
    {
        float scale = 1.0f;
        int   halfW = texCannon.width / 2;
        float bx = cannonX + (float)halfW * 2.2f * 0.88f
            - (float)texCannonExplosion.width * scale * 0.5f;
        float by = (float)SH * 0.47f - (float)texCannonExplosion.height * scale * 0.5f;
        DrawTextureEx(texCannonExplosion, { bx, by }, 0.0f, scale,
            { 255, 255, 255, (unsigned char)(boomAlpha * 230.0f) });
    }

    // --- Exploding pixel debris ---
    if (introPhase >= 3)
    {
        float t = Clamp((introTimer - 2.0f) / 0.5f, 0.0f, 1.0f);
        float scale = 1.2f + t * 0.6f;
        unsigned char a = (unsigned char)((1.0f - t) * 200.0f);
        float x = (float)SW * 0.35f - (float)texExplodingPixels.width * scale * 0.5f;
        float y = (float)SH * 0.47f - (float)texExplodingPixels.height * scale * 0.5f;
        DrawTextureEx(texExplodingPixels, { x, y }, 0.0f, scale, { 255, 255, 255, a });
    }

    // --- Flying bullet casings ---
    for (const auto& b : flyingBullets)
    {
        int sprW = texBullets.width / 4;
        int sprH = texBullets.height / 2;
        Rectangle src = { 0.0f, 0.0f, (float)sprW, (float)sprH };
        Rectangle dst = { b.x, b.y, (float)sprW * 1.2f, (float)sprH * 1.2f };
        DrawTexturePro(texBullets, src, dst, { 0.0f, 0.0f }, 0.0f,
            { 255, 255, 255, (unsigned char)(b.alpha * 255.0f) });
    }

    // --- METAL SLUG logo ---
    if (introPhase >= 5)
    {
        float scaleM = 2.5f;
        float scaleS = 2.5f;

        float totalH = (float)texMetalBig.height * scaleM
            + (float)texSlugTM.height * scaleS + 4.0f;
        float startY = (float)SH * 0.5f - totalH * 0.5f;

        // METAL row — slides from left
        float my = startY;
        DrawTextureEx(texMetalBig, { metalX, my }, 0.0f, scaleM, WHITE);

        // SLUG row — slides from right
        float sy = my + (float)texMetalBig.height * scaleM + 4.0f;
        DrawTextureEx(texSlugTM, { slugX, sy }, 0.0f, scaleS, WHITE);

        // Small logo header centered above
        float logoHeaderScale = 1.3f;
        float hx = (float)SW * 0.5f - (float)texLogoTop.width * logoHeaderScale * 0.5f;
        float hy = startY - (float)texLogoTop.height * logoHeaderScale - 6.0f;
        DrawTextureEx(texLogoTop, { hx, hy }, 0.0f, logoHeaderScale, WHITE);
    }

    // --- Press ENTER blinking ---
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