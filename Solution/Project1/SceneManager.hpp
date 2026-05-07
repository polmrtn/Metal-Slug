#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <future>
#include "UiManager.hpp"

class SceneManager {
public:
    enum Gamestates { INTRO, TITLE, GAME, CONTINUE_SCREEN, GAME_OVER };

    SceneManager();
    ~SceneManager();
    void Init();

    void       DrawTexts();
    void       UpdateIntro();
    Gamestates GetGamestate();
    void       SetGameState(Gamestates gamestate);

    Gamestates currentState;
    UiManager* ui = nullptr;
    void SetUiManager(UiManager* u);

private:
    // ── TITLE screen textures ─────────────────────────────────
    Texture2D texBrrrt;
    Texture2D texExplo2sprites;
    Texture2D texMetalBig;
    Texture2D texSlugTM;

    // ── Intro: sliding-window player ──────────────────────────
    //  RAM holds at most 2 frames:
    //    imgCur  = currently displayed frame (pixels in CPU RAM)
    //    imgNext = next frame, pre-loaded while current is shown
    //  GPU holds 1 texture (introTex) = imgCur uploaded.
    //
    //  On advance (N → N+1):
    //    1. UnloadImage(imgCur)
    //    2. imgCur = imgNext            (no disk I/O, instant)
    //    3. UnloadTexture(introTex)
    //    4. introTex = LoadTextureFromImage(imgCur)  (~1 ms)
    //    5. Load imgNext for frame N+2  (~5-10 ms on SSD)
    // ─────────────────────────────────────────────────────────
    int       introTotalFrames = 0;
    int       introFrameIdx    = 0;
    float     introFrameTimer  = 0.0f;
    float     introTimer       = 0.0f;

    Image     imgCur  = {};        // current frame pixels (CPU)
    Texture2D introTex = {};       // current frame on GPU (one at a time)

    // Background preload: next frame decoded on a separate thread
    std::future<Image> nextFrameFuture;

    static constexpr float INTRO_PLAYBACK_FPS = 60.0f;

    void ResetIntro();
    void DrawIntro() const;
    void IntroStartPreload(int idx);   // kick off async decode of frame idx
    void IntroAdvanceFrame();          // swap cur←next, upload GPU, kick next preload
};
