#include "SceneManager.hpp"
#include "raymath.h"
#include <cstdlib>
#include <cmath>

static inline float EaseOut3(float t) { return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t); }

// ============================================================
//  SEKWENCJA INTRO:
//
//  PHASE 0  0.00-0.60   Czarny ekran, fade-in NIEBIESKIEGO tla
//  PHASE 1  0.60-2.50   Czolg wjezdza z PRAWEJ do centrum (niebieskie tlo)
//  PHASE 2  2.50-2.65   STRZAL: blysk + kapsulka leci + shake + lusksi
//  PHASE 3  2.65-4.50   Widok wnetrza czolgu (capsuleload) - ladowanie
//  PHASE 4  4.50-5.00   Fade blue->red, czolg wraca
//  PHASE 5  5.00-6.50   Czolg na CZERWONYM tle, jedzie dalej w prawo
//  PHASE 6  6.50-7.50   METAL z lewej, SLUG z prawej
//  PHASE 7  7.50+       Logo stoi, PRESS ENTER miga
// ============================================================

static const float T_P0 = 0.00f;
static const float T_P1 = 0.60f;
static const float T_P2 = 2.50f;
static const float T_P3 = 2.65f;
static const float T_P4 = 4.50f;
static const float T_P5 = 5.00f;
static const float T_P6 = 6.50f;
static const float T_P7 = 7.50f;

static const float SHOT_FLASH_DUR = 0.12f;
static const float BULLET_TRAVEL = 0.22f;

// Sprite sheet czolgu - dokladne wspolrzedne (zmierzone pikselowo)
static const int BODY_X = 209, BODY_Y = 365, BODY_W = 151, BODY_H = 113;
struct TrackFrame { int x, y, w, h; };
static const TrackFrame TRACKS[4] = {
    {  0, 528, 133, 110 },
    {211, 528, 131, 110 },
    {421, 528, 136, 110 },
    {631, 528, 138, 110 },
};

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
    texBrrrt = LoadTexture("Graphics/intro/brrrt.png");
    ResetIntro();
}

SceneManager::~SceneManager()
{
    UnloadTexture(texRedBg);    UnloadTexture(texBlueBg);
    UnloadTexture(texCannon);   UnloadTexture(texCannonExplosion);
    UnloadTexture(texBoom);     UnloadTexture(texBullets);
    UnloadTexture(texExplodingPixels); UnloadTexture(texExplo2sprites);
    UnloadTexture(texTrees);    UnloadTexture(texTankShit);
    UnloadTexture(texCapsuleCannon);   UnloadTexture(texCapsuleLoad);
    UnloadTexture(texMetalBig); UnloadTexture(texSlugTM);
    UnloadTexture(texMetalSmall); UnloadTexture(texLogoTop);
    UnloadTexture(texBrrrt);
}

void SceneManager::ResetIntro()
{
    introTimer = 0.0f;
    introPhase = 0;
    tankX = 99999.0f;   // poza ekranem z prawej
    bulletX = -999.0f;
    metalX = -9999.0f;
    slugX = 99999.0f;
    boomAlpha = 0.0f;
    bgAlpha = 0.0f;
    shakeTime = 0.0f;
    shakeStrength = 0.0f;
    flashAlpha = 0.0f;
    bulletVisible = false;
    trackAnim = 0.0f;
    bulletsSpawned = false;
    pixelsSpawned = false;
    capsuleAlpha = 0.0f;
    flyingBullets.clear();
    explodingPixels.clear();
}

SceneManager::Gamestates SceneManager::GetGamestate() { return currentState; }

void SceneManager::SetGameState(Gamestates gamestate)
{
    currentState = gamestate;
    if (gamestate == INTRO) ResetIntro();
}

