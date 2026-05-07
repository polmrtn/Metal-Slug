#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <deque>
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

    // ── Streaming frame player ────────────────────────────────
    // Frame files: Graphics/intro/frames/frames_60fps/frame_000001.jpg
    // (6-digit index, starts at 1)
    int   totalIntroFrames  = 0;   // total files found on disk
    int   introFrameIdx     = 0;   // currently displayed frame (0-based)
    float introFrameTimer   = 0.0f;
    float introTimer        = 0.0f;

    static constexpr float INTRO_PLAYBACK_FPS = 60.0f;

    // ── Ring-buffer: keep INTRO_BUF frames in GPU memory ─────
    // Front of deque = oldest loaded frame index
    static constexpr int INTRO_BUF = 6;   // frames kept in VRAM at once

    struct FrameSlot { int idx; Texture2D tex; };
    std::deque<FrameSlot> frameBuffer;

    // CPU-side preload: one Image decoded ahead of time
    Image preloadImg       = {};
    int   preloadImgIdx    = -1;
    bool  preloadImgReady  = false;

    void ResetIntro();
    void DrawIntro() const;
    void IntroLoadNextIntoBuffer();
    void IntroPurgeOldFrames();
    const Texture2D* IntroGetCurrentTex() const;
};
