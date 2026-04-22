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

    if (gamestate == INTRO) {
        introTimer = 0.0f;
        introPhase = 0;
        cannonX = -300.0f;
        bulletX = 1400.0f;
        logoY = 950.0f;
        metalX = -600.0f;
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

    // Phase 0: fade in background
    bgAlpha = Clamp(introTimer / 0.5f, 0.0f, 1.0f);

    // Phase 1: cannon slides in from left
    if (introPhase >= 1) {
        float t = Clamp((introTimer - 0.6f) / 0.6f, 0.0f, 1.0f);
        cannonX = -300.0f + t * (SW * 0.1f + 300.0f);
    }

    // Phase 2: capsule flies right, boom flashes then fades
    if (introPhase >= 2) {
        float t = Clamp((introTimer - 1.4f) / 0.4f, 0.0f, 1.0f);
        // capsule starts at cannon tip and flies off right
        bulletX = SW * 0.35f + t * (SW * 0.8f);

        float boomIn = Clamp((introTimer - 1.55f) / 0.15f, 0.0f, 1.0f);
        float boomOut = Clamp((introTimer - 1.9f) / 0.35f, 0.0f, 1.0f);
        boomAlpha = boomIn * (1.0f - boomOut);
        boomScale = 0.25f + boomIn * 0.55f;  // max scale 0.8 - small flash
    }

    // Phase 3: spawn bullet casings once
    if (introPhase >= 3 && !bulletsSpawned) {
        bulletsSpawned = true;
        for (int i = 0; i < 6; i++) {
            Bullet2D b;
            b.x = SW * 0.35f;
            b.y = SH * 0.48f;
            b.vx = (float)(rand() % 300) + 50.0f;   // all fly right
            b.vy = (float)(rand() % 200) - 250.0f;
            b.alpha = 1.0f;
            flyingBullets.push_back(b);
        }
    }

    for (auto& b : flyingBullets) {
        b.x += b.vx * dt;
        b.y += b.vy * dt;
        b.vy += 500.0f * dt;
        b.alpha = Clamp(b.alpha - dt * 1.5f, 0.0f, 1.0f);
    }

    // Phase 5: logo slides in with ease-out cubic
    if (introPhase >= 5) {
        float t = Clamp((introTimer - 3.8f) / 0.7f, 0.0f, 1.0f);
        float ease = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);

        // METAL target: left-aligned, starts off left edge
        float targetMetalX = (float)GetScreenWidth() * 0.05f;
        metalX = -700.0f + ease * (targetMetalX + 700.0f);

        // SLUG target: right side, starts off right edge
        float targetSlugX = (float)GetScreenWidth() * 0.38f;
        slugX = (float)GetScreenWidth() + ease * (targetSlugX - (float)GetScreenWidth());
    }
}

