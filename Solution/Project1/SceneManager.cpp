#include "SceneManager.hpp"
#include "raymath.h"
#include <cstdio>
#include <algorithm>
#include <cstring>

// Frame file path: Graphics/intro/frames/frames_png/frame_000001.png
static void IntroFramePath(char* buf, int bufSize, int zeroBasedIdx)
{
    snprintf(buf, bufSize,
        "Graphics/intro/frames/frames_png/frame_%06d.png",
        zeroBasedIdx + 1);
}

// ============================================================
//  Constructor / Destructor
// ============================================================
SceneManager::SceneManager()
{
    currentState = INTRO;
    memset(&introTex, 0, sizeof(introTex));
}

void SceneManager::Init()
{
    // ── Title screen assets ───────────────────────────────────
    texBrrrt         = LoadTexture("Graphics/intro/brrrt.png");
    texExplo2sprites = LoadTexture("Graphics/intro/newintro2explosprites.png");
    texMetalBig      = LoadTexture("Graphics/intro/NEWintroMETALSLUG1.png");
    texSlugTM        = LoadTexture("Graphics/intro/NEWINTROmetalslugTM.png");

    // ── Pre-load ALL intro frames into CPU RAM ────────────────
    // Frames are 320×180 PNG — ~175 MB total, loads in ~1-2 s.
    char path[512];
    for (int i = 0; ; i++)
    {
        IntroFramePath(path, sizeof(path), i);
        if (!FileExists(path)) break;
        introImages.push_back(LoadImage(path));
        if (i > 99999) break; // safety
    }

    // Upload first frame to GPU immediately
    ResetIntro();
}

SceneManager::~SceneManager()
{
    UnloadTexture(texBrrrt);
    UnloadTexture(texExplo2sprites);
    UnloadTexture(texMetalBig);
    UnloadTexture(texSlugTM);

    if (introTex.id) UnloadTexture(introTex);
    for (auto& img : introImages) UnloadImage(img);
    introImages.clear();
}

void SceneManager::SetUiManager(UiManager* u) { ui = u; }

// ============================================================
//  State management
// ============================================================
SceneManager::Gamestates SceneManager::GetGamestate() { return currentState; }

void SceneManager::SetGameState(Gamestates gs)
{
    currentState = gs;
    if (gs == INTRO) ResetIntro();
}

void SceneManager::ResetIntro()
{
    introTimer      = 0.0f;
    introFrameTimer = 0.0f;
    introFrameIdx   = 0;

    // Upload frame 0 to GPU
    if (introTex.id) { UnloadTexture(introTex); memset(&introTex, 0, sizeof(introTex)); }
    if (!introImages.empty())
    {
        introTex = LoadTextureFromImage(introImages[0]);
        SetTextureFilter(introTex, TEXTURE_FILTER_BILINEAR);
    }
}

// ============================================================
//  UpdateIntro
// ============================================================
void SceneManager::UpdateIntro()
{
    if (introImages.empty())
    {
        SetGameState(TITLE);
        return;
    }

    float dt = GetFrameTime();
    introTimer      += dt;
    introFrameTimer += dt;

    // Advance to next frame?
    const float frameDur = 1.0f / INTRO_PLAYBACK_FPS;
    if (introFrameTimer >= frameDur)
    {
        introFrameTimer -= frameDur;
        introFrameIdx++;

        if (introFrameIdx >= (int)introImages.size())
        {
            SetGameState(TITLE);
            return;
        }

        // Swap GPU texture: unload old, upload new (fast — pixels already in RAM)
        if (introTex.id) UnloadTexture(introTex);
        introTex = LoadTextureFromImage(introImages[introFrameIdx]);
        SetTextureFilter(introTex, TEXTURE_FILTER_BILINEAR);
    }

    // Skip with Enter or Space
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        SetGameState(TITLE);
}

