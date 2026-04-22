#include "SceneManager.hpp"
#include "raymath.h"
#include <cstdlib>
#include <cmath>

// ============================================================
//  Pomocnicze inline
// ============================================================
static inline float EaseOut2(float t) { return 1.0f - (1.0f - t) * (1.0f - t); }
static inline float EaseOut3(float t) { return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t); }

// ============================================================
//  Fazy intro (czas w sekundach):
//
//  0.00 - 0.50  PHASE 0 : czarny ekran / fade-in czerwonego tla
//  0.50 - 1.20  PHASE 1 : dzialo wjezdza z lewej + drzewa + tank
//  1.20 - 1.35  PHASE 2 : strzal - blysk + kapsulka leci w prawo + shake
//  1.35 - 3.00  PHASE 3 : lusksi lecace + exploding pixels + kapsulka znika
//  3.00 - 4.00  PHASE 4 : crossfade red->blue bg + dzialo wysuwa sie dalej
//  4.00 - 4.80  PHASE 5 : METAL wjezdza z lewej, SLUG z prawej
//  4.80 - 6.00  PHASE 6 : logo zatrzymane, "SUPER VEHICLE-001" pojawia sie
//  6.00+        PHASE 7 : "PRESS ENTER" miga
// ============================================================

// Granice faz
static const float T_P0 = 0.0f;
static const float T_P1 = 0.50f;
static const float T_P2 = 1.20f;
static const float T_P3 = 1.35f;
static const float T_P4 = 3.00f;
static const float T_P5 = 4.00f;
static const float T_P6 = 4.80f;
static const float T_P7 = 6.00f;

// Czas trwania strzalu (blysk + kapsulka)
static const float SHOT_FLASH_DUR = 0.12f;   // czas trwania blysku
static const float BULLET_TRAVEL = 0.18f;   // czas przelotu kapsulki przez caly ekran

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
    UnloadTexture(texBrrrt);
}

void SceneManager::ResetIntro()
{
    introTimer = 0.0f;
    introPhase = 0;
    cannonX = -900.0f;
    bulletX = -999.0f;   // poza ekranem - nie rysujemy
    logoY = 0.0f;
    metalX = -900.0f;
    slugX = 9999.0f;
    boomAlpha = 0.0f;
    boomScale = 1.0f;
    bgAlpha = 0.0f;
    shakeTime = 0.0f;
    shakeStrength = 0.0f;
    flashAlpha = 0.0f;
    bulletT = 0.0f;
    bulletsSpawned = false;
    bulletVisible = false;
    trackAnim = 0.0f;
    pixelsSpawned = false;
    flyingBullets.clear();
    explodingPixels.clear();
}

SceneManager::Gamestates SceneManager::GetGamestate()
{
    return currentState;
}

void SceneManager::SetGameState(Gamestates gamestate)
{
    currentState = gamestate;
    if (gamestate == INTRO)
        ResetIntro();
}

