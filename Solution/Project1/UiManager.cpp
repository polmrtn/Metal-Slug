#include "UiManager.hpp"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>

static const float TICK_INTERVAL = 5.0f;

const float UiManager::INTRO_DURATION = 3.0f;
const float UiManager::BLINK_INTERVAL = 0.25f;

// �cie�ki do plik�w
static const char* PATH_ARMS = "Graphics/new fonts and HUDs/arms.png";
static const char* PATH_BOMB = "Graphics/new fonts and HUDs/hudbomb.png";
static const char* PATH_CANNON = "Graphics/new fonts and HUDs/hudcannon.png";
static const char* PATH_TIMELEVEL = "Graphics/new fonts and HUDs/TIMELEVEL.png";
static const char* PATH_FONT2BIG = "Graphics/new fonts and HUDs/hudfont2big.png";
static const char* PATH_FONT2NUM = "Graphics/new fonts and HUDs/hudfont2numbers.png";
static const char* PATH_FONT2SMALL = "Graphics/new fonts and HUDs/hudfont2small.png";
static const char* PATH_HIGHSCORE = "Graphics/new fonts and HUDs/highscorefont.png";
static const char* PATH_HIGHSCORE_SM = "Graphics/new fonts and HUDs/highscorefontsmall.png";
static const char* PATH_HPBAR_L = "Graphics/new fonts and HUDs/hpbarleft.png";
static const char* PATH_HPBAR_R = "Graphics/new fonts and HUDs/hpbarright.png";
static const char* PATH_HPBAR_P = "Graphics/new fonts and HUDs/hpbarparts.png";
static const char* PATH_GO = "Graphics/new fonts and HUDs/GO.png";
static const char* PATH_TIME_NUM      = "Graphics/letters/time_numbers.png";
static const char* PATH_METAL_BIGNUM  = "Graphics/letters/metal_numbers.png";

static constexpr float NUM_CHAR_W = 12.0f;
static constexpr float NUM_CHAR_H = 14.0f;
static constexpr float HSF_CHAR_W = 16.0f;
static constexpr float HSF_CHAR_H = 16.0f;
static constexpr float BIG_CHAR_W = 8.0f;
static constexpr float BIG_CHAR_H = 16.0f;

UiManager::UiManager()
    : credits(0), score(0), level(1), timeLeft(60), bombs(10), ammo(0),
    timeAccum(0.0f), introTimer(0.0f),
    blinkAccum(0.0f), blinkVisible(true),
    idleTimer(0.0f), goVisible(false), goBlinkAccum(0.0f), goBlinkOn(false)
{ }

void UiManager::Init() {
    texArms = LoadTexture(PATH_ARMS);
    texBomb = LoadTexture(PATH_BOMB);
    texCannon = LoadTexture(PATH_CANNON);
    texTimeLevel = LoadTexture(PATH_TIMELEVEL);
    texHudFont2Big = LoadTexture(PATH_FONT2BIG);
    texHudFont2Num = LoadTexture(PATH_FONT2NUM);
    texHudFont2Small = LoadTexture(PATH_FONT2SMALL);
    texHighScore = LoadTexture(PATH_HIGHSCORE);
    texHighScoreSmall = LoadTexture(PATH_HIGHSCORE_SM);
    texHpBarLeft = LoadTexture(PATH_HPBAR_L);
    texHpBarRight = LoadTexture(PATH_HPBAR_R);
    texHpBarParts = LoadTexture(PATH_HPBAR_P);
    texGo = LoadTexture(PATH_GO);
    texTimeNum     = LoadTexture(PATH_TIME_NUM);
    texMetalBigNum = LoadTexture(PATH_METAL_BIGNUM);

    SetTextureFilter(texHudFont2Num, TEXTURE_FILTER_POINT);
    SetTextureFilter(texHighScore, TEXTURE_FILTER_POINT);
    SetTextureFilter(texHudFont2Big, TEXTURE_FILTER_POINT);
    SetTextureFilter(texHighScoreSmall, TEXTURE_FILTER_POINT);
    SetTextureFilter(texArms, TEXTURE_FILTER_POINT);
    SetTextureFilter(texBomb, TEXTURE_FILTER_POINT);
    SetTextureFilter(texCannon, TEXTURE_FILTER_POINT);
    SetTextureFilter(texTimeLevel, TEXTURE_FILTER_POINT);
    SetTextureFilter(texGo, TEXTURE_FILTER_POINT);
    SetTextureFilter(texTimeNum,     TEXTURE_FILTER_POINT);
    SetTextureFilter(texMetalBigNum, TEXTURE_FILTER_POINT);
    SetTextureFilter(texHpBarParts,  TEXTURE_FILTER_POINT);
    SetTextureFilter(texHpBarLeft, TEXTURE_FILTER_POINT);
    SetTextureFilter(texHpBarRight, TEXTURE_FILTER_POINT);

}

