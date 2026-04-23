#include "SceneManager.hpp"
#include "raymath.h"
#include <cstdlib>
#include <cmath>

static inline float EaseOut3(float t) { return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t); }

// ============================================================
//  SEKWENCJA INTRO:
//  PHASE 0  0.00-0.60   Czarny -> fade-in NIEBIESKIEGO tla
//  PHASE 1  0.60-2.80   Czolg wjezdza z PRAWEJ do centrum (niebieskie tlo)
//  PHASE 2  2.80-2.95   STRZAL: blysk + kapsulka + shake + lusksi
//  PHASE 3  2.95-5.00   CZARNE TLO: capsuleload wycentrowany, armata lewy dolny rog
//  PHASE 4  5.00-5.50   Fade black->red
//  PHASE 5  5.50-7.00   Czolg na CZERWONYM tle, jedzie dalej w prawo
//  PHASE 6  7.00-8.00   METAL z lewej, SLUG z prawej
//  PHASE 7  8.00+       Logo stoi, PRESS ENTER miga
// ============================================================

static const float T_P0 = 0.00f;
static const float T_P1 = 0.60f;
static const float T_P2 = 2.80f;
static const float T_P3 = 2.95f;
static const float T_P4 = 5.00f;
static const float T_P5 = 5.50f;
static const float T_P6 = 7.00f;
static const float T_P7 = 8.00f;

static const float SHOT_FLASH_DUR = 0.12f;
static const float BULLET_TRAVEL = 0.22f;

// ============================================================
//  Sprite sheet czolgu - zmierzone pikselowo
//  body_rear  = tyl z wiezyczka:  x=209, y=365, w=151, h=113
//  body_front = przod z chwytakiem: x=0,  y=365, w=200, h=113
//  Gasienice (4 klatki):
// ============================================================
static const int BODY_REAR_X = 209, BODY_REAR_Y = 365, BODY_REAR_W = 151, BODY_REAR_H = 113;
static const int BODY_FRONT_X = 0, BODY_FRONT_Y = 365, BODY_FRONT_W = 200, BODY_FRONT_H = 113;

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
    introTimer = 0.0f;  introPhase = 0;
    tankX = 99999.0f;
    bulletX = -999.0f;
    metalX = -99999.0f; slugX = 99999.0f;
    boomAlpha = 0.0f;  bgAlpha = 0.0f;
    shakeTime = 0.0f;  shakeStrength = 0.0f;
    flashAlpha = 0.0f;  trackAnim = 0.0f;
    capsuleAlpha = 0.0f;
    bulletVisible = false; bulletsSpawned = false;
    pixelsSpawned = false;
    treeScrollX = 0.0f;
    flyingBullets.clear();
    explodingPixels.clear();
}

SceneManager::Gamestates SceneManager::GetGamestate() { return currentState; }
void SceneManager::SetGameState(Gamestates gs)
{
    currentState = gs;
    if (gs == INTRO) ResetIntro();
}

// ============================================================
//  Oblicz skale i geometry czolgu
//  tankScale bazuje na wysokosci sceny
//  Zwraca totalTankW = szerokosc calego czolgu (armata+oba body)
// ============================================================
static float GetTankScale(float sH)
{
    // Body rear jest referencja wysokosci
    return (sH * 0.28f) / (float)BODY_REAR_H;
}