// ============================================================
//  UPDATE INTRO
// ============================================================
void SceneManager::UpdateIntro()
{
    float dt = GetFrameTime();
    introTimer += dt;

    float SW = (float)GetScreenWidth();
    float SH = (float)GetScreenHeight();

    // --- ustal faze ---
    if (introTimer < T_P1) introPhase = 0;
    else if (introTimer < T_P2) introPhase = 1;
    else if (introTimer < T_P3) introPhase = 2;
    else if (introTimer < T_P4) introPhase = 3;
    else if (introTimer < T_P5) introPhase = 4;
    else if (introTimer < T_P6) introPhase = 5;
    else if (introTimer < T_P7) introPhase = 6;
    else                        introPhase = 7;

    // -------------------------------------------------------
    //  PHASE 0: fade-in tla
    // -------------------------------------------------------
    bgAlpha = Clamp(introTimer / T_P1, 0.0f, 1.0f);

    // -------------------------------------------------------
    //  PHASE 1: dzialo wjezdza plynnie z lewej
    //  Zatrzymuje sie tak, ze lufa wynosi ok 30% szerokosci ekranu
    // -------------------------------------------------------
    if (introPhase >= 1)
    {
        // Sprite armaty to sprite sheet 2 klatki obok siebie (left = armata z boku)
        float cannonScale = 1.5f;
        float halfW = (float)(texCannon.width / 2) * cannonScale;

        // target: lewa krawedz sprite'a armaty na x = -halfW*0.1
        // czyli lufa (prawy koniec) mniej-wiecej na 28% ekranu
        float targetX = SW * 0.00f - halfW * 0.05f;
        float t = Clamp((introTimer - T_P1) / (T_P2 - T_P1), 0.0f, 1.0f);
        float ease = EaseOut3(t);
        cannonX = -halfW + ease * (targetX + halfW);
    }

    // -------------------------------------------------------
    //  PHASE 2: STRZAL - blysk + kapsulka
    // -------------------------------------------------------
    if (introPhase == 2)
    {
        // blysk na poczatku fazy
        float tFlash = Clamp((introTimer - T_P2) / SHOT_FLASH_DUR, 0.0f, 1.0f);
        boomAlpha = 1.0f - tFlash;      // zanika szybko
        boomScale = 1.2f;
        flashAlpha = 1.0f - tFlash;

        // shake ekranu
        shakeStrength = 10.0f * (1.0f - tFlash);
        shakeTime = SHOT_FLASH_DUR;

        // kapsulka pojawia sie natychmiast i leci
        bulletVisible = true;
        float tBullet = Clamp((introTimer - T_P2) / BULLET_TRAVEL, 0.0f, 1.0f);
        bulletT = tBullet;

        float cannonScale = 1.5f;
        float halfW = (float)(texCannon.width / 2) * cannonScale;
        float startBulletX = cannonX + halfW * 0.92f;   // z konca lufy
        bulletX = startBulletX + tBullet * (SW + 200.0f);   // leci az za ekran
    }
    else if (introPhase < 2)
    {
        bulletVisible = false;
        boomAlpha = 0.0f;
    }
    else
    {
        // po fazie 2 kapsulka juz poza ekranem - ukryta
        bulletVisible = false;
        boomAlpha = 0.0f;
    }

    // -------------------------------------------------------
    //  PHASE 3: lusksi + exploding pixels
    // -------------------------------------------------------
    if (introPhase >= 3 && !bulletsSpawned)
    {
        bulletsSpawned = true;

        // Sprite buletsow to sheet z 4 kolumnami x 3 rzedami = 12 klatek roznych lusek
        // Spawnujemy 8 losowych
        for (int i = 0; i < 8; i++)
        {
            Bullet2D b;
            // spawn z okolic konca lufy
            float cannonScale = 1.5f;
            float halfW = (float)(texCannon.width / 2) * cannonScale;
            b.x = cannonX + halfW * 0.85f + (float)(rand() % 40) - 20.0f;
            b.y = SH * 0.52f + (float)(rand() % 20) - 10.0f;
            // lecq w prawo i w gore, potem grawitacja
            b.vx = (float)(rand() % 300) + 150.0f;
            b.vy = -(float)(rand() % 300) - 100.0f;
            b.alpha = 1.0f;
            // losowa klatka ze sprite sheeta (4 kolumny x 3 rzedy)
            b.rot = 0.0f;
            b.rotSpeed = ((float)(rand() % 600) - 300.0f);
            flyingBullets.push_back(b);
        }
    }

    if (introPhase >= 3 && !pixelsSpawned)
    {
        pixelsSpawned = true;

        // Exploding pixels - losowe punkty wokol konca lufy
        for (int i = 0; i < 40; i++)
        {
            ExPixel p;
            float cannonScale = 1.5f;
            float halfW = (float)(texCannon.width / 2) * cannonScale;
            p.x = cannonX + halfW * 0.9f + (float)(rand() % 30) - 15.0f;
            p.y = SH * 0.52f + (float)(rand() % 20) - 10.0f;
            p.vx = (float)(rand() % 500) - 100.0f;
            p.vy = (float)(rand() % 400) - 350.0f;
            float r = (float)(rand() % 55) + 200;
            float g = (float)(rand() % 100) + 100;
            p.col = { (unsigned char)r, (unsigned char)g, 30, 255 };
            p.life = 1.0f;
            explodingPixels.push_back(p);
        }
    }

    // Update lusek
    for (auto& b : flyingBullets)
    {
        b.x += b.vx * dt;
        b.y += b.vy * dt;
        b.vy += 600.0f * dt;       // grawitacja
        b.rot += b.rotSpeed * dt;
        b.alpha = Clamp(b.alpha - dt * 1.2f, 0.0f, 1.0f);
    }

    // Update exploding pixels
    for (auto& p : explodingPixels)
    {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.vy += 700.0f * dt;
        p.life = Clamp(p.life - dt * 1.5f, 0.0f, 1.0f);
        p.col.a = (unsigned char)(p.life * 255.0f);
    }

    // -------------------------------------------------------
    //  PHASE 4: crossfade bg red -> blue, dzialo kontynuuje wjazd
    //  (w oryginale kamera troche odsuwa sie, tu symulujemy
    //   lekkim dalszym ruchem armaty w lewo poza ekran)
    // -------------------------------------------------------

    // -------------------------------------------------------
    //  PHASE 5 & 6: METAL wjezdza z lewej, SLUG z prawej
    //  Zatrzymuja sie wycentrowane
    // -------------------------------------------------------
    if (introPhase >= 5)
    {
        float t = Clamp((introTimer - T_P5) / (T_P6 - T_P5), 0.0f, 1.0f);
        float ease = EaseOut3(t);

        float scaleM = 2.5f;
        float metalW = (float)texMetalBig.width * scaleM;
        float scaleS = 2.5f;
        float slugW = (float)texSlugTM.width * scaleS;

        // METAL: docelowo lewostronnie wycentrowany
        float metalTargetX = SW * 0.5f - metalW * 0.5f - SW * 0.04f;
        metalX = -metalW + ease * (metalTargetX + metalW);

        // SLUG: docelowo prawostronnie wycentrowany
        float slugTargetX = SW * 0.5f - slugW * 0.5f + SW * 0.04f;
        slugX = SW + ease * (slugTargetX - SW);
    }

    // -------------------------------------------------------
    //  SHAKE UPDATE
    // -------------------------------------------------------
    if (shakeTime > 0.0f)
        shakeTime -= dt;

    // -------------------------------------------------------
    //  FLASH UPDATE
    // -------------------------------------------------------
    flashAlpha = Clamp(flashAlpha - dt * 4.0f, 0.0f, 1.0f);
}