UiManager::~UiManager()
{
    UnloadTexture(texArms);   UnloadTexture(texBomb);
    UnloadTexture(texCannon); UnloadTexture(texTimeLevel);
    UnloadTexture(texHudFont2Big); UnloadTexture(texHudFont2Num);
    UnloadTexture(texHudFont2Small); UnloadTexture(texHighScore);
    UnloadTexture(texHighScoreSmall);
    UnloadTexture(texHpBarLeft); UnloadTexture(texHpBarRight); UnloadTexture(texHpBarParts);
    UnloadTexture(texGo);
    UnloadTexture(texTimeNum);
    UnloadTexture(texMetalBigNum);
}

void UiManager::Update()
{
    float dt = GetFrameTime();
    if (introTimer < INTRO_DURATION)
    {
        introTimer += dt; blinkAccum += dt;
        if (blinkAccum >= BLINK_INTERVAL) { blinkAccum -= BLINK_INTERVAL; blinkVisible = !blinkVisible; }
        return;
    }
    if (timeLeft > 0)
    {
        timeAccum += dt;
        if (timeAccum >= TICK_INTERVAL) { timeAccum -= TICK_INTERVAL; timeLeft--; if (timeLeft < 0) timeLeft = 0; }
    }
    UpdateGoTimer(dt);
}

void UiManager::DrawCreditsOnly()
{
    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    char cText[16];
    std::snprintf(cText, sizeof(cText), "CREDIT %02d", credits);

    float scale = 1.5f;
    float w = MeasureScoreText(cText, scale);

    float x = (float)SW - w - 40.0f;
    float y = (float)SH - HSF_CHAR_H * scale - 10.0f;

    DrawScoreText(cText, { x, y }, scale);
}

void UiManager::UpdateGoTimer(float dt)
{
    idleTimer += dt;
    if (idleTimer >= IDLE_THRESHOLD)
    {
        goVisible = true;
        goBlinkAccum += dt;
        if (goBlinkAccum >= GO_BLINK_RATE) { goBlinkAccum -= GO_BLINK_RATE; goBlinkOn = !goBlinkOn; }
    }
    else { goVisible = false; goBlinkOn = false; goBlinkAccum = 0.0f; }
}

void UiManager::NotifyPlayerMoved()
{
    idleTimer = 0.0f; goVisible = false; goBlinkOn = false; goBlinkAccum = 0.0f;
}

bool UiManager::IsMissionIntroOver() const { return introTimer >= INTRO_DURATION; }

void UiManager::UseGrenade() { if (bombs > 0) bombs--; }
void UiManager::SetAmmo(int amount) { ammo = amount; }
void UiManager::UseAmmo() { if (ammo > 0) ammo--; }