static float GetTotalTankW(float tankScale)
{
    float cannonHalfW = (float)(444 / 2) * tankScale;  // texCannon.width=444
    float cannonOverlap = cannonHalfW * 0.17f;
    float bodyRearW = BODY_REAR_W * tankScale;
    float bodyFrontW = BODY_FRONT_W * tankScale;
    return cannonHalfW - cannonOverlap + bodyRearW + bodyFrontW - bodyFrontW * 0.05f;
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

    if (introTimer < T_P1) introPhase = 0;
    else if (introTimer < T_P2) introPhase = 1;
    else if (introTimer < T_P3) introPhase = 2;
    else if (introTimer < T_P4) introPhase = 3;
    else if (introTimer < T_P5) introPhase = 4;
    else if (introTimer < T_P6) introPhase = 5;
    else if (introTimer < T_P7) introPhase = 6;
    else                        introPhase = 7;

    bgAlpha = Clamp(introTimer / T_P1, 0.0f, 1.0f);

    float tankScale = GetTankScale(sH);
    float totalTankW = GetTotalTankW(tankScale);
    float centeredX = SW * 0.5f - totalTankW * 0.5f;

    // PHASE 1: czolg wjezdza z prawej
    if (introPhase == 1)
    {
        float t = Clamp((introTimer - T_P1) / (T_P2 - T_P1 - 0.5f), 0.0f, 1.0f);
        tankX = SW + EaseOut3(t) * (centeredX - SW);
        trackAnim += dt;
    }

    // PHASE 2: STRZAL
    if (introPhase == 2)
    {
        tankX = centeredX;
        float tF = Clamp((introTimer - T_P2) / SHOT_FLASH_DUR, 0.0f, 1.0f);
        boomAlpha = 1.0f - tF;
        flashAlpha = 1.0f - tF;
        shakeStrength = 14.0f * (1.0f - tF);
        shakeTime = SHOT_FLASH_DUR;
        bulletVisible = true;

        float tB = Clamp((introTimer - T_P2) / BULLET_TRAVEL, 0.0f, 1.0f);
        bulletX = tankX + tB * (SW + 400.0f);
    }
    else if (introPhase < 2) { bulletVisible = false; boomAlpha = 0.0f; }
    else { bulletVisible = false; boomAlpha = 0.0f; }

    // Spawn lusek
    if (introPhase >= 2 && !bulletsSpawned)
    {
        bulletsSpawned = true;
        float lufaY = (float)border + sH * 0.42f;
        float cannonHalfW = (float)(444 / 2) * tankScale;
        for (int i = 0; i < 8; i++)
        {
            Bullet2D b;
            b.x = cannonHalfW * 0.3f;
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
        b.x += b.vx * dt; b.y += b.vy * dt;
        b.vy += 600.0f * dt;
        b.rot += b.rotSpeed * dt;
        b.alpha = Clamp(b.alpha - dt * 1.2f, 0.0f, 1.0f);
    }

    // PHASE 3: capsuleload fade in/out
    if (introPhase == 3)
    {
        float dur = T_P4 - T_P3;
        float t = (introTimer - T_P3) / dur;
        if (t < 0.12f) capsuleAlpha = t / 0.12f;
        else if (t > 0.82f) capsuleAlpha = 1.0f - (t - 0.82f) / 0.18f;
        else                capsuleAlpha = 1.0f;
    }
    else capsuleAlpha = 0.0f;

    // PHASE 4: crossfade black->red, czolg stoi
    if (introPhase == 4) { tankX = centeredX; trackAnim += dt; }

    // PHASE 5: czolg jedzie w prawo i wychodzi
    if (introPhase == 5)
    {
        float t = Clamp((introTimer - T_P5) / (T_P6 - T_P5), 0.0f, 1.0f);
        tankX = centeredX + t * (SW + totalTankW + 200.0f);
        trackAnim += dt;
    }

    // PHASE 6: logo wjezdza
    if (introPhase >= 6)
    {
        float t = Clamp((introTimer - T_P6) / (T_P7 - T_P6), 0.0f, 1.0f);
        float ease = EaseOut3(t);
        float scaleM = 2.2f, scaleS = 2.2f;
        float mW = texMetalBig.width * scaleM;
        float sW2 = texSlugTM.width * scaleS;
        metalX = -mW + ease * (SW * 0.5f - mW * 0.5f - SW * 0.03f + mW);
        slugX = SW + ease * (SW * 0.5f - sW2 * 0.5f + SW * 0.03f - SW);
    }

    if (shakeTime > 0.0f) shakeTime -= dt;
    flashAlpha = Clamp(flashAlpha - dt * 5.0f, 0.0f, 1.0f);

    // Drzewa powoli ida w prawo (paralax)
    if (introPhase >= 1 && introPhase != 3)
        treeScrollX += dt * 18.0f;   // px/s w naturalnej skali drzewa
}

// ============================================================
//  DrawTank
//  tankX  = lewa krawedz armaty (najdalsza lewa czesc czolgu)
//  groundY = linia ziemi = dol gasienica
// ============================================================
void SceneManager::DrawTank(float tankX_, float groundY,
    float tankScale, int trackFrame,
    float ox, float oy) const
{
    const TrackFrame& tf = TRACKS[trackFrame];

    float cannonHalfW = (float)(texCannon.width / 2) * tankScale;
    float cannonH = (float)texCannon.height * tankScale;
    float cannonOverlap = cannonHalfW * 0.17f;

    float bodyRearW = BODY_REAR_W * tankScale;
    float bodyRearH = BODY_REAR_H * tankScale;
    float bodyFrontW = BODY_FRONT_W * tankScale;
    float bodyFrontH = BODY_FRONT_H * tankScale;

    float trackW = tf.w * tankScale;
    float trackH = tf.h * tankScale;

    // X pozycje:
    // tankX_ = lewa krawedz armaty
    float bodyRearX = tankX_ + cannonHalfW - cannonOverlap;
    float bodyFrontX = bodyRearX + bodyRearW - bodyFrontW * 0.05f;
    float totalBodyW = bodyFrontX + bodyFrontW - bodyRearX;
    float trackX = bodyRearX + totalBodyW * 0.5f - trackW * 0.5f;

    // Y pozycje:
    float trackY = groundY - trackH;
    float bodyY = trackY - bodyRearH + trackH * 0.42f;
    float cannonY = bodyY + bodyRearH * 0.22f - cannonH * 0.5f;

    // 1. Gasienice (pod spodem)
    Rectangle tSrc = { (float)tf.x,(float)tf.y,(float)tf.w,(float)tf.h };
    Rectangle tDst = { trackX + ox, trackY + oy, trackW, trackH };
    DrawTexturePro(texTankShit, tSrc, tDst, { 0,0 }, 0.0f, WHITE);

    // 2. Body tyl (wiezyczka) - rysowane pierwsze
    Rectangle brSrc = { (float)BODY_REAR_X,(float)BODY_REAR_Y,
                        (float)BODY_REAR_W,(float)BODY_REAR_H };
    Rectangle brDst = { bodyRearX + ox, bodyY + oy, bodyRearW, bodyRearH };
    DrawTexturePro(texTankShit, brSrc, brDst, { 0,0 }, 0.0f, WHITE);

    // 3. Body przod (chwytaki) - na wierzchu tylu
    Rectangle bfSrc = { (float)BODY_FRONT_X,(float)BODY_FRONT_Y,
                        (float)BODY_FRONT_W,(float)BODY_FRONT_H };
    Rectangle bfDst = { bodyFrontX + ox, bodyY + oy, bodyFrontW, bodyFrontH };
    DrawTexturePro(texTankShit, bfSrc, bfDst, { 0,0 }, 0.0f, WHITE);

    // 4. Armata (lewa polowa cannon sprite sheet)
    float cHW = (float)(texCannon.width / 2);
    Rectangle cSrc = { 0.0f, 0.0f, cHW, (float)texCannon.height };
    Rectangle cDst = { tankX_ + ox, cannonY + oy, cannonHalfW, cannonH };
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

    float tankScale = GetTankScale(sH);
    float totalTankW = GetTotalTankW(tankScale);
    float groundY = sY + sH * 0.82f;
    int   tFrame = (int)(trackAnim * 8.0f) % 4;
    float centeredX = (float)SW * 0.5f - totalTankW * 0.5f;

    // ===== TLO =====
    auto DrawBg = [&](Texture2D tex, unsigned char alpha)
        {
            float s = std::max(sW / tex.width, sH / tex.height);
            Rectangle src = { 0,0,(float)tex.width,(float)tex.height };
            Rectangle dst = { sX + ox, sY + oy, tex.width * s, tex.height * s };
            DrawTexturePro(tex, src, dst, { 0,0 }, 0.0f, { 255,255,255,alpha });
        };

    if (introPhase <= 2)
        DrawBg(texBlueBg, (unsigned char)(bgAlpha * 255.0f));
    else if (introPhase == 3)
    {
        // Czarne tlo dla capsuleload - nic nie rysujemy (ClearBackground juz je dalo)
    }
    else if (introPhase == 4)
    {
        float t = Clamp((introTimer - T_P4) / (T_P5 - T_P4), 0.0f, 1.0f);
        DrawBg(texRedBg, (unsigned char)(t * 255.0f));
    }
    else
        DrawBg(texRedBg, 255);

    // ===== DRZEWA - dwa drzewa (lewe + srodkowe), bez zielonego slupa =====
    // Sprite texTrees 192x192:
    //   drzewo lewe:      x=0..67   (szerokosc ~68px)
    //   drzewo srodkowe:  x=68..91  (szerokosc ~24px cienkie)
    //   zielony slup:     x=128..143 - POMIJAMY
    //
    // Skalujemy wysokosc drzew na cala wysokosc sceny
    if (introPhase >= 1 && introPhase != 3)
    {
        float treeH = (float)texTrees.height;
        float treeScale = sH / treeH;    // drzewa na pelna wysokosc sceny
        float treeY = sY;

        // --- Drzewo lewe (x=0..67 w sprite) ---
        // Pozycja: z lewej strony, przesuwa sie w prawo
        float leftTreeSrcW = 68.0f;
        float leftTreeDstW = leftTreeSrcW * treeScale;
        float leftTreeX = sX - leftTreeDstW * 0.15f + treeScrollX * treeScale;

        Rectangle srcL = { 0.0f, 0.0f, leftTreeSrcW, treeH };
        Rectangle dstL = { leftTreeX, treeY, leftTreeDstW, sH };
        DrawTexturePro(texTrees, srcL, dstL, { 0,0 }, 0.0f, WHITE);

        // --- Drzewo srodkowe (x=68..91 w sprite) - bardziej po prawej =====
        float midTreeSrcX = 68.0f;
        float midTreeSrcW = 24.0f;
        float midTreeDstW = midTreeSrcW * treeScale * 2.0f;  // troche szerzej
        // bazowa pozycja: ok 35% ekranu + scroll
        float midTreeX = sX + sW * 0.35f + treeScrollX * treeScale * 0.7f;

        Rectangle srcM = { midTreeSrcX, 0.0f, midTreeSrcW, treeH };
        Rectangle dstM = { midTreeX, treeY, midTreeDstW, sH };
        DrawTexturePro(texTrees, srcM, dstM, { 0,0 }, 0.0f, WHITE);
    }

    // ===== CZOLG =====
    bool drawTank = (introPhase == 1) || (introPhase == 2) || (introPhase == 4) || (introPhase == 5);
    if (drawTank)
        DrawTank(tankX, groundY, tankScale, tFrame, ox, oy);

    // ===== KAPSULKA (pocisk) =====
    if (bulletVisible && introPhase == 2)
    {
        float capScale = tankScale * 1.0f;
        float cHW = (float)(texCapsuleCannon.width / 2) * capScale;
        float cH = (float)texCapsuleCannon.height * capScale;
        float capsY = sY + sH * 0.38f - cH * 0.5f;
        Rectangle src = { 0.0f,0.0f,(float)(texCapsuleCannon.width / 2),(float)texCapsuleCannon.height };
        Rectangle dst = { bulletX + ox, capsY + oy, cHW, cH };
        DrawTexturePro(texCapsuleCannon, src, dst, { 0,0 }, 0.0f, WHITE);
    }

    // ===== BLYSK STRZALU =====
    if (boomAlpha > 0.01f && introPhase == 2)
    {
        float tipX = tankX + ox;
        float tipY = sY + sH * 0.42f + oy;
        float bSc = tankScale * 0.9f;
        DrawTextureEx(texBoom,
            { tipX - texBoom.width * bSc * 0.5f, tipY - texBoom.height * bSc * 0.5f },
            0.0f, bSc, { 255,255,255,(unsigned char)(boomAlpha * 220.0f) });
        DrawTextureEx(texCannonExplosion,
            { tipX - texCannonExplosion.width * bSc * 0.5f, tipY - texCannonExplosion.height * bSc * 0.5f },
            0.0f, bSc, { 255,255,255,(unsigned char)(boomAlpha * 255.0f) });
    }

    // ===== LUSKSI =====
    if (!flyingBullets.empty() && introPhase != 3)
    {
        int cols = 4, rows = 3;
        float bW = (float)texBullets.width / cols;
        float bH = (float)texBullets.height / rows;
        float bSc = tankScale * 0.8f;
        for (const auto& b : flyingBullets)
        {
            if (b.alpha <= 0.01f) continue;
            int frame = ((int)(b.rot / 60.0f)) % 12; if (frame < 0) frame += 12;
            Rectangle src = { (frame % cols) * bW,(frame / cols) * bH,bW,bH };
            Rectangle dst = { tankX + b.x + ox, b.y + oy, bW * bSc, bH * bSc };
            DrawTexturePro(texBullets, src, dst,
                { bW * bSc * 0.5f,bH * bSc * 0.5f }, b.rot,
                { 255,255,255,(unsigned char)(b.alpha * 255.0f) });
        }
    }

    // ===== WNETRZE CZOLGU (phase 3) =====
    // Czarne tlo + capsuleload wycentrowany - BEZ ARMATY
    if (introPhase == 3)
    {
        if (capsuleAlpha > 0.01f)
        {
            float scaleW = sW / (float)texCapsuleLoad.width;
            float scaleH = sH / (float)texCapsuleLoad.height;
            float s = std::max(scaleW, scaleH);
            float drawW = texCapsuleLoad.width * s;
            float drawH = texCapsuleLoad.height * s;
            float cx = sX + sW * 0.5f - drawW * 0.5f;
            float cy = sY + sH * 0.5f - drawH * 0.5f;
            Rectangle src = { 0,0,(float)texCapsuleLoad.width,(float)texCapsuleLoad.height };
            Rectangle dst = { cx, cy, drawW, drawH };
            DrawTexturePro(texCapsuleLoad, src, dst, { 0,0 }, 0.0f,
                { 255,255,255,(unsigned char)(capsuleAlpha * 255.0f) });
        }
    }

    // ===== LOGO =====
    if (introPhase >= 6)
    {
        float scaleM = 2.2f, scaleS = 2.2f;
        float metalH = texMetalBig.height * scaleM;
        float slugH = texSlugTM.height * scaleS;
        float startY = (float)SH * 0.50f - (metalH + slugH + 4.0f) * 0.5f;
        DrawTextureEx(texMetalBig, { metalX + ox, startY + oy }, 0.0f, scaleM, WHITE);
        DrawTextureEx(texSlugTM, { slugX + ox,  startY + metalH + 4.0f + oy }, 0.0f, scaleS, WHITE);
    }

    // ===== BIALY FLASH =====
    if (flashAlpha > 0.01f)
        DrawRectangle(0, 0, SW, SH, { 255,255,255,(unsigned char)(flashAlpha * 180.0f) });

    // ===== RAMKA =====
    DrawRectangle(0, 0, SW, border, BLACK);
    DrawRectangle(0, SH - border, SW, border, BLACK);

    // ===== PRESS ENTER =====
    if (introPhase >= 7 && (int)(introTimer * 2.5f) % 2 == 0)
    {
        const char* txt = "PRESS ENTER";
        int fs = 26, tw = MeasureText(txt, fs);
        DrawText(txt, SW / 2 - tw / 2 + 2, (int)(SH * 0.87f) + 2, fs, BLACK);
        DrawText(txt, SW / 2 - tw / 2, (int)(SH * 0.87f), fs, YELLOW);
    }
}

// ============================================================
//  DrawTexts
// ============================================================
void SceneManager::DrawTexts()
{
    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    // ===== TITLE =====
    if (currentState == TITLE)
    {
        ClearBackground(BLACK);

        // Tlo brrrt
        float scaleW = (float)SW / texBrrrt.width, scaleH = (float)SH / texBrrrt.height;
        DrawTextureEx(texBrrrt, { 0,0 }, 0.0f, std::max(scaleW, scaleH), WHITE);

        // Bullet hole - WIEKSZY niz poprzednio (impactScale 5.0)
        float impactScale = 5.0f;
        int   halfW = texExplo2sprites.width / 2;
        Rectangle src = { (float)halfW, 0.0f, (float)halfW, (float)texExplo2sprites.height };
        Rectangle dst = {
            SW * 0.5f - halfW * impactScale * 0.5f,
            SH * 0.48f - texExplo2sprites.height * impactScale * 0.5f,
            (float)halfW * impactScale, (float)texExplo2sprites.height * impactScale
        };
        DrawTexturePro(texExplo2sprites, src, dst, { 0,0 }, 0.0f, { 255,255,255,200 });

        // Logo METAL SLUG - WIEKSZE (scale 3.2)
        float scaleM = 3.2f, scaleS = 3.2f;
        float totalH = texMetalBig.height * scaleM + texSlugTM.height * scaleS + 10.0f;
        float startY = SH * 0.46f - totalH * 0.5f;
        DrawTextureEx(texMetalBig,
            { SW * 0.5f - texMetalBig.width * scaleM * 0.5f, startY },
            0.0f, scaleM, WHITE);
        DrawTextureEx(texSlugTM,
            { SW * 0.5f - texSlugTM.width * scaleS * 0.5f,
             startY + texMetalBig.height * scaleM + 10.0f },
            0.0f, scaleS, WHITE);

        // PUSH ENTER
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

    // ===== INTRO =====
    ClearBackground(BLACK);
    UpdateIntro();
    DrawIntro();
}