// ============================================================
//  UPDATE
// ============================================================
void SceneManager::UpdateIntro()
{
    float dt = GetFrameTime();
    introTimer += dt;

    float SW = (float)GetScreenWidth();
    float SH = (float)GetScreenHeight();
    int   border = 110;
    float sH = SH - border * 2.0f;
    float sW = SW;

    // Faza
    if (introTimer < T_P1) introPhase = 0;
    else if (introTimer < T_P2) introPhase = 1;
    else if (introTimer < T_P3) introPhase = 2;
    else if (introTimer < T_P4) introPhase = 3;
    else if (introTimer < T_P5) introPhase = 4;
    else if (introTimer < T_P6) introPhase = 5;
    else if (introTimer < T_P7) introPhase = 6;
    else                        introPhase = 7;

    bgAlpha = Clamp(introTimer / T_P1, 0.0f, 1.0f);

    // Skala czolgu - stala przez cale intro
    float tankScale = (sH * 0.28f) / (float)BODY_H;
    float bodyW = BODY_W * tankScale;
    float cannonHalfW = (float)(texCannon.width / 2) * tankScale;
    float cannonOverlap = cannonHalfW * 0.15f;
    float totalTankW = cannonHalfW - cannonOverlap + bodyW;

    // Pozycja docelowa: czolg wycentrowany na ekranie
    float centeredX = SW * 0.5f - totalTankW * 0.5f;

    // -------------------------------------------------------
    //  PHASE 1: czolg wjezdza z PRAWEJ, zatrzymuje sie w centrum
    // -------------------------------------------------------
    if (introPhase == 1)
    {
        float t = Clamp((introTimer - T_P1) / (T_P2 - T_P1 - 0.4f), 0.0f, 1.0f);
        float ease = EaseOut3(t);
        tankX = SW + ease * (centeredX - SW);
        trackAnim += dt;
    }

    // -------------------------------------------------------
    //  PHASE 2: STRZAL
    // -------------------------------------------------------
    if (introPhase == 2)
    {
        tankX = centeredX;   // stoi nieruchomo
        float tFlash = Clamp((introTimer - T_P2) / SHOT_FLASH_DUR, 0.0f, 1.0f);
        boomAlpha = 1.0f - tFlash;
        flashAlpha = 1.0f - tFlash;
        shakeStrength = 12.0f * (1.0f - tFlash);
        shakeTime = SHOT_FLASH_DUR;
        bulletVisible = true;

        float tBullet = Clamp((introTimer - T_P2) / BULLET_TRAVEL, 0.0f, 1.0f);
        // Kapsulka startuje z konca lufy (tankX = lewa krawedz armaty)
        bulletX = tankX + tBullet * (SW + 400.0f);
    }
    else if (introPhase < 2)
    {
        bulletVisible = false;
        boomAlpha = 0.0f;
    }
    else
    {
        bulletVisible = false;
        boomAlpha = 0.0f;
    }

    // -------------------------------------------------------
    //  Spawn lusek przy strzale
    // -------------------------------------------------------
    if (introPhase >= 2 && !bulletsSpawned)
    {
        bulletsSpawned = true;
        float lufaY = (float)border + sH * 0.42f;
        for (int i = 0; i < 8; i++)
        {
            Bullet2D b;
            b.x = cannonHalfW * 0.3f;   // wzgledem tankX
            b.y = lufaY;
            b.vx = (float)(rand() % 250) + 80.0f;
            b.vy = -(float)(rand() % 250) - 60.0f;
            b.alpha = 1.0f;
            b.rot = 0.0f;
            b.rotSpeed = (float)(rand() % 600) - 300.0f;
            flyingBullets.push_back(b);
        }
    }

    for (auto& b : flyingBullets)
    {
        b.x += b.vx * dt;
        b.y += b.vy * dt;
        b.vy += 600.0f * dt;
        b.rot += b.rotSpeed * dt;
        b.alpha = Clamp(b.alpha - dt * 1.2f, 0.0f, 1.0f);
    }

    // -------------------------------------------------------
    //  PHASE 3: capsuleload - fade in/out
    // -------------------------------------------------------
    if (introPhase == 3)
    {
        float dur = T_P4 - T_P3;
        float t = (introTimer - T_P3) / dur;
        if (t < 0.15f) capsuleAlpha = t / 0.15f;
        else if (t > 0.80f) capsuleAlpha = 1.0f - (t - 0.80f) / 0.20f;
        else                capsuleAlpha = 1.0f;
    }
    else capsuleAlpha = 0.0f;

    // -------------------------------------------------------
    //  PHASE 4: crossfade bg, czolg stoi w centrum
    // -------------------------------------------------------
    if (introPhase == 4)
    {
        tankX = centeredX;
        trackAnim += dt;
    }

    // -------------------------------------------------------
    //  PHASE 5: czolg jedzie DALEJ W PRAWO - wysuwa sie z ekranu
    // -------------------------------------------------------
    if (introPhase == 5)
    {
        float t = Clamp((introTimer - T_P5) / (T_P6 - T_P5), 0.0f, 1.0f);
        tankX = centeredX + t * (SW + totalTankW);
        trackAnim += dt;
    }

    // -------------------------------------------------------
    //  PHASE 6: METAL z lewej, SLUG z prawej
    // -------------------------------------------------------
    if (introPhase >= 6)
    {
        float t = Clamp((introTimer - T_P6) / (T_P7 - T_P6), 0.0f, 1.0f);
        float ease = EaseOut3(t);

        float scaleM = 2.2f;
        float scaleS = 2.2f;
        float mW = (float)texMetalBig.width * scaleM;
        float sW2 = (float)texSlugTM.width * scaleS;

        float metalTargetX = SW * 0.5f - mW * 0.5f - SW * 0.03f;
        metalX = -mW + ease * (metalTargetX + mW);

        float slugTargetX = SW * 0.5f - sW2 * 0.5f + SW * 0.03f;
        slugX = SW + ease * (slugTargetX - SW);
    }

    // Shake & flash decay
    if (shakeTime > 0.0f) shakeTime -= dt;
    flashAlpha = Clamp(flashAlpha - dt * 5.0f, 0.0f, 1.0f);
}