void UiManager::DrawHudDigit(char c, Vector2 pos, float scale, Color tint) const
{
    if (c < '0' || c > '9') return;

    // X starty z Twojego sprite
    static const int digitX[10] = {
        1,   // 0
        13,  // 1
        25,  // 2
        37,  // 3
        49,  // 4
        61,  // 5
        73,  // 6
        85,  // 7
        97,  // 8
        109  // 9
    };

    // 🔥 PRAWDZIWE szerokości cyfr
    static const int digitW[10] = {
        9, // 0
        6, // 1  👈 KLUCZ
        9, // 2
        9, // 3
        9, // 4
        9, // 5
        9, // 6
        9, // 7
        9, // 8
        9  // 9
    };

    int d = c - '0';

    Rectangle src = {
        (float)digitX[d],
        0.0f,
        (float)digitW[d],
        NUM_CHAR_H
    };

    Rectangle dst = {
        pos.x,
        pos.y,
        digitW[d] * scale,
        NUM_CHAR_H * scale
    };

    DrawTexturePro(texHudFont2Num, src, dst, { 0,0 }, 0.0f, tint);
}

void UiManager::DrawHudNumber(int value, int digits, Vector2 pos, float scale, Color tint) const
{
    char buf[16];
    if (digits > 0) std::snprintf(buf, sizeof(buf), "%0*d", digits, value);
    else            std::snprintf(buf, sizeof(buf), "%d", value);
    float x = pos.x;
    for (int i = 0; buf[i]; ++i)
    {
        if (buf[i] != ' ') DrawHudDigit(buf[i], { x, pos.y }, scale, tint);
        static const int digitW[10] = {
    9,6,9,9,9,9,9,9,9,9
        };

        int d = buf[i] - '0';
        x += digitW[d] * scale;
    }
}

float UiManager::MeasureHudNumber(int digits, float scale) const
{
    return (float)digits * NUM_CHAR_W * scale;
}

void UiManager::DrawBigLetter(char c, Vector2 pos, float scale) const
{
    int col = -1;
    if (c >= 'A' && c <= 'Z') col = c - 'A';
    else if (c >= 'a' && c <= 'z') col = c - 'a';
    if (col < 0) return;
    Rectangle src = { (float)col * BIG_CHAR_W, 0.0f, BIG_CHAR_W, BIG_CHAR_H };
    Rectangle dst = { pos.x, pos.y, BIG_CHAR_W * scale, BIG_CHAR_H * scale };
    DrawTexturePro(texHudFont2Big, src, dst, { 0,0 }, 0.0f, WHITE);
}

float UiManager::MeasureBigText(const char* str, float scale) const
{
    float w = 0;
    int len = (int)std::strlen(str);
    for (int i = 0; i < len; ++i) w += BIG_CHAR_W * scale;
    return w;
}

void UiManager::DrawScoreChar(char c, Vector2 pos, float scale) const
{
    int col = -1;
    if (c >= 'A' && c <= 'Z') col = c - 'A';
    else if (c >= 'a' && c <= 'z') col = c - 'a';
    else if (c >= '0' && c <= '9') col = 26 + (c - '0');
    else if (c == '-')              col = 36;
    if (col < 0) return;
    Rectangle src = { (float)col * HSF_CHAR_W, 0.0f, HSF_CHAR_W, HSF_CHAR_H };
    Rectangle dst = { pos.x, pos.y, HSF_CHAR_W * scale, HSF_CHAR_H * scale };
    DrawTexturePro(texHighScore, src, dst, { 0,0 }, 0.0f, WHITE);
}

void UiManager::DrawScoreText(const char* str, Vector2 pos, float scale) const
{
    float x = pos.x;
    for (int i = 0; str[i]; ++i)
    {
        if (str[i] == ' ') { x += HSF_CHAR_W * scale * 0.5f; continue; }
        DrawScoreChar(str[i], { x, pos.y }, scale);
        x += HSF_CHAR_W * scale;
    }
}

float UiManager::MeasureScoreText(const char* str, float scale) const
{
    float w = 0;
    for (int i = 0; str[i]; ++i) w += (str[i] == ' ') ? HSF_CHAR_W * scale * 0.5f : HSF_CHAR_W * scale;
    return w;
}

