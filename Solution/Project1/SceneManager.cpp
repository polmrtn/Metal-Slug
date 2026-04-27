#include "SceneManager.hpp"
#include "raymath.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

static inline float EaseOut3(float t) { return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t); }

// ============================================================
//  SEKWENCJA INTRO:
//  PHASE 0  0.00-0.60   Czarny -> fade-in NIEBIESKIEGO tla
//  PHASE 1  0.60-2.80   Czolg wjezdza z PRAWEJ do centrum
//  PHASE 2  2.80-2.95   STRZAL: blysk + kapsulka + shake + luski
//  PHASE 3  2.95-5.00   CZARNE TLO: capsuleload animowany
//  PHASE 4  5.00-5.50   Fade black->red
//  PHASE 5  5.50-7.00   Czolg na CZERWONYM tle, jedzie w prawo
//  PHASE 6  7.00-8.00   METAL z lewej, SLUG z prawej
//  PHASE 7  8.00+       Logo stoi, PRESS ENTER miga
// ============================================================

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
//  Sprite sheet czolgu (newintrotankshit.png 1470x800)
//  Zmierzone pikselowo:
//
//  BODY FRONT (chwytaki):  x=1,   y=365, w=198, h=114
//  BODY REAR  (wiezyczka): x=211, y=365, w=148, h=114
//
//  TRACKS B (B1-B4, dolny rzad � lepsza jakosc): y=672, h=127
//    B1: x=1,   w=163
//    B2: x=210, w=163
//    B3: x=421, w=162
//    B4: x=631, w=162
//
//  Kolejnosc rysowania: gasienice -> body_rear -> body_front -> armata
//
//  Logika Y:
//    groundY          = dol gasienicy (linia ziemi)
//    trackY           = groundY - trackH
//    bodyBottomY      = trackY + trackH * TRACK_BODY_OVERLAP
//    bodyY (top)      = bodyBottomY - bodyH
//    cannonCenterY    = bodyY + bodyH * 0.35f   (srodek lufy)
// ============================================================

static const int BODY_FRONT_X = 1, BODY_FRONT_Y = 365, BODY_FRONT_W = 198, BODY_FRONT_H = 114;
static const int BODY_REAR_X = 211, BODY_REAR_Y = 365, BODY_REAR_W = 148, BODY_REAR_H = 114;

// Jaka czesc gasienicy (od gory) jest zakryta przez body
// Gasienica h=127, body h=114. Body siedzi tak ze dolne ~28px gasienicy widac
// 1 - 28/127 = 0.78 => body zakrywa 78% gasienicy od gory
static const float TRACK_BODY_OVERLAP = 0.78f;

struct TrackFrame { int x, y, w, h; };
static const TrackFrame TRACKS[4] = {
    {  1, 672, 163, 127 },
    {210, 672, 163, 127 },
    {421, 672, 162, 127 },
    {631, 672, 162, 127 },
};

// ============================================================

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

void SceneManager::SetUiManager(UiManager* u) { ui = u; }