// ============================================================
//  Rysuje pelnego czolgu: gasienice + body + armata
//  baseX = lewa krawedz armaty (= najdalsza lewa czesc czolgu)
//  groundY = linia ziemi (dol gasienica)
// ============================================================
void SceneManager::DrawTank(float baseX, float groundY,
    float tankScale, int trackFrame,
    float ox, float oy) const
{
    const TrackFrame& tf = TRACKS[trackFrame];

    float bodyW = BODY_W * tankScale;
    float bodyH = BODY_H * tankScale;
    float trackW = tf.w * tankScale;
    float trackH = tf.h * tankScale;

    float cannonHalfW = (float)(texCannon.width / 2) * tankScale;
    float cannonH = (float)texCannon.height * tankScale;
    float cannonOverlap = cannonHalfW * 0.15f;

    // X body: armata wchodzi w body o cannonOverlap
    float bodyX = baseX + cannonHalfW - cannonOverlap;
    // X track: wycentrowany pod body
    float trackX = bodyX + bodyW * 0.5f - trackW * 0.5f;

    // Y: gasienice na lini ziemi, body nad gasienicami z nakladem
    float trackY = groundY - trackH;
    float bodyY = trackY - bodyH + trackH * 0.42f;

    // Armata: pionowo wycentrowana na gornej polowie body
    float cannonY = bodyY + bodyH * 0.18f - cannonH * 0.5f;

    // 1. Gasienice
    Rectangle tSrc = { (float)tf.x, (float)tf.y, (float)tf.w, (float)tf.h };
    Rectangle tDst = { trackX + ox, trackY + oy, trackW, trackH };
    DrawTexturePro(texTankShit, tSrc, tDst, { 0,0 }, 0.0f, WHITE);

    // 2. Body
    Rectangle bSrc = { (float)BODY_X,(float)BODY_Y,(float)BODY_W,(float)BODY_H };
    Rectangle bDst = { bodyX + ox, bodyY + oy, bodyW, bodyH };
    DrawTexturePro(texTankShit, bSrc, bDst, { 0,0 }, 0.0f, WHITE);

    // 3. Armata (lewa polowa sprite sheeta = armata z boku)
    float cHW = (float)(texCannon.width / 2);
    Rectangle cSrc = { 0.0f, 0.0f, cHW, (float)texCannon.height };
    Rectangle cDst = { baseX + ox, cannonY + oy, cannonHalfW, cannonH };
    DrawTexturePro(texCannon, cSrc, cDst, { 0,0 }, 0.0f, WHITE);
}