void UiManager::DrawHpBar(Vector2 pos, int segs, float scale) const
{
    DrawTextureEx(texHpBarLeft, pos, 0.0f, scale, WHITE);
    float offsetX = (float)texHpBarLeft.width * scale;
    float pW = (float)texHpBarParts.width * scale;
    for (int i = 0; i < segs; ++i)
    {
        DrawTextureEx(texHpBarParts, { pos.x + offsetX, pos.y }, 0.0f, scale, WHITE);
        offsetX += pW;
    }
    DrawTextureEx(texHpBarRight, { pos.x + offsetX, pos.y }, 0.0f, scale, WHITE);
}

void UiManager::DrawHUD(Camera2D /*camera*/)
{
    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    const float topPad = 10.0f;
    const float scoreSc = 1.5f;
    const float timeScale = 4.0f;

    // --- TIMER (center top) ---
    float timeY   = topPad + (HSF_CHAR_H * scoreSc) - 10.0f;
    float timerH  = 16.0f * timeScale;
    float totalW  = 16.0f * timeScale * 2;
    float startXTime = (float)SW * 0.5f - totalW * 0.5f;
    DrawTimeNumber(timeLeft, { startXTime, timeY }, timeScale);

    // --- ARMS/BOMB — skalowane do wysokości timera ---
    float hudSc      = timerH / (float)texArms.height;
    float bombOffsetX = 31.0f * hudSc;
    float totalFrameW = bombOffsetX + (float)texBomb.width * hudSc;

    float hudY = timeY;
    float hudX = startXTime - totalFrameW - 12.0f;

    // Pozycja score
    char sBuf[16];
    std::snprintf(sBuf, sizeof(sBuf), "%d", score);
    float scoreW = MeasureScoreText(sBuf, scoreSc);
    float scoreX = hudX - scoreW - 8.0f;

    if (continueScreenActive)
    {
        if (continueBlinkOn)
        {
            const char* contLabel = "CONTINUE";
            const float lblScY = 4.0f;
            const float areaW  = startXTime * 0.55f - 18.0f;
            const float baseW  = MeasureScoreText(contLabel, 1.0f);
            const float lblScX = (baseW > 0.0f && areaW > 0.0f)
                                 ? std::min(2.5f, areaW / baseW) : 2.5f;
            float x = 28.0f;
            for (int i = 0; contLabel[i]; ++i) {
                char c = contLabel[i];
                int col = (c >= 'A' && c <= 'Z') ? c - 'A' : (c >= 'a' && c <= 'z') ? c - 'a' : -1;
                if (col < 0) { x += HSF_CHAR_W * lblScX; continue; }
                Rectangle src = { (float)col * HSF_CHAR_W, 0.0f, HSF_CHAR_W, HSF_CHAR_H };
                Rectangle dst = { x, hudY, HSF_CHAR_W * lblScX, HSF_CHAR_H * lblScY };
                DrawTexturePro(texHighScore, src, dst, { 0.0f, 0.0f }, 0.0f, WHITE);
                x += HSF_CHAR_W * lblScX;
            }
        }
    }
    else
    {
        // Normalne rysowanie ramek i score
        DrawTextureEx(texArms, { hudX,              hudY }, 0.0f, hudSc, WHITE);
        DrawTextureEx(texBomb, { hudX + bombOffsetX, hudY }, 0.0f, hudSc, WHITE);

        const float innerSc = 1.4f;
        float innerNumY   = hudY + (12.5f * hudSc) - (8.0f * innerSc);
        float armsCenterX = hudX + (18.0f * hudSc);
        float bombCenterX = hudX + bombOffsetX + (18.0f * hudSc);

        if (weaponDisplay == WeaponDisplay::MACHINEGUN)
        {
            char ammoBuf[8];
            std::snprintf(ammoBuf, sizeof(ammoBuf), "%03d", ammo);
            float ammoW = MeasureInnerText(ammoBuf, innerSc);
            DrawInnerText(ammoBuf, { armsCenterX - ammoW * 0.5f, innerNumY }, innerSc);
        }
        else
        {
            const char* infPlaceholder = "...";
            float infW = MeasureInnerText(infPlaceholder, innerSc);
            DrawInnerText(infPlaceholder, { armsCenterX - infW * 0.5f, innerNumY }, innerSc);
        }

        char bombBuf[8];
        std::snprintf(bombBuf, sizeof(bombBuf), "%02d", bombs);
        float bombW = MeasureInnerText(bombBuf, innerSc);
        DrawInnerText(bombBuf, { bombCenterX - bombW * 0.5f, innerNumY }, innerSc);

        DrawScoreText(sBuf, { scoreX, hudY }, scoreSc);

        continueLabelX = scoreX;
        continueLabelY = hudY;
    }

    // --- DOLNY HUD ---
    char lvlText[16]; std::snprintf(lvlText, sizeof(lvlText), "LEVEL-%d", level);
    float lSc = 1.5f;
    float lW = MeasureScoreText(lvlText, lSc);
    float lX = (float)SW * 0.5f - lW * 0.5f;
    float lY = (float)SH - HSF_CHAR_H * lSc - 10.0f;
    DrawScoreText(lvlText, { lX, lY }, lSc);

    char cText[16]; std::snprintf(cText, sizeof(cText), "CREDIT %02d", credits);
    float cSc = 1.5f;
    float cW = MeasureScoreText(cText, cSc);
    float cX = (float)SW - cW - 40.0f;
    float cY = (float)SH - HSF_CHAR_H * cSc - 10.0f;
    DrawScoreText(cText, { cX, cY }, cSc);

    if (goVisible && goBlinkOn)
    {
        const float goSc = 4.5f; 

        float goW = texGo.width * goSc;

        
        float goX = (float)SW * 0.80f - goW * 0.5f;

        
        float goY = hudY + 40.0f;

        DrawTextureEx(texGo, { goX, goY }, 0.0f, goSc, WHITE);
    }

    DrawMissionIntroInternal();
}

