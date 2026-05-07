#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>
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

    // ── Intro frame player ────────────────────────────────────
    // All frames pre-decoded into CPU RAM (Image = raw pixels, no GPU cost).
    // Only ONE Texture2D lives on the GPU at any moment.
    std::vector<Image> introImages;   // all frames in RAM
    Texture2D          introTex = {}; // current frame on GPU
    int   introFrameIdx   = 0;
    float introFrameTimer = 0.0f;
    float introTimer      = 0.0f;

    static constexpr float INTRO_PLAYBACK_FPS = 60.0f;

    void ResetIntro();
    void DrawIntro() const;
};