// ============================================================
//  DrawIntro  — fullscreen, no letterbox
// ============================================================
void SceneManager::DrawIntro() const
{
    if (!introTex.id || introTex.width == 0) return;

    int   SW = GetScreenWidth();
    int   SH = GetScreenHeight();

    // std::max  → fill whole screen (crops slightly if aspect differs)
    float scaleX = (float)SW / (float)introTex.width;
    float scaleY = (float)SH / (float)introTex.height;
    float scale  = std::max(scaleX, scaleY);   // ← fullscreen fill

    float drawW = introTex.width  * scale;
    float drawH = introTex.height * scale;
    float drawX = ((float)SW - drawW) * 0.5f;
    float drawY = ((float)SH - drawH) * 0.5f;

    DrawTexturePro(introTex,
        { 0.0f, 0.0f, (float)introTex.width, (float)introTex.height },
        { drawX, drawY, drawW, drawH },
        { 0.0f, 0.0f }, 0.0f, WHITE);
}

// ============================================================
//  DrawTexts  (main entry called by game loop)
// ============================================================
void SceneManager::DrawTexts()
{
    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    // ── TITLE ─────────────────────────────────────────────────
    if (currentState == TITLE)
    {
        ClearBackground(BLACK);

        float scaleW = (float)SW / texBrrrt.width;
        float scaleH = (float)SH / texBrrrt.height;
        DrawTextureEx(texBrrrt, { 0, 0 }, 0.0f,
                      std::max(scaleW, scaleH), WHITE);

        // Impact sprite (right half of sheet)
        float impactScale = 5.0f;
        int   halfW = texExplo2sprites.width / 2;
        Rectangle src = { (float)halfW, 0.0f,
                          (float)halfW, (float)texExplo2sprites.height };
        Rectangle dst = {
            SW * 0.5f - halfW * impactScale * 0.5f,
            SH * 0.48f - texExplo2sprites.height * impactScale * 0.5f,
            (float)halfW * impactScale,
            (float)texExplo2sprites.height * impactScale
        };
        DrawTexturePro(texExplo2sprites, src, dst,
                       { 0.0f, 0.0f }, 0.0f, { 255, 255, 255, 200 });

        // METAL / SLUG logo
        float scaleM = 3.2f, scaleS = 3.2f;
        float totalH = texMetalBig.height * scaleM
                     + texSlugTM.height   * scaleS + 10.0f;
        float startY = SH * 0.46f - totalH * 0.5f;
        DrawTextureEx(texMetalBig,
            { SW * 0.5f - texMetalBig.width * scaleM * 0.5f, startY },
            0.0f, scaleM, WHITE);
        DrawTextureEx(texSlugTM,
            { SW * 0.5f - texSlugTM.width * scaleS * 0.5f,
              startY + texMetalBig.height * scaleM + 10.0f },
            0.0f, scaleS, WHITE);

        // Blinking INSERT COIN
        const char* txt = "INSERT COINT!";
        if ((int)(GetTime() * 2.5f) % 2 == 0)
        {
            int fs = 28, tw = MeasureText(txt, fs);
            DrawText(txt, SW / 2 - tw / 2 + 2, (int)(SH * 0.82f) + 2, fs, BLACK);
            DrawText(txt, SW / 2 - tw / 2,     (int)(SH * 0.82f),     fs, YELLOW);
        }

        const char* footer = "2026 KURVVA PRODUCTIONS";
        int fSize = 20, fw = MeasureText(footer, fSize);
        DrawText(footer, SW / 2 - fw / 2 + 2, (int)(SH * 0.92f) + 2, fSize, BLACK);
        DrawText(footer, SW / 2 - fw / 2,     (int)(SH * 0.92f),     fSize, WHITE);

        if (ui) ui->DrawCreditsOnly();
        return;
    }

    if (currentState == GAME) { ClearBackground(BLACK); return; }

    // ── INTRO ─────────────────────────────────────────────────
    ClearBackground(BLACK);
    UpdateIntro();
    DrawIntro();
    if (ui) ui->DrawCreditsOnly();
}
