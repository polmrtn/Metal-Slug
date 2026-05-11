#include "SceneManager.hpp"
#include "raymath.h"
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <future>
#include <chrono>

// Frame path — 1920x1080 PNG (lossless copy of original JPEGs, raylib loads PNG natively)
static void IntroFramePath(char* buf, int bufSize, int zeroBasedIdx)
{
    snprintf(buf, bufSize,
        "Graphics/intro/frames/frames_png/frame_%06d.png",
        zeroBasedIdx + 1);
}

// ============================================================
SceneManager::SceneManager()
{
    currentState = INTRO;
    memset(&introTex, 0, sizeof(introTex));
    memset(&imgCur,   0, sizeof(imgCur));
}

void SceneManager::Init()
{
    // Title screen textures
    texBrrrt         = LoadTexture("Graphics/intro/brrrt.png");
    texExplo2sprites = LoadTexture("Graphics/intro/newintro2explosprites.png");
    texMetalBig      = LoadTexture("Graphics/intro/NEWintroMETALSLUG1.png");
    texSlugTM        = LoadTexture("Graphics/intro/NEWINTROmetalslugTM.png");

    // Count total intro frames
    char path[512];
    introTotalFrames = 0;
    for (;;)
    {
        IntroFramePath(path, sizeof(path), introTotalFrames);
        if (!FileExists(path)) break;
        introTotalFrames++;
        if (introTotalFrames > 99999) break;
    }

    ResetIntro();

    // Count total howtoplay frames
    htpTotalFrames = 0;
    for (;;)
    {
        char htpPath[512];
        snprintf(htpPath, sizeof(htpPath),
            "Graphics/howtoplay_png/frame_%06d.png", htpTotalFrames + 1);
        if (!FileExists(htpPath)) break;
        htpTotalFrames++;
        if (htpTotalFrames > 99999) break;
    }
}

SceneManager::~SceneManager()
{
    UnloadTexture(texBrrrt);
    UnloadTexture(texExplo2sprites);
    UnloadTexture(texMetalBig);
    UnloadTexture(texSlugTM);

    for (int i = 0; i < PRELOAD_AHEAD; i++)
        if (preloadFutures[i].valid()) preloadFutures[i].wait();
    if (introTex.id)  UnloadTexture(introTex);
    if (imgCur.data)  UnloadImage(imgCur);

    for (int i = 0; i < PRELOAD_AHEAD; i++)
        if (htpFutures[i].valid()) htpFutures[i].wait();
    if (htpTex.id)    UnloadTexture(htpTex);
    if (htpImgCur.data) UnloadImage(htpImgCur);
}

void SceneManager::SetUiManager(UiManager* u) { ui = u; }

// ============================================================
SceneManager::Gamestates SceneManager::GetGamestate() { return currentState; }

void SceneManager::SetGameState(Gamestates gs)
{
    currentState = gs;
    if (gs == INTRO)     ResetIntro();
    if (gs == HOWTOPLAY) ResetHowtoplay();
}

// ============================================================
//  ResetIntro — load frame 0 to GPU, pre-load frame 1 to RAM
// ============================================================
void SceneManager::ResetIntro()
{
    introTimer      = 0.0f;
    introFrameTimer = 0.0f;
    introFrameIdx   = 0;

    // Free old data
    // Cancel all running preloads
    for (int i = 0; i < PRELOAD_AHEAD; i++)
        if (preloadFutures[i].valid()) preloadFutures[i].wait();

    if (introTex.id) { UnloadTexture(introTex); memset(&introTex, 0, sizeof(introTex)); }
    if (imgCur.data) { UnloadImage(imgCur);      memset(&imgCur,   0, sizeof(imgCur));  }

    if (introTotalFrames == 0) return;

    // Load frame 0 synchronously → GPU
    char path[512];
    IntroFramePath(path, sizeof(path), 0);
    imgCur   = LoadImage(path);
    introTex = LoadTextureFromImage(imgCur);
    SetTextureFilter(introTex, TEXTURE_FILTER_BILINEAR);

    // Kick off PRELOAD_AHEAD async decodes immediately
    for (int i = 0; i < PRELOAD_AHEAD; i++)
        IntroStartPreload(i, i + 1);
}