// ============================================================
//  DRAW INTRO
// ============================================================
void SceneManager::DrawIntro()
{
    int   SW = GetScreenWidth();
    int   SH = GetScreenHeight();
    int   border = 110;
    float sX = 0.0f;
    float sY = (float)border;
    float sW = (float)SW;
    float sH = (float)(SH - border * 2);

    float ox = 0.0f, oy = 0.0f;
    if (shakeTime > 0.0f)
    {
        ox = (float)((rand() % 3) - 1) * shakeStrength;
        oy = (float)((rand() % 3) - 1) * shakeStrength;
    }

    // ===========================
    //  TLO
    // ===========================
    auto DrawBg = [&](Texture2D tex, unsigned char alpha)
        {
            float scaleW = sW / tex.width;
            float scaleH = sH / tex.height;
            float s = (scaleW > scaleH) ? scaleW : scaleH;
            Rectangle src = { 0,0,(float)tex.width,(float)tex.height };
            Rectangle dst = { sX + ox, sY + oy, tex.width * s, tex.height * s };
            DrawTexturePro(tex, src, dst, { 0,0 }, 0.0f, { 255,255,255,alpha });
        };

    if (introPhase <= 2)
    {
        DrawBg(texBlueBg, (unsigned char)(bgAlpha * 255.0f));
    }
    else if (introPhase == 3)
    {
        DrawBg(texBlueBg, 255);
    }
    else if (introPhase == 4)
    {
        float t = Clamp((introTimer - T_P4) / (T_P5 - T_P4), 0.0f, 1.0f);
        DrawBg(texBlueBg, 255);
        DrawBg(texRedBg, (unsigned char)(t * 255.0f));
    }
    else
    {
        DrawBg(texRedBg, 255);
    }

    // ===========================
    //  DRZEWA
    // ===========================
    if (introPhase >= 1 && introPhase != 3)
    {
        float treeScale = sW / (float)texTrees.width;
        float treeY = sY + sH - (float)texTrees.height * treeScale;
        DrawTextureEx(texTrees, { sX + ox, treeY + oy }, 0.0f, treeScale, WHITE);
    }

    // ===========================
    //  CZOLG
    // ===========================
    float tankScale = (sH * 0.28f) / (float)BODY_H;
    float groundY = sY + sH * 0.82f;
    int   tFrame = (int)(trackAnim * 8.0f) % 4;

    bool drawTank = (introPhase >= 1 && introPhase <= 2) ||
        (introPhase == 4) ||
        (introPhase == 5);

    if (drawTank)
    {
        DrawTank(tankX, groundY, tankScale, tFrame, ox, oy);
    }

    // ===========================
    //  KAPSULKA - leci po strzale
    // ===========================
    if (bulletVisible && introPhase == 2)
    {
        float capScale = tankScale * 1.0f;
        float cHW = (float)(texCapsuleCannon.width / 2) * capScale;
        float cH = (float)texCapsuleCannon.height * capScale;
        float capsY = sY + sH * 0.38f - cH * 0.5f;

        Rectangle src = { 0.0f, 0.0f, (float)(texCapsuleCannon.width / 2),
                         (float)texCapsuleCannon.height };
        Rectangle dst = { bulletX + ox, capsY + oy, cHW, cH };
        DrawTexturePro(texCapsuleCannon, src, dst, { 0,0 }, 0.0f, WHITE);
    }

    // ===========================
    //  BLYSK STRZALU
    // ===========================
    if (boomAlpha > 0.01f && introPhase == 2)
    {
        float tipX = tankX + ox;
        float tipY = sY + sH * 0.42f + oy;
        float bSc = tankScale * 0.9f;

        DrawTextureEx(texBoom,
            { tipX - texBoom.width * bSc * 0.5f, tipY - texBoom.height * bSc * 0.5f },
            0.0f, bSc, { 255,255,255,(unsigned char)(boomAlpha * 220.0f) });
        DrawTextureEx(texCannonExplosion,
            { tipX - texCannonExplosion.width * bSc * 0.5f,
             tipY - texCannonExplosion.height * bSc * 0.5f },
            0.0f, bSc, { 255,255,255,(unsigned char)(boomAlpha * 255.0f) });
    }

    // ===========================
    //  LUSKSI
    // ===========================
    if (!flyingBullets.empty())
    {
        int   cols = 4, rows = 3;
        float bW = (float)texBullets.width / cols;
        float bH = (float)texBullets.height / rows;
        float bSc = tankScale * 0.8f;

        for (const auto& b : flyingBullets)
        {
            if (b.alpha <= 0.01f) continue;
            int frame = (int)(b.rot / 60.0f) % 12;
            if (frame < 0) frame += 12;
            int col = frame % cols, row = frame / cols;
            Rectangle src = { col * bW, row * bH, bW, bH };
            Rectangle dst = { tankX + b.x + ox, b.y + oy, bW * bSc, bH * bSc };
            DrawTexturePro(texBullets, src, dst,
                { bW * bSc * 0.5f, bH * bSc * 0.5f }, b.rot,
                { 255,255,255,(unsigned char)(b.alpha * 255.0f) });
        }
    }

    // ===========================
    //  WNETRZE CZOLGU (capsuleload) - phase 3
    // ===========================
    if (capsuleAlpha > 0.01f)
    {
        float scaleW = sW / (float)texCapsuleLoad.width;
        float scaleH = sH / (float)texCapsuleLoad.height;
        float s = (scaleW > scaleH) ? scaleW : scaleH;
        float drawW = texCapsuleLoad.width * s;
        float drawH = texCapsuleLoad.height * s;
        float cx = sX + sW * 0.5f - drawW * 0.5f;
        float cy = sY + sH * 0.5f - drawH * 0.5f;

        Rectangle src = { 0,0,(float)texCapsuleLoad.width,(float)texCapsuleLoad.height };
        Rectangle dst = { cx, cy, drawW, drawH };
        DrawTexturePro(texCapsuleLoad, src, dst, { 0,0 }, 0.0f,
            { 255,255,255,(unsigned char)(capsuleAlpha * 255.0f) });
    }

    // ===========================
    //  LOGO: METAL + SLUG
    // ===========================
    if (introPhase >= 6)
    {
        float scaleM = 2.2f, scaleS = 2.2f;
        float metalH = texMetalBig.height * scaleM;
        float slugH = texSlugTM.height * scaleS;
        float totalH = metalH + slugH + 4.0f;
        float startY = (float)SH * 0.50f - totalH * 0.5f;

        DrawTextureEx(texMetalBig, { metalX + ox, startY + oy }, 0.0f, scaleM, WHITE);
        DrawTextureEx(texSlugTM,
            { slugX + ox, startY + metalH + 4.0f + oy }, 0.0f, scaleS, WHITE);
    }

    // ===========================
    //  BIALY FLASH
    // ===========================
    if (flashAlpha > 0.01f)
        DrawRectangle(0, 0, SW, SH, { 255,255,255,(unsigned char)(flashAlpha * 180.0f) });

    // ===========================
    //  RAMKA gora/dol
    // ===========================
    DrawRectangle(0, 0, SW, border, BLACK);
    DrawRectangle(0, SH - border, SW, border, BLACK);

    // ===========================
    //  PRESS ENTER
    // ===========================
    if (introPhase >= 7)
    {
        if ((int)(introTimer * 2.5f) % 2 == 0)
        {
            const char* txt = "PRESS ENTER";
            int fs = 26, tw = MeasureText(txt, fs);
            DrawText(txt, SW / 2 - tw / 2 + 2, (int)(SH * 0.87f) + 2, fs, BLACK);
            DrawText(txt, SW / 2 - tw / 2, (int)(SH * 0.87f), fs, YELLOW);
        }
    }
}