// ============================================================
//  DRAW INTRO  (wewnetrzna, wywolywana z DrawTexts)
// ============================================================
static void DrawTextureFill(Texture2D tex, float x, float y, float w, float h, Color tint)
{
    Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
    Rectangle dst = { x, y, w, h };
    DrawTexturePro(tex, src, dst, { 0, 0 }, 0.0f, tint);
}

void SceneManager::DrawIntro()
{
    int   SW = GetScreenWidth();
    int   SH = GetScreenHeight();
    int   border = 110;

    // Obszar "ekranu gry" wewnatrz czarnej ramki
    float sX = 0.0f;                        // brak bocznych ramek
    float sY = (float)border;
    float sW = (float)SW;
    float sH = (float)(SH - border * 2);

    // --- shake offset ---
    float ox = 0.0f, oy = 0.0f;
    if (shakeTime > 0.0f)
    {
        ox = (float)((rand() % 3) - 1) * shakeStrength;
        oy = (float)((rand() % 3) - 1) * shakeStrength;
    }

    // ===========================
    //  TLO
    // ===========================
    // Phase 0-3: czerwone tlo (fade in)
    // Phase 4+:  crossfade do niebieskiego
    {
        float scaleRW = sW / (float)texRedBg.width;
        float scaleRH = sH / (float)texRedBg.height;
        float sR = (scaleRW > scaleRH) ? scaleRW : scaleRH;

        unsigned char redA = (unsigned char)(bgAlpha * 255.0f);
        DrawTextureFill(texRedBg, sX + ox, sY + oy,
            (float)texRedBg.width * sR, (float)texRedBg.height * sR,
            { 255, 255, 255, redA });

        if (introPhase >= 4)
        {
            float t = Clamp((introTimer - T_P4) / (T_P5 - T_P4), 0.0f, 1.0f);
            float scaleBW = sW / (float)texBlueBg.width;
            float scaleBH = sH / (float)texBlueBg.height;
            float sB = (scaleBW > scaleBH) ? scaleBW : scaleBH;
            unsigned char blueA = (unsigned char)(t * 255.0f);
            DrawTextureFill(texBlueBg, sX + ox, sY + oy,
                (float)texBlueBg.width * sB, (float)texBlueBg.height * sB,
                { 255, 255, 255, blueA });
        }
    }

    // ===========================
    //  DRZEWA (dolna czesc sceny)
    // ===========================
    if (introPhase >= 1)
    {
        float treeScale = sW / (float)texTrees.width;
        float treeY = sY + sH - (float)texTrees.height * treeScale;
        DrawTextureEx(texTrees, { sX + ox, treeY + oy }, 0.0f, treeScale, WHITE);
    }

    // ===========================
    //  TANK + DZIALO (zlozoone ze sprite sheeta)
    //
    //  newintrotankshit.png (1470x800) zawiera:
    //    Body czolgu:  x=209, y=365, w=151, h=113
    //    Gasienice:    4 klatki animacji, kazda ~133x110
    //      klatka 0: x=0,   y=528, w=133, h=110
    //      klatka 1: x=211, y=528, w=131, h=110
    //      klatka 2: x=421, y=528, w=136, h=110
    //      klatka 3: x=631, y=528, w=138, h=110
    //
    //  newintrocannon.png = sprite sheet 2 klatki obok siebie
    //    lewa polowa = armata z boku (ta ktorej uzywamy)
    // ===========================

    // Animacja gasienica - 8 fps
    trackAnim += GetFrameTime();
    int trackFrame = (int)(trackAnim * 8.0f) % 4;

    // Wspolrzedne klatkek gasienica w sprite sheecie
    struct TrackFrame { int x, y, w, h; };
    static const TrackFrame TRACKS[4] = {
        {  0,  528, 133, 110 },
        { 211, 528, 131, 110 },
        { 421, 528, 136, 110 },
        { 631, 528, 138, 110 },
    };
    // Body czolgu w sprite sheecie
    static const int BODY_X = 209, BODY_Y = 365, BODY_W = 151, BODY_H = 113;

    // Skala: dopasuj wysokosc body do ~22% wysokosci sceny
    float tankScale = (sH * 0.22f) / (float)BODY_H;

    // Pozycja bazowa czolgu (lewy dolny rog body)
    // Czolg stoi na "ziemi" - dolna krawedz gasienica = ok 78% sceny
    float groundY = sY + sH * 0.78f;   // linia ziemi

    float bodyW = BODY_W * tankScale;
    float bodyH = BODY_H * tankScale;

    const TrackFrame& tf = TRACKS[trackFrame];
    float trackW = tf.w * tankScale;
    float trackH = tf.h * tankScale;

    // Gasienice: ich dol = linia ziemi
    float trackDrawY = groundY - trackH;
    // Body: naklada sie NA gasienice - dol body ~40% w dol gasienica
    // (z podgladu: body_y = trackY - 80px przy skali 1.0, tu skalujemy)
    // offset = trackH * 0.72 (body zachodzi 28% gasienica od gory)
    float bodyDrawY = trackDrawY - bodyH + trackH * 0.45f;

    // X: gasienice
    float tankBaseX = sX + sW * 0.01f + ox;
    // Body lekko przesuniete w prawo wzgledem gasienica (karoseria szersza)
    float bodyDrawX = tankBaseX - bodyW * 0.06f;

    if (introPhase >= 1)
    {
        // --- Gasienice (animowane) - rysowane PIERWSZE (pod body) ---
        Rectangle trackSrc = {
            (float)tf.x, (float)tf.y,
            (float)tf.w, (float)tf.h
        };
        Rectangle trackDst = {
            tankBaseX,
            trackDrawY + oy,
            trackW,
            trackH
        };
        DrawTexturePro(texTankShit, trackSrc, trackDst, { 0, 0 }, 0.0f, WHITE);

        // --- Body czolgu - rysowane NA gasienicach ---
        Rectangle bodySrc = {
            (float)BODY_X, (float)BODY_Y,
            (float)BODY_W, (float)BODY_H
        };
        Rectangle bodyDst = {
            bodyDrawX,
            bodyDrawY + oy,
            bodyW,
            bodyH
        };
        DrawTexturePro(texTankShit, bodySrc, bodyDst, { 0, 0 }, 0.0f, WHITE);
    }

    // ===========================
    //  DZIALO (armata na szczycie body)
    //  Sprite sheet newintrocannon.png: lewa polowa = armata z boku
    // ===========================
    float cannonScale = 1.5f;
    float cannonHalfW = (float)(texCannon.width / 2);
    float cannonH = (float)texCannon.height;

    // Armata: srodek pionowy armaty = szczyt body czolgu (z lekkim zejsciem)
    float cannonY = bodyDrawY + bodyH * 0.10f - cannonH * cannonScale * 0.5f;

    if (introPhase >= 1)
    {
        Rectangle src = { 0.0f, 0.0f, cannonHalfW, cannonH };
        Rectangle dst = {
            sX + cannonX + ox,
            cannonY + oy,
            cannonHalfW * cannonScale,
            cannonH * cannonScale
        };
        DrawTexturePro(texCannon, src, dst, { 0, 0 }, 0.0f, WHITE);
    }

    // ===========================
    //  KAPSULKA (pocisk)
    // ===========================
    if (bulletVisible && introPhase == 2)
    {
        float capsScale = 1.8f;
        float capsHalfW = (float)(texCapsuleCannon.width / 2);
        float capsH = (float)texCapsuleCannon.height;
        float capsY = cannonY + cannonH * cannonScale * 0.5f - capsH * capsScale * 0.5f;

        Rectangle src = { 0.0f, 0.0f, capsHalfW, capsH };
        Rectangle dst = {
            sX + bulletX + ox,
            capsY + oy,
            capsHalfW * capsScale,
            capsH * capsScale
        };
        DrawTexturePro(texCapsuleCannon, src, dst, { 0, 0 }, 0.0f, WHITE);
    }

    // ===========================
    //  BLYSK STRZALU (BOOM + CANNON EXPLOSION)
    // ===========================
    if (boomAlpha > 0.01f && introPhase == 2)
    {
        float tipX = sX + cannonX + cannonHalfW * cannonScale * 0.90f;
        float tipY = cannonY + cannonH * cannonScale * 0.50f;

        // boomSprite (duzy flash okragly)
        float boomSc = 1.2f;
        DrawTextureEx(texBoom,
            {
                tipX - (float)texBoom.width * boomSc * 0.5f + ox,
                tipY - (float)texBoom.height * boomSc * 0.5f + oy
            },
            0.0f, boomSc,
            { 255, 255, 255, (unsigned char)(boomAlpha * 220.0f) });

        // cannonExplosion (ogien z lufy)
        float expSc = 1.0f;
        DrawTextureEx(texCannonExplosion,
            {
                tipX - (float)texCannonExplosion.width * expSc * 0.5f + ox,
                tipY - (float)texCannonExplosion.height * expSc * 0.5f + oy
            },
            0.0f, expSc,
            { 255, 255, 255, (unsigned char)(boomAlpha * 255.0f) });
    }

    // ===========================
    //  EXPLODING PIXELS (iskry ze strzalu)
    // ===========================
    for (const auto& p : explodingPixels)
    {
        if (p.life <= 0.01f) continue;
        DrawPixel((int)(sX + p.x + ox), (int)(p.y + oy), p.col);
        DrawPixel((int)(sX + p.x + ox + 1), (int)(p.y + oy), p.col);
        DrawPixel((int)(sX + p.x + ox), (int)(p.y + oy + 1), p.col);
    }

    // ===========================
    //  LUSKSI (latajace)
    // ===========================
    if (!flyingBullets.empty())
    {
        // Sprite sheet lusek: 4 kolumny, 3 rzedy = 12 klatek
        int cols = 4, rows = 3;
        float bW = (float)texBullets.width / cols;
        float bH = (float)texBullets.height / rows;
        float bSc = 1.5f;

        for (const auto& b : flyingBullets)
        {
            if (b.alpha <= 0.01f) continue;
            int frame = (int)(b.rot / 60.0f) % 12;
            if (frame < 0) frame += 12;
            int col = frame % cols;
            int row = frame / cols;

            Rectangle src = { col * bW, row * bH, bW, bH };
            Rectangle dst = {
                sX + b.x + ox,
                b.y + oy,
                bW * bSc,
                bH * bSc
            };
            DrawTexturePro(texBullets, src, dst, { bW * bSc * 0.5f, bH * bSc * 0.5f },
                b.rot, { 255, 255, 255, (unsigned char)(b.alpha * 255.0f) });
        }
    }

    // ===========================
    //  LOGO METAL SLUG
    //  METAL wjezdza z lewej, SLUG z prawej
    //  Nad spodem: texLogoTop ("SUPER VEHICLE-001" / naglowek)
    // ===========================
    if (introPhase >= 5)
    {
        float scaleM = 2.5f;
        float scaleS = 2.5f;
        float metalW = (float)texMetalBig.width * scaleM;
        float metalH = (float)texMetalBig.height * scaleM;
        float slugW = (float)texSlugTM.width * scaleS;
        float slugH = (float)texSlugTM.height * scaleS;

        // Pionowe centrum logo - okolo 50% ekranu
        float totalLogoH = metalH + slugH + 6.0f;
        float logoStartY = (float)SH * 0.46f - totalLogoH * 0.5f;

        // METAL
        DrawTextureEx(texMetalBig, { metalX + ox, logoStartY + oy }, 0.0f, scaleM, WHITE);

        // SLUG TM
        DrawTextureEx(texSlugTM,
            { slugX + ox, logoStartY + metalH + 6.0f + oy },
            0.0f, scaleS, WHITE);

        // Naglowek "SUPER VEHICLE-001" (texLogoTop) - USUNIETO (maly napis)
    }

    // ===========================
    //  BIALY FLASH po strzale
    // ===========================
    if (flashAlpha > 0.01f)
    {
        DrawRectangle(0, 0, SW, SH,
            { 255, 255, 255, (unsigned char)(flashAlpha * 180.0f) });
    }

    // ===========================
    //  CZARNA RAMKA - tylko gora i dol (letterbox)
    // ===========================
    DrawRectangle(0, 0, SW, border, BLACK);
    DrawRectangle(0, SH - border, SW, border, BLACK);

    // ===========================
    //  PRESS ENTER (phase 7)
    // ===========================
    if (introPhase >= 7)
    {
        if ((int)(introTimer * 2.5f) % 2 == 0)
        {
            const char* txt = "PRESS ENTER";
            int   fs = 24;
            int   tw = MeasureText(txt, fs);
            int   tx = SW / 2 - tw / 2;
            int   ty = (int)((float)SH * 0.87f);
            DrawText(txt, tx + 2, ty + 2, fs, BLACK);
            DrawText(txt, tx, ty, fs, YELLOW);
        }
    }
}