// ============================================================
//  IntroStartPreload — async decode of frameIdx into slot
// ============================================================
void SceneManager::IntroStartPreload(int slot, int frameIdx)
{
    if (frameIdx >= introTotalFrames) return;
    char path[512];
    IntroFramePath(path, sizeof(path), frameIdx);
    std::string pathStr(path);
    preloadFutures[slot] = std::async(std::launch::async,
        [pathStr]() -> Image { return LoadImage(pathStr.c_str()); });
}

// ============================================================
//  IntroAdvanceFrame
//  preloadFutures[0] = N+1  ← take this (has had PRELOAD_AHEAD frames to decode)
//  preloadFutures[1] = N+2  → becomes [0]
//  preloadFutures[2] = N+3  → becomes [1]
//  start new future for N+1+PRELOAD_AHEAD → slot [2]
// ============================================================
void SceneManager::IntroAdvanceFrame()
{
    introFrameIdx++;
    if (introFrameIdx >= introTotalFrames)
    {
        SetGameState(TITLE);
        return;
    }

    // Free old CPU frame
    if (imgCur.data) { UnloadImage(imgCur); memset(&imgCur, 0, sizeof(imgCur)); }

    // Get next frame from slot 0 (started PRELOAD_AHEAD frames ago — plenty of time)
    if (preloadFutures[0].valid())
        imgCur = preloadFutures[0].get();
    else
    {
        char path[512];
        IntroFramePath(path, sizeof(path), introFrameIdx);
        imgCur = LoadImage(path);
    }

    // Rotate buffer: [1]→[0], [2]→[1]
    for (int i = 0; i < PRELOAD_AHEAD - 1; i++)
        preloadFutures[i] = std::move(preloadFutures[i + 1]);

    // Start new preload for the furthest-ahead frame
    IntroStartPreload(PRELOAD_AHEAD - 1, introFrameIdx + PRELOAD_AHEAD);

    // GPU upload (~1 ms — pixels already in RAM)
    if (introTex.id) { UnloadTexture(introTex); memset(&introTex, 0, sizeof(introTex)); }
    introTex = LoadTextureFromImage(imgCur);
    SetTextureFilter(introTex, TEXTURE_FILTER_BILINEAR);
}