void UiManager::DrawMissionIntro() { DrawMissionIntroInternal(); }

void UiManager::DrawMissionIntroInternal()
{
    if (IsMissionIntroOver() || !blinkVisible) return;
    int SW = GetScreenWidth(); int SH = GetScreenHeight();
    char text[32]; std::snprintf(text, sizeof(text), "MISSION %02d", level);
    float sc = 3.0f;
    float w = MeasureScoreText(text, sc);
    DrawScoreText(text, { (float)SW * 0.5f - w * 0.5f, (float)SH * 0.5f }, sc);
}

void UiManager::SetCredits(int amount) { credits += amount; if (credits > 99) credits = 99; if (credits < 0) credits = 0; }
int  UiManager::GetCredits()  const { return credits; }
void UiManager::AddScore(int amount) { score += amount; if (score > 9999999) score = 9999999; if (score < 0) score = 0; }
int  UiManager::GetScore()    const { return score; }
int  UiManager::GetTimeLeft() const { return timeLeft; }
bool UiManager::IsTimeUp()    const { return timeLeft <= 0; }
void UiManager::NextLevel() { level++; introTimer = 0.0f; blinkAccum = 0.0f; blinkVisible = true; timeLeft = 60; timeAccum = 0.0f; }
int  UiManager::GetLevel() const { return level; }

void UiManager::DrawTimeDigit(char c, Vector2 pos, float scale) const
{
    if (c < '0' || c > '9') return;

    int d = c - '0';

    const int CHAR_W = 16;
    const int CHAR_H = 16;

    Rectangle src = {
        (float)(d * CHAR_W),
        0.0f,
        (float)CHAR_W,
        (float)CHAR_H
    };

    Rectangle dst = {
        pos.x,
        pos.y,
        CHAR_W * scale,
        CHAR_H * scale
    };

    DrawTexturePro(texTimeNum, src, dst, { 0,0 }, 0.0f, WHITE);
}

void UiManager::DrawTimeNumber(int value, Vector2 pos, float scale) const
{
    char buf[8];
    std::snprintf(buf, sizeof(buf), "%02d", value);

    float x = pos.x;

    const int CHAR_W = 16;

    for (int i = 0; buf[i]; ++i)
    {
        DrawTimeDigit(buf[i], { x, pos.y }, scale);
        x += CHAR_W * scale;
    }
}