// ============================================================
//  DrawTexts  -  glowny punkt wejscia wywo³ywany z zewnatrz
// ============================================================
void SceneManager::DrawTexts()
{
    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    // =========================================================
    //  TITLE
    // =========================================================
    if (currentState == TITLE)
    {
        ClearBackground(BLACK);

        float scaleW = (float)SW / texBrrrt.width;
        float scaleH = (float)SH / texBrrrt.height;
        float scale = (scaleW > scaleH) ? scaleW : scaleH;
        DrawTextureEx(texBrrrt, { 0, 0 }, 0.0f, scale, WHITE);

        // impact flash
        float impactScale = 3.0f;
        int   halfW = texExplo2sprites.width / 2;
        Rectangle src = { (float)halfW, 0.0f, (float)halfW, (float)texExplo2sprites.height };
        Rectangle dst = {
            SW * 0.5f - halfW * impactScale * 0.5f,
            SH * 0.42f - texExplo2sprites.height * impactScale * 0.5f,
            (float)halfW * impactScale,
            (float)texExplo2sprites.height * impactScale
        };
        DrawTexturePro(texExplo2sprites, src, dst, { 0, 0 }, 0.0f, { 255,255,255,200 });

        // logo
        float scaleM = 2.5f, scaleS = 2.5f;
        float totalH = texMetalBig.height * scaleM + texSlugTM.height * scaleS + 8.0f;
        float startY = SH * 0.45f - totalH * 0.5f;
        float mX = SW * 0.5f - texMetalBig.width * scaleM * 0.5f;
        float sX2 = SW * 0.5f - texSlugTM.width * scaleS * 0.5f;
        DrawTextureEx(texMetalBig, { mX,  startY }, 0.0f, scaleM, WHITE);
        DrawTextureEx(texSlugTM, { sX2, startY + texMetalBig.height * scaleM + 8.0f },
            0.0f, scaleS, WHITE);

        // PUSH ENTER
        const char* txt = "PUSH ENTER TO START!";
        if ((int)(GetTime() * 2.5f) % 2 == 0)
        {
            int fs = 28, tw = MeasureText(txt, fs);
            int tx = SW / 2 - tw / 2, ty = (int)(SH * 0.82f);
            DrawText(txt, tx + 2, ty + 2, fs, BLACK);
            DrawText(txt, tx, ty, fs, YELLOW);
        }

        // stopka
        const char* footer = "2026 KURVVA PRODUCTIONS";
        int fSize = 20, fw = MeasureText(footer, fSize);
        DrawText(footer, SW / 2 - fw / 2 + 2, (int)(SH * 0.92f) + 2, fSize, BLACK);
        DrawText(footer, SW / 2 - fw / 2, (int)(SH * 0.92f), fSize, WHITE);
        return;
    }

    // =========================================================
    //  GAME
    // =========================================================
    if (currentState == GAME)
    {
        ClearBackground(BLACK);
        return;
    }

    // =========================================================
    //  INTRO
    // =========================================================
    ClearBackground(BLACK);
    UpdateIntro();
    DrawIntro();
}