// ============================================================
//  UpdateIntro — time-based sync (frame index = elapsed time * FPS)
//  Video stays perfectly in sync with audio regardless of load time.
// ============================================================
void SceneManager::UpdateIntro()
{
    if (introTotalFrames == 0) { SetGameState(TITLE); return; }

    introTimer += GetFrameTime();

    // Which frame should we be showing right now?
    int targetFrame = (int)(introTimer * INTRO_PLAYBACK_FPS);
    if (targetFrame >= introTotalFrames)
    {
        SetGameState(TITLE);
        return;
    }

    // Advance frame(s) until we're caught up with real time
    while (introFrameIdx < targetFrame)
    {
        IntroAdvanceFrame();
        if (currentState != INTRO) return;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        SetGameState(TITLE);
}

// ============================================================
//  DrawIntro — fullscreen fill (no black bars)
// ============================================================
void SceneManager::DrawIntro() const
{
    if (!introTex.id || introTex.width == 0) return;

    int   SW = GetScreenWidth();
    int   SH = GetScreenHeight();
    float scaleX = (float)SW / (float)introTex.width;
    float scaleY = (float)SH / (float)introTex.height;
    float scale  = std::max(scaleX, scaleY);  // fill whole screen

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

        float impactScale = 4.2f;
        int   halfW = texExplo2sprites.width / 2;
        Rectangle src = { (float)halfW, 0.0f, (float)halfW, (float)texExplo2sprites.height };
        Rectangle dst = {
            SW * 0.5f - halfW * impactScale * 0.5f,
            SH * 0.48f - texExplo2sprites.height * impactScale * 0.5f,
            (float)halfW * impactScale,
            (float)texExplo2sprites.height * impactScale
        };
        DrawTexturePro(texExplo2sprites, src, dst, { 0.0f, 0.0f }, 0.0f, { 255, 255, 255, 200 });

        float scaleM = 3.7f, scaleS = 3.7f;
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

    if (currentState == HOWTOPLAY)
    {
        ClearBackground(BLACK);
        UpdateHowtoplay();
        DrawHowtoplay();
        return;
    }

    // INTRO
    ClearBackground(BLACK);
    UpdateIntro();
    DrawIntro();
}

// ============================================================
//  HOWTOPLAY — frame player (identyczny mechanizm co INTRO)
// ============================================================
void SceneManager::ResetHowtoplay()
{
    htpTimer     = 0.0f;
    htpFrameIdx  = 0;

    for (int i = 0; i < PRELOAD_AHEAD; i++)
        if (htpFutures[i].valid()) htpFutures[i].wait();

    if (htpTex.id)      { UnloadTexture(htpTex);   memset(&htpTex,    0, sizeof(htpTex));    }
    if (htpImgCur.data) { UnloadImage(htpImgCur);  memset(&htpImgCur, 0, sizeof(htpImgCur)); }

    if (htpTotalFrames == 0) return;

    char path[512];
    snprintf(path, sizeof(path), "Graphics/howtoplay_png/frame_%06d.png", 1);
    htpImgCur = LoadImage(path);
    htpTex    = LoadTextureFromImage(htpImgCur);
    SetTextureFilter(htpTex, TEXTURE_FILTER_BILINEAR);

    for (int i = 0; i < PRELOAD_AHEAD; i++)
        HtpStartPreload(i, i + 1);
}

void SceneManager::HtpStartPreload(int slot, int frameIdx)
{
    if (frameIdx >= htpTotalFrames) return;
    char path[512];
    snprintf(path, sizeof(path), "Graphics/howtoplay_png/frame_%06d.png", frameIdx + 1);
    std::string pathStr(path);
    htpFutures[slot] = std::async(std::launch::async,
        [pathStr]() -> Image { return LoadImage(pathStr.c_str()); });
}

void SceneManager::HtpAdvanceFrame()
{
    htpFrameIdx++;
    if (htpFrameIdx >= htpTotalFrames)
    {
        SetGameState(GAME);
        return;
    }

    if (htpImgCur.data) { UnloadImage(htpImgCur); memset(&htpImgCur, 0, sizeof(htpImgCur)); }

    if (htpFutures[0].valid())
        htpImgCur = htpFutures[0].get();
    else
    {
        char path[512];
        snprintf(path, sizeof(path), "Graphics/howtoplay_png/frame_%06d.png", htpFrameIdx + 1);
        htpImgCur = LoadImage(path);
    }

    for (int i = 0; i < PRELOAD_AHEAD - 1; i++)
        htpFutures[i] = std::move(htpFutures[i + 1]);

    HtpStartPreload(PRELOAD_AHEAD - 1, htpFrameIdx + PRELOAD_AHEAD);

    if (htpTex.id) { UnloadTexture(htpTex); memset(&htpTex, 0, sizeof(htpTex)); }
    htpTex = LoadTextureFromImage(htpImgCur);
    SetTextureFilter(htpTex, TEXTURE_FILTER_BILINEAR);
}

void SceneManager::UpdateHowtoplay()
{
    if (htpTotalFrames == 0) { SetGameState(GAME); return; }

    htpTimer += GetFrameTime();

    int targetFrame = (int)(htpTimer * HTP_PLAYBACK_FPS);
    if (targetFrame >= htpTotalFrames)
    {
        SetGameState(GAME);
        return;
    }

    while (htpFrameIdx < targetFrame)
    {
        HtpAdvanceFrame();
        if (currentState != HOWTOPLAY) return;
    }

    // Czekaj co najmniej 0.3s zanim Enter/Space bedzie dzialac
    // (zapobiega natychmiastowemu skipowi przez Enter z TITLE)
    if (htpTimer > 0.3f && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)))
        SetGameState(GAME);
}

void SceneManager::DrawHowtoplay() const
{
    if (!htpTex.id || htpTex.width == 0) return;

    int   SW = GetScreenWidth();
    int   SH = GetScreenHeight();
    float scaleX = (float)SW / (float)htpTex.width;
    float scaleY = (float)SH / (float)htpTex.height;
    float scale  = std::max(scaleX, scaleY);

    float drawW = htpTex.width  * scale;
    float drawH = htpTex.height * scale;
    float drawX = ((float)SW - drawW) * 0.5f;
    float drawY = ((float)SH - drawH) * 0.5f;

    DrawTexturePro(htpTex,
        { 0.0f, 0.0f, (float)htpTex.width, (float)htpTex.height },
        { drawX, drawY, drawW, drawH },
        { 0.0f, 0.0f }, 0.0f, WHITE);
}