// Sprite: 0123456789:. — każdy znak 16x16 px
void UiManager::DrawInnerChar(char c, Vector2 pos, float scale) const
{
    const int CHAR_W = 16;
    const int CHAR_H = 16;

    int idx = -1;
    if (c >= '0' && c <= '9') idx = c - '0';
    else if (c == ':')        idx = 10;
    else if (c == '.')        idx = 11;
    if (idx < 0) return;

    Rectangle src = { (float)(idx * CHAR_W), 0.0f, (float)CHAR_W, (float)CHAR_H };
    Rectangle dst = { pos.x, pos.y, CHAR_W * scale, CHAR_H * scale };
    DrawTexturePro(texTimeNum, src, dst, { 0,0 }, 0.0f, WHITE);
}

void UiManager::DrawInnerText(const char* str, Vector2 pos, float scale) const
{
    const float CHAR_W = 16.0f;
    float x = pos.x;
    for (int i = 0; str[i]; ++i)
    {
        DrawInnerChar(str[i], { x, pos.y }, scale);
        x += CHAR_W * scale;
    }
}

float UiManager::MeasureInnerText(const char* str, float scale) const
{
    int len = 0;
    for (int i = 0; str[i]; ++i) ++len;
    return len * 16.0f * scale;
}

// ─────────────────────────────────────────
//  Continue screen
// ─────────────────────────────────────────

void UiManager::StartContinue()
{
    continueElapsed      = 0.0f;
    continueBlinkAccum   = 0.0f;
    continueBlinkOn      = true;
    continueScreenActive = true;
}

void UiManager::StopContinue()
{
    continueScreenActive = false;
}

void UiManager::UpdateContinue(float dt)
{
    continueElapsed    += dt;
    continueBlinkAccum += dt;
    if (continueBlinkAccum >= 0.25f) {
        continueBlinkAccum -= 0.25f;
        continueBlinkOn = !continueBlinkOn;
    }
}

bool UiManager::IsContinueOver() const
{
    return continueElapsed >= 10.0f;
}

void UiManager::DrawContinueScreen()
{
    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    // Aktualnie wyswietlana cyfra (9 -> 0)
    int currentDigit = 9 - (int)continueElapsed;
    if (currentDigit < 0) currentDigit = 0;

    // Animacja "obrotu" — skalowanie w osi X
    // sprite: "1234567890" => index cyfry d: d==0 -> 9, else d-1
    float flipPhase = std::fmod(continueElapsed, 1.0f);
    float xScale;
    if (flipPhase < 0.2f)
        xScale = flipPhase / 0.2f;
    else if (flipPhase < 0.82f)
        xScale = 1.0f;
    else
        xScale = 1.0f - (flipPhase - 0.82f) / 0.18f;
    if (xScale < 0.0f) xScale = 0.0f;

    int spriteIdx = (currentDigit == 0) ? 9 : currentDigit - 1;
    float charW   = (float)texMetalBigNum.width / 10.0f;
    float charH   = (float)texMetalBigNum.height;
    float bigSc   = 4.5f;

    float renderW    = charW * bigSc * xScale;
    float renderH    = charH * bigSc;
    float numCenterX = (float)SW * 0.5f;
    float numY       = (float)SH * 0.42f;

    if (renderW > 0.5f) {
        Rectangle src = { spriteIdx * charW, 0.0f, charW, charH };
        Rectangle dst = { numCenterX - renderW * 0.5f, numY, renderW, renderH };
        DrawTexturePro(texMetalBigNum, src, dst, { 0.0f, 0.0f }, 0.0f, WHITE);
    }

    // "CONTINUE?" wycentrowane nad cyfra
    const char* contText = "CONTINUE?";
    float textSc = 4.0f;
    float textW  = MeasureScoreText(contText, textSc);
    float textY  = (float)SH * 0.25f;
    DrawScoreText(contText, { (float)SW * 0.5f - textW * 0.5f, textY }, textSc);

    // Liczba kreditow (dolny prawy rog)
    DrawCreditsOnly();
}