void SceneManager::ResetIntro()
{
    introTimer = 0.0f;  introPhase = 0;
    tankX = 99999.0f;
    bulletX = -999.0f;
    metalX = -99999.0f;  slugX = 99999.0f;
    boomAlpha = 0.0f;  bgAlpha = 0.0f;
    shakeTime = 0.0f;  shakeStrength = 0.0f;
    flashAlpha = 0.0f;  trackAnim = 0.0f;
    capsuleAlpha = 0.0f;
    capsulePulse = 0.0f;
    capsuleScanY = 0.0f;
    capsuleVibration = 0.0f;
    bulletVisible = false;  bulletsSpawned = false;
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
//  Helpers geometrii
// ============================================================
static float GetTankScale(float sH)
{
    // Calkowita wysokosc czolgu w pikselach sprite'a:
    // trackH + czesc body ktora wystaje nad gasienice
    // = 127 + 114*(1-0.78) = 127 + 25 = 152px
    // Chcemy zeby czolg mial ok 32% wysokosci sceny
    float totalH_px = (float)TRACKS[0].h + (float)BODY_REAR_H * (1.0f - TRACK_BODY_OVERLAP);
    return (sH * 0.32f) / totalH_px;
}

static float GetTotalTankW(float tankScale)
{
    float cannonHalfW = (float)(444 / 2) * tankScale;  // texCannon.width=444
    float bodyRearW = (float)BODY_REAR_W * tankScale;
    float bodyFrontW = (float)BODY_FRONT_W * tankScale;
    // Body front i rear nachodza ok 5% frontu
    return cannonHalfW + bodyRearW + bodyFrontW - bodyFrontW * 0.05f;
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
    float sH = SH - (float)(border * 2);

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

    // --- PHASE 1: wjazd z prawej ---
    if (introPhase == 1)
    {
        float t = Clamp((introTimer - T_P1) / (T_P2 - T_P1 - 0.5f), 0.0f, 1.0f);
        tankX = SW + EaseOut3(t) * (centeredX - SW);
        trackAnim += dt;
    }

    // --- PHASE 2: strzal ---
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

    // --- Spawn lusek ---
    if (introPhase >= 2 && !bulletsSpawned)
    {
        bulletsSpawned = true;
        float cannonHalfW = (float)(444 / 2) * tankScale;
        float spawnLocalX = cannonHalfW * 0.55f;  // offset od tankX
        float spawnY = (float)border + sH * 0.40f;
        for (int i = 0; i < 10; i++)
        {
            Bullet2D b;
            b.x = spawnLocalX;
            b.y = spawnY;
            b.vx = (float)(rand() % 180) + 40.0f;
            b.vy = -(float)(rand() % 280) - 80.0f;
            b.alpha = 1.0f;
            b.rot = (float)(rand() % 360);
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

    // --- PHASE 3: capsuleload fade in/out + animacja ---
    if (introPhase == 3)
    {
        float dur = T_P4 - T_P3;
        float t = (introTimer - T_P3) / dur;
        if (t < 0.12f) capsuleAlpha = t / 0.12f;
        else if (t > 0.82f) capsuleAlpha = 1.0f - (t - 0.82f) / 0.18f;
        else                capsuleAlpha = 1.0f;

        capsulePulse += dt;
        capsuleScanY += dt * 0.35f;
        if (capsuleScanY > 1.0f) capsuleScanY -= 1.0f;
        capsuleVibration = (float)(rand() % 100) / 100.0f;
    }
    else
    {
        capsuleAlpha = 0.0f;
        capsulePulse = 0.0f;
        capsuleScanY = 0.0f;
        capsuleVibration = 0.0f;
    }

    // --- PHASE 4: fade black->red, czolg stoi ---
    if (introPhase == 4) { tankX = centeredX; trackAnim += dt; }

    // --- PHASE 5: jedzie w prawo ---
    if (introPhase == 5)
    {
        float t = Clamp((introTimer - T_P5) / (T_P6 - T_P5), 0.0f, 1.0f);
        tankX = centeredX + t * (SW + totalTankW + 200.0f);
        trackAnim += dt;
    }

    // --- PHASE 6: logo wjezdza ---
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

    // Paralax drzew
    if (introPhase >= 1 && introPhase != 3)
        treeScrollX += dt * 18.0f;
}

// ============================================================
//  DrawTank
//
//  tankX_  = lewa krawedz ARMATY
//  groundY = dol gasienicy (linia ziemi)
//
//  Logika Y:
//    trackY      = groundY - trackH
//    bodyBottom  = trackY + trackH * TRACK_BODY_OVERLAP
//    bodyTop     = bodyBottom - bodyH
//    cannonCenter= bodyTop + bodyH * 0.35
// ============================================================
void SceneManager::DrawTank(float tankX_, float groundY,
    float tankScale, int trackFrame,
    float ox, float oy) const
{
    const TrackFrame& tf = TRACKS[trackFrame % 4];

    float trackW = (float)tf.w * tankScale;
    float trackH = (float)tf.h * tankScale;
    float bodyRearW = (float)BODY_REAR_W * tankScale;
    float bodyRearH = (float)BODY_REAR_H * tankScale;
    float bodyFrontW = (float)BODY_FRONT_W * tankScale;
    float bodyFrontH = (float)BODY_FRONT_H * tankScale;
    float cannonHalfW = (float)(texCannon.width / 2) * tankScale;
    float cannonH = (float)texCannon.height * tankScale;

    // --- Pozycje X ---
    float bodyRearX = tankX_ + cannonHalfW;
    float bodyFrontX = bodyRearX + bodyRearW - bodyFrontW * 0.05f;

    // Gasienice centrowane pod calym body
    float totalBodyLeft = bodyRearX;
    float totalBodyRight = bodyFrontX + bodyFrontW;
    float trackX = totalBodyLeft + (totalBodyRight - totalBodyLeft) * 0.5f - trackW * 0.5f;

    // --- Pozycje Y ---
    float trackY = groundY - trackH;
    float bodyBottomY = trackY + trackH * TRACK_BODY_OVERLAP;
    float bodyY = bodyBottomY - bodyRearH;
    float cannonY = bodyY + bodyRearH * 0.35f - cannonH * 0.5f;

    // --- Rysowanie: tyl -> przod ---

    // 1. Gasienice
    Rectangle tSrc = { (float)tf.x, (float)tf.y, (float)tf.w, (float)tf.h };
    Rectangle tDst = { trackX + ox, trackY + oy, trackW, trackH };
    DrawTexturePro(texTankShit, tSrc, tDst, { 0, 0 }, 0.0f, WHITE);

    // 2. Body tyl (wiezyczka)
    Rectangle brSrc = { (float)BODY_REAR_X, (float)BODY_REAR_Y,
                        (float)BODY_REAR_W,  (float)BODY_REAR_H };
    Rectangle brDst = { bodyRearX + ox, bodyY + oy, bodyRearW, bodyRearH };
    DrawTexturePro(texTankShit, brSrc, brDst, { 0, 0 }, 0.0f, WHITE);

    // 3. Body przod (chwytaki)
    Rectangle bfSrc = { (float)BODY_FRONT_X, (float)BODY_FRONT_Y,
                        (float)BODY_FRONT_W,  (float)BODY_FRONT_H };
    Rectangle bfDst = { bodyFrontX + ox, bodyY + oy, bodyFrontW, bodyFrontH };
    DrawTexturePro(texTankShit, bfSrc, bfDst, { 0, 0 }, 0.0f, WHITE);

    // 4. Armata (lewa polowa texCannon) 
    float cHW = (float)(texCannon.width / 2);
    Rectangle cSrc = { 0.0f, 0.0f, cHW, (float)texCannon.height };
    Rectangle cDst = { tankX_ + ox, cannonY + oy, cannonHalfW, cannonH };
    DrawTexturePro(texCannon, cSrc, cDst, { 0, 0 }, 0.0f, WHITE);
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

    // Shake / vibration offset
    float ox = 0.0f, oy = 0.0f;
    if (shakeTime > 0.0f)
    {
        ox = (float)((rand() % 3) - 1) * shakeStrength;
        oy = (float)((rand() % 3) - 1) * shakeStrength;
    }
    if (introPhase == 3 && capsuleAlpha > 0.01f)
    {
        ox = (capsuleVibration - 0.5f) * 1.6f;
        oy = (capsuleVibration - 0.5f) * 1.0f;
    }

    float tankScale = GetTankScale(sH);
    float totalTankW = GetTotalTankW(tankScale);
    float groundY = sY + sH;   // dol gasienicy = gorna krawedz dolnej czarnej ramki
    int   tFrame = (int)(trackAnim * 8.0f) % 4;
    float centeredX = (float)SW * 0.5f - totalTankW * 0.5f;

    // ===== TLO =====
    auto DrawBg = [&](Texture2D tex, unsigned char alpha)
        {
            float s = std::max(sW / (float)tex.width, sH / (float)tex.height);
            Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
            Rectangle dst = { sX + ox, sY + oy, (float)tex.width * s, (float)tex.height * s };
            DrawTexturePro(tex, src, dst, { 0, 0 }, 0.0f, { 255, 255, 255, alpha });
        };

    if (introPhase <= 2)
        DrawBg(texBlueBg, (unsigned char)(bgAlpha * 255.0f));
    else if (introPhase == 3)
    { /* czarne tlo z ClearBackground */
    }
    else if (introPhase == 4)
    {
        float t = Clamp((introTimer - T_P4) / (T_P5 - T_P4), 0.0f, 1.0f);
        DrawBg(texRedBg, (unsigned char)(t * 255.0f));
    }
    else
        DrawBg(texRedBg, 255);

    // ===== DRZEWA =====
    if (introPhase >= 1 && introPhase != 3)
    {
        float treeH = (float)texTrees.height;
        float treeScale = sH / treeH;
        float treeY = sY;

        float leftSrcW = 68.0f;
        float leftDstW = leftSrcW * treeScale;
        float leftX = sX - leftDstW * 0.15f + treeScrollX * treeScale;
        DrawTexturePro(texTrees, { 0, 0, leftSrcW, treeH },
            { leftX, treeY, leftDstW, sH }, { 0, 0 }, 0.0f, WHITE);

        float midSrcX = 68.0f, midSrcW = 24.0f;
        float midDstW = midSrcW * treeScale * 2.0f;
        float midX = sX + sW * 0.35f + treeScrollX * treeScale * 0.7f;
        DrawTexturePro(texTrees, { midSrcX, 0, midSrcW, treeH },
            { midX, treeY, midDstW, sH }, { 0, 0 }, 0.0f, WHITE);
    }

    // ===== CZOLG =====
    bool drawTank = (introPhase == 1) || (introPhase == 2) ||
        (introPhase == 4) || (introPhase == 5);
    if (drawTank)
        DrawTank(tankX, groundY, tankScale, tFrame, ox, oy);

    // ===== KAPSULKA LECACA =====
    if (bulletVisible && introPhase == 2)
    {
        float capScale = tankScale;
        float cHW = (float)(texCapsuleCannon.width / 2) * capScale;
        float cH = (float)texCapsuleCannon.height * capScale;
        float capsY = sY + sH * 0.38f - cH * 0.5f;
        Rectangle src = { 0.0f, 0.0f,
                          (float)(texCapsuleCannon.width / 2),
                          (float)texCapsuleCannon.height };
        Rectangle dst = { bulletX + ox, capsY + oy, cHW, cH };
        DrawTexturePro(texCapsuleCannon, src, dst, { 0, 0 }, 0.0f, WHITE);
    }

    // ===== BLYSK STRZALU =====
    if (boomAlpha > 0.01f && introPhase == 2)
    {
        float tipX = tankX + ox;
        float tipY = sY + sH * 0.40f + oy;
        float bSc = tankScale * 0.9f;
        DrawTextureEx(texBoom,
            { tipX - texBoom.width * bSc * 0.5f,
              tipY - texBoom.height * bSc * 0.5f },
            0.0f, bSc, { 255, 255, 255, (unsigned char)(boomAlpha * 220.0f) });
        DrawTextureEx(texCannonExplosion,
            { tipX - texCannonExplosion.width * bSc * 0.5f,
              tipY - texCannonExplosion.height * bSc * 0.5f },
            0.0f, bSc, { 255, 255, 255, (unsigned char)(boomAlpha * 255.0f) });
    }

    // ===== LUSKI =====
    if (!flyingBullets.empty() && introPhase <= 3)
    {
        int   cols = 4, rows = 3;
        float bW = (float)texBullets.width / cols;
        float bH = (float)texBullets.height / rows;
        float bSc = tankScale * 0.8f;
        for (const auto& b : flyingBullets)
        {
            if (b.alpha <= 0.01f) continue;
            int frame = ((int)(b.rot / 60.0f)) % 12;
            if (frame < 0) frame += 12;
            Rectangle src = { (frame % cols) * bW, (frame / cols) * bH, bW, bH };
            Rectangle dst = { tankX + b.x + ox, b.y + oy, bW * bSc, bH * bSc };
            DrawTexturePro(texBullets, src, dst,
                { bW * bSc * 0.5f, bH * bSc * 0.5f }, b.rot,
                { 255, 255, 255, (unsigned char)(b.alpha * 255.0f) });
        }
    }

    // ===== WNETRZE CZOLGU � PHASE 3 =====
    // Statyczny obraz + animowane efekty swietlne
    if (introPhase == 3 && capsuleAlpha > 0.01f)
    {
        unsigned char baseA = (unsigned char)(capsuleAlpha * 255.0f);

        // Skaluj do pelnej sceny (letterbox)
        float scaleW = sW / (float)texCapsuleLoad.width;
        float scaleH = sH / (float)texCapsuleLoad.height;
        float s = std::max(scaleW, scaleH);
        float drawW = (float)texCapsuleLoad.width * s;
        float drawH = (float)texCapsuleLoad.height * s;
        float cx = sX + sW * 0.5f - drawW * 0.5f + ox;
        float cy = sY + sH * 0.5f - drawH * 0.5f + oy;

        DrawTexturePro(texCapsuleLoad,
            { 0, 0, (float)texCapsuleLoad.width, (float)texCapsuleLoad.height },
            { cx, cy, drawW, drawH },
            { 0, 0 }, 0.0f, { 255, 255, 255, baseA });

        // --- Pulsujace zielone swiatlo na okraglej szybie kokpitu ---
        // Szyba w centrum-prawo obrazu, ok 52% x, 42% y
        float glowCX = cx + drawW * 0.52f;
        float glowCY = cy + drawH * 0.42f;
        float glowR = drawW * 0.09f;

        // Dwa pulsowania nalozone (wolniejszy + szybszy migot)
        float p1 = 0.5f + 0.5f * sinf(capsulePulse * 9.4f);
        float p2 = 0.5f + 0.5f * sinf(capsulePulse * 23.0f + 1.0f);
        float pv = p1 * 0.7f + p2 * 0.3f;

        // Warstwy glow od zewnatrz do srodka
        for (int layer = 3; layer >= 0; layer--)
        {
            float r = glowR * (1.0f + layer * 0.65f);
            float a = pv * (0.50f - layer * 0.11f) * capsuleAlpha;
            if (a < 0.01f) continue;
            DrawCircle((int)glowCX, (int)glowCY, r,
                { 70, 210, 90, (unsigned char)(a * 255.0f) });
        }
        // Jasne jadro
        DrawCircle((int)glowCX, (int)glowCY, glowR * 0.42f,
            { 190, 255, 170, (unsigned char)(pv * 0.38f * capsuleAlpha * 255.0f) });

        // --- Scanline (refleks swiatla przesuwajacy sie od gory do dolu) ---
        float scanY1 = cy + capsuleScanY * drawH;
        float scanH = drawH * 0.055f;
        DrawRectangle((int)cx, (int)(scanY1 - scanH * 0.5f),
            (int)drawW, (int)scanH,
            { 255, 255, 255, (unsigned char)(0.07f * capsuleAlpha * 255.0f) });

        // Drugi scanline przesuniely o pol okresu
        float scanY2 = cy + fmodf(capsuleScanY + 0.5f, 1.0f) * drawH;
        DrawRectangle((int)cx, (int)(scanY2 - scanH * 0.5f),
            (int)drawW, (int)scanH,
            { 255, 255, 255, (unsigned char)(0.04f * capsuleAlpha * 255.0f) });
    }

    // ===== LOGO =====
    if (introPhase >= 6)
    {
        float scaleM = 2.2f, scaleS = 2.2f;
        float metalH = texMetalBig.height * scaleM;
        float slugH = texSlugTM.height * scaleS;
        float startY = (float)SH * 0.50f - (metalH + slugH + 4.0f) * 0.5f;
        DrawTextureEx(texMetalBig, { metalX + ox, startY + oy },
            0.0f, scaleM, WHITE);
        DrawTextureEx(texSlugTM, { slugX + ox, startY + metalH + 4.0f + oy },
            0.0f, scaleS, WHITE);
    }

    // ===== BIALY FLASH =====
    if (flashAlpha > 0.01f)
        DrawRectangle(0, 0, SW, SH,
            { 255, 255, 255, (unsigned char)(flashAlpha * 180.0f) });

    // ===== RAMKI =====
    DrawRectangle(0, 0, SW, border, BLACK);
    DrawRectangle(0, SH - border, SW, border, BLACK);

    // ===== PRESS ENTER =====
    if (introPhase >= 7 && (int)(introTimer * 2.5f) % 2 == 0)
    {
        const char* txt = "INSERT COINT!";
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

    if (currentState == TITLE)
    {
        ClearBackground(BLACK);

        float scaleW = (float)SW / texBrrrt.width;
        float scaleH = (float)SH / texBrrrt.height;
        DrawTextureEx(texBrrrt, { 0, 0 }, 0.0f, std::max(scaleW, scaleH), WHITE);

        float impactScale = 5.0f;
        int   halfW = texExplo2sprites.width / 2;
        Rectangle src = { (float)halfW, 0.0f, (float)halfW, (float)texExplo2sprites.height };
        Rectangle dst = {
            SW * 0.5f - halfW * impactScale * 0.5f,
            SH * 0.48f - texExplo2sprites.height * impactScale * 0.5f,
            (float)halfW * impactScale,
            (float)texExplo2sprites.height * impactScale
        };
        DrawTexturePro(texExplo2sprites, src, dst, { 0, 0 }, 0.0f, { 255, 255, 255, 200 });

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

        const char* txt = "INSERT COINT!";
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
        if (ui) ui->DrawCreditsOnly();
        return;
    }

    if (currentState == GAME) { ClearBackground(BLACK); return; }

    // INTRO
    ClearBackground(BLACK);
    UpdateIntro();
    DrawIntro();
    if (ui) ui->DrawCreditsOnly();
}