// ============================================================
//  DrawTexts
// ============================================================
void SceneManager::DrawTexts()
{
    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    if (currentState == TITLE)
    {
        ClearBackground(BLACK);
        float scaleW = (float)SW / texBrrrt.width;
        float scaleH = (float)SH / texBrrrt.height;
        float scale = (scaleW > scaleH) ? scaleW : scaleH;
        DrawTextureEx(texBrrrt, { 0,0 }, 0.0f, scale, WHITE);

        float impactScale = 3.0f;
        int   halfW = texExplo2sprites.width / 2;
        Rectangle src = { (float)halfW, 0.0f, (float)halfW, (float)texExplo2sprites.height };
        Rectangle dst = { SW * 0.5f - halfW * impactScale * 0.5f,
                         SH * 0.42f - texExplo2sprites.height * impactScale * 0.5f,
                         (float)halfW * impactScale,(float)texExplo2sprites.height * impactScale };
        DrawTexturePro(texExplo2sprites, src, dst, { 0,0 }, 0.0f, { 255,255,255,200 });

        float scaleM = 2.5f, scaleS = 2.5f;
        float totalH = texMetalBig.height * scaleM + texSlugTM.height * scaleS + 8.0f;
        float startY = SH * 0.45f - totalH * 0.5f;
        DrawTextureEx(texMetalBig, { SW * 0.5f - texMetalBig.width * scaleM * 0.5f, startY }, 0.0f, scaleM, WHITE);
        DrawTextureEx(texSlugTM, { SW * 0.5f - texSlugTM.width * scaleS * 0.5f,
                                   startY + texMetalBig.height * scaleM + 8.0f }, 0.0f, scaleS, WHITE);

        const char* txt = "PUSH ENTER TO START!";
        if ((int)(GetTime() * 2.5f) % 2 == 0)
        {
            int fs = 28, tw = MeasureText(txt, fs);
            DrawText(txt, SW / 2 - tw / 2 + 2, (int)(SH * 0.82f) + 2, fs, BLACK);
            DrawText(txt, SW / 2 - tw / 2, (int)(SH * 0.82f), fs, YELLOW);
        }
        const char* footer = "2026 KURVVA PRODUCTIONS";
        int fSize = 20, fw = MeasureText(footer, fSize);
        DrawText(footer, SW / 2 - fw / 2 + 2, (int)(SH * 0.92f) + 2, fSize, BLACK);
        DrawText(footer, SW / 2 - fw / 2, (int)(SH * 0.92f), fSize, WHITE);
        return;
    }

    if (currentState == GAME) { ClearBackground(BLACK); return; }

    ClearBackground(BLACK);
    UpdateIntro();
    DrawIntro();
}