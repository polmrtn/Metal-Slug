#include "SceneManager.hpp"
#include "raymath.h"
#include <cstdio>
#include <algorithm>
#include <cstring>

// ============================================================
//  Frame file path helper
//  Naming: frame_000001.jpg ... frame_001074.jpg  (6 digits, from 1)
// ============================================================
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
    memset(&preloadImg, 0, sizeof(preloadImg));
}

void SceneManager::Init()
{
    // ── Title screen assets ───────────────────────────────────
    texBrrrt         = LoadTexture("Graphics/intro/brrrt.png");
    texExplo2sprites = LoadTexture("Graphics/intro/newintro2explosprites.png");
    texMetalBig      = LoadTexture("Graphics/intro/NEWintroMETALSLUG1.png");
    texSlugTM        = LoadTexture("Graphics/intro/NEWINTROmetalslugTM.png");

    // ── Count how many intro frames exist ────────────────────
    char path[512];
    totalIntroFrames = 0;
    for (;;)
    {
        IntroFramePath(path, sizeof(path), totalIntroFrames);
        if (!FileExists(path)) break;
        totalIntroFrames++;
        if (totalIntroFrames > 99999) break; // safety cap
    }

    // Start with the buffer filled
    ResetIntro();
}

SceneManager::~SceneManager()
{
    UnloadTexture(texBrrrt);
    UnloadTexture(texExplo2sprites);
    UnloadTexture(texMetalBig);
    UnloadTexture(texSlugTM);

    for (auto& s : frameBuffer) UnloadTexture(s.tex);
    frameBuffer.clear();

    if (preloadImgReady && preloadImg.data)
        UnloadImage(preloadImg);
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

    // Unload any buffered frames
    for (auto& s : frameBuffer) UnloadTexture(s.tex);
    frameBuffer.clear();

    if (preloadImgReady && preloadImg.data)
        UnloadImage(preloadImg);
    memset(&preloadImg, 0, sizeof(preloadImg));
    preloadImgIdx   = -1;
    preloadImgReady = false;

    // Pre-fill the buffer with first INTRO_BUF frames
    for (int i = 0; i < INTRO_BUF && i < totalIntroFrames; i++)
    {
        char path[512];
        IntroFramePath(path, sizeof(path), i);
        FrameSlot s;
        s.idx = i;
        s.tex = LoadTexture(path);
        frameBuffer.push_back(s);
    }
}

// ============================================================
//  IntroLoadNextIntoBuffer
//  Loads one more frame into the GPU ring-buffer.
//  Uses a CPU-side pre-decoded Image if available.
// ============================================================
void SceneManager::IntroLoadNextIntoBuffer()
{
    // Which index should we load next?
    int nextIdx = frameBuffer.empty()
                    ? introFrameIdx
                    : frameBuffer.back().idx + 1;

    if (nextIdx >= totalIntroFrames) return;

    FrameSlot s;
    s.idx = nextIdx;

    if (preloadImgReady && preloadImgIdx == nextIdx)
    {
        // The CPU image was already decoded last frame — just upload
        s.tex = LoadTextureFromImage(preloadImg);
        UnloadImage(preloadImg);
        memset(&preloadImg, 0, sizeof(preloadImg));
        preloadImgIdx   = -1;
        preloadImgReady = false;
    }
    else
    {
        // Synchronous load (fallback / first frame)
        char path[512];
        IntroFramePath(path, sizeof(path), nextIdx);
        s.tex = LoadTexture(path);
    }

    frameBuffer.push_back(s);
}

// ============================================================
//  IntroPurgeOldFrames
//  Remove frames from the front that are no longer needed.
// ============================================================
void SceneManager::IntroPurgeOldFrames()
{
    while (!frameBuffer.empty() &&
           frameBuffer.front().idx < introFrameIdx - 1)
    {
        UnloadTexture(frameBuffer.front().tex);
        frameBuffer.pop_front();
    }
}

// ============================================================
//  IntroGetCurrentTex
// ============================================================
const Texture2D* SceneManager::IntroGetCurrentTex() const
{
    for (const auto& s : frameBuffer)
        if (s.idx == introFrameIdx) return &s.tex;
    return nullptr;
}

// ============================================================
//  UpdateIntro
// ============================================================
void SceneManager::UpdateIntro()
{
    if (totalIntroFrames == 0)
    {
        SetGameState(TITLE);
        return;
    }

    float dt = GetFrameTime();
    introTimer      += dt;
    introFrameTimer += dt;

    // ── Pre-decode next frame on CPU (so GPU upload is nearly free) ──
    // We pre-decode one frame AHEAD of the buffer's tail.
    int wantPreload = frameBuffer.empty()
                        ? introFrameIdx + 1
                        : frameBuffer.back().idx + 1;

    if (!preloadImgReady && wantPreload < totalIntroFrames)
    {
        char path[512];
        IntroFramePath(path, sizeof(path), wantPreload);
        if (FileExists(path))
        {
            preloadImg      = LoadImage(path);   // JPEG decode on CPU
            preloadImgIdx   = wantPreload;
            preloadImgReady = true;
        }
    }

    // ── Maintain buffer: keep INTRO_BUF frames ahead ─────────
    while ((int)frameBuffer.size() < INTRO_BUF)
        IntroLoadNextIntoBuffer();

    IntroPurgeOldFrames();

    // ── Advance frame index ───────────────────────────────────
    const float frameDur = 1.0f / INTRO_PLAYBACK_FPS;
    if (introFrameTimer >= frameDur)
    {
        introFrameTimer -= frameDur;
        introFrameIdx++;

        if (introFrameIdx >= totalIntroFrames)
        {
            SetGameState(TITLE);
            return;
        }
    }

    // ── Skip with Enter / Space ───────────────────────────────
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        SetGameState(TITLE);
}

// ============================================================
//  DrawIntro
// ============================================================
void SceneManager::DrawIntro() const
{
    const Texture2D* tex = IntroGetCurrentTex();
    if (!tex || tex->width == 0) return;

    int   SW = GetScreenWidth();
    int   SH = GetScreenHeight();
    float scaleX = (float)SW / (float)tex->width;
    float scaleY = (float)SH / (float)tex->height;
    float scale  = std::min(scaleX, scaleY);          // letterbox
    // float scale = std::max(scaleX, scaleY);         // fullscreen crop

    float drawW = tex->width  * scale;
    float drawH = tex->height * scale;
    float drawX = ((float)SW - drawW) * 0.5f;
    float drawY = ((float)SH - drawH) * 0.5f;

    DrawTexturePro(*tex,
        { 0.0f, 0.0f, (float)tex->width, (float)tex->height },
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
