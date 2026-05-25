#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <future>
#include "UiManager.hpp"

class SceneManager {
public:
    enum Gamestates { INTRO, TITLE, HOWTOPLAY, GAME, CONTINUE_SCREEN, GAME_OVER };

    SceneManager();
    ~SceneManager();
    void Init();

    void       DrawTexts();
    void       UpdateIntro();
    Gamestates GetGamestate();
    void       SetGameState(Gamestates gamestate);
    int        GetHtpTotalFrames() const { return htpTotalFrames; }

    Gamestates currentState;
    UiManager* ui = nullptr;
    void SetUiManager(UiManager* u);
    void FullReset();
    bool IsSplashActive() const { return splashActive; }

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

    // 3-frame async preload buffer:
    //   preloadFutures[0] = N+1  (started 1 frame ago,  ~16 ms to decode)
    //   preloadFutures[1] = N+2  (started 2 frames ago, ~32 ms to decode)
    //   preloadFutures[2] = N+3  (started 3 frames ago, ~48 ms to decode)
    // On advance N→N+1: get [0], rotate [1]→[0] [2]→[1], start new [2]=N+4
    static constexpr int   PRELOAD_AHEAD      = 3;
    static constexpr float INTRO_PLAYBACK_FPS = 54.0f;
    static constexpr float HTP_PLAYBACK_FPS   = 58.0f;

    std::future<Image> preloadFutures[PRELOAD_AHEAD];

    void ResetIntro();
    void DrawIntro() const;
    void IntroStartPreload(int slot, int frameIdx);
    void IntroAdvanceFrame();

    // ── HOWTOPLAY: sliding-window player ─────────────────────
    int       htpTotalFrames = 0;
    int       htpFrameIdx    = 0;
    float     htpTimer       = 0.0f;

    Image     htpImgCur  = {};
    Texture2D htpTex     = {};

    std::future<Image> htpFutures[PRELOAD_AHEAD];

    void ResetHowtoplay();
    void DrawHowtoplay() const;
    void HtpStartPreload(int slot, int frameIdx);
    void HtpAdvanceFrame();
    void UpdateHowtoplay();
    
    //intro
    bool splashActive = true;
    float splashTimer = 0.0f;
    static constexpr float SPLASH_DURATION = 5.0f;
    Texture2D texSplash = { 0 };
};