void SceneManager::DrawTexts()
{
    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    if (currentState == TITLE) {
        ClearBackground(BLACK);
        DrawText("METAL SLUG", 450, 300, 50, WHITE);
        DrawText("Press ENTER to start", 420, 400, 20, GRAY);
        return;
    }
    if (currentState == GAME) {
        ClearBackground(BLACK);
        DrawText("FUCK YOU", 420, 400, 20, GRAY);
        return;
    }

    UpdateIntro();

    // Background: red fading to blue
    if (introPhase < 4) {
        float scale = (float)SW / (float)texRedBg.width;
        DrawTextureEx(texRedBg, { 0.0f, 0.0f }, 0.0f, scale,
            { 255, 255, 255, (unsigned char)(bgAlpha * 255.0f) });
    }
    else {
        float t = Clamp((introTimer - 2.8f) / 1.0f, 0.0f, 1.0f);
        float scaleR = (float)SW / (float)texRedBg.width;
        float scaleB = (float)SW / (float)texBlueBg.width;
        DrawTextureEx(texRedBg, { 0.0f, 0.0f }, 0.0f, scaleR, WHITE);
        DrawTextureEx(texBlueBg, { 0.0f, 0.0f }, 0.0f, scaleB,
            { 255, 255, 255, (unsigned char)(t * 255.0f) });
    }

    // Trees always visible from phase 1
    if (introPhase >= 1) {
        float scale = (float)SW / (float)texTrees.width;
        float y = (float)SH - (float)texTrees.height * scale;
        DrawTextureEx(texTrees, { 0.0f, y }, 0.0f, scale, WHITE);
    }

    // Single cannon sprite (texCannon), positioned bottom-left area
    if (introPhase >= 1) {
        float scale = 2.2f;
        float y = (float)SH * 0.52f - (float)texCannon.height * scale * 0.5f;
        DrawTextureEx(texCannon, { cannonX, y }, 0.0f, scale, WHITE);
    }

    // Capsule / bullet flies from cannon tip to right (phase 2 only)
    if (introPhase == 2) {
        float scale = 1.8f;
        float y = (float)SH * 0.47f - (float)texCapsuleCannon.height * scale * 0.5f;
        DrawTextureEx(texCapsuleCannon, { bulletX, y }, 0.0f, scale, WHITE);
    }

    // Small boom flash at cannon tip
    if (introPhase >= 2 && boomAlpha > 0.01f) {
        float bx = cannonX + (float)texCannon.width * 2.2f * 0.85f;
        float by = (float)SH * 0.47f - (float)texBoom.height * boomScale * 0.5f;
        DrawTextureEx(texBoom, { bx, by }, 0.0f, boomScale,
            { 255, 255, 255, (unsigned char)(boomAlpha * 200.0f) });
    }

    // Cannon explosion sprite at same spot, fades with boom
    if (introPhase >= 2 && boomAlpha > 0.01f) {
        float scale = 1.0f;
        float bx = cannonX + (float)texCannon.width * 2.2f * 0.82f
            - (float)texCannonExplosion.width * scale * 0.5f;
        float by = (float)SH * 0.47f - (float)texCannonExplosion.height * scale * 0.5f;
        DrawTextureEx(texCannonExplosion, { bx, by }, 0.0f, scale,
            { 255, 255, 255, (unsigned char)(boomAlpha * 230.0f) });
    }

    // Exploding pixel debris (phase 3, fades fast)
    if (introPhase >= 3) {
        float t = Clamp((introTimer - 2.0f) / 0.5f, 0.0f, 1.0f);
        float scale = 1.2f + t * 0.6f;
        unsigned char a = (unsigned char)((1.0f - t) * 200.0f);
        float x = (float)SW * 0.35f - (float)texExplodingPixels.width * scale * 0.5f;
        float y = (float)SH * 0.47f - (float)texExplodingPixels.height * scale * 0.5f;
        DrawTextureEx(texExplodingPixels, { x, y }, 0.0f, scale, { 255, 255, 255, a });
    }

    // Flying bullet casings
    for (const auto& b : flyingBullets) {
        int sprW = texBullets.width / 4;
        int sprH = texBullets.height / 2;
        Rectangle src = { 0.0f, 0.0f, (float)sprW, (float)sprH };
        Rectangle dst = { b.x, b.y, (float)sprW * 1.2f, (float)sprH * 1.2f };
        DrawTexturePro(texBullets, src, dst, { 0.0f, 0.0f }, 0.0f,
            { 255, 255, 255, (unsigned char)(b.alpha * 255.0f) });
    }

    // METAL SLUG logo — METAL from left, SLUG from right, stacked
    if (introPhase >= 5) {
        float scaleM = 2.5f;
        float scaleS = 2.5f;

        // Vertical center of the logo block
        float totalH = (float)texMetalBig.height * scaleM
            + (float)texSlugTM.height * scaleS + 4.0f;
        float startY = (float)SH * 0.5f - totalH * 0.5f;

        // METAL row
        float my = startY;
        DrawTextureEx(texMetalBig, { metalX, my }, 0.0f, scaleM, WHITE);

        // SLUG row, directly below METAL
        float sy = my + (float)texMetalBig.height * scaleM + 4.0f;
        DrawTextureEx(texSlugTM, { slugX, sy }, 0.0f, scaleS, WHITE);

        // "Super Vehicle-001 / Metal Slug" small logo centered above block
        float logoHeaderScale = 1.3f;
        float hx = (float)SW * 0.5f - (float)texLogoTop.width * logoHeaderScale * 0.5f;
        float hy = startY - (float)texLogoTop.height * logoHeaderScale - 6.0f;
        DrawTextureEx(texLogoTop, { hx, hy }, 0.0f, logoHeaderScale, WHITE);

        // texMetalSmall NOT drawn — it's redundant with texLogoTop
    }

    // Press ENTER blinking
    if (introPhase >= 6) {
        if ((int)(introTimer * 2.0f) % 2 == 0) {
            DrawText("Press ENTER to start",
                SW / 2 - MeasureText("Press ENTER to start", 20) / 2,
                (int)((float)SH * 0.88f), 20, YELLOW);
        }
    }
}