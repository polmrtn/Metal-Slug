#include "UiManager.hpp"
#include <cstdio>
#include <cstring>
#include <cmath>

static const float TICK_INTERVAL = 1.0f;

const float UiManager::INTRO_DURATION = 3.0f;
const float UiManager::BLINK_INTERVAL = 0.25f;

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

static constexpr float NUM_CHAR_W = 12.0f;
static constexpr float NUM_CHAR_H = 16.0f;
static constexpr float HSF_CHAR_W = 16.0f;
static constexpr float HSF_CHAR_H = 16.0f;
static constexpr float BIG_CHAR_W = 8.0f;
static constexpr float BIG_CHAR_H = 16.0f;

UiManager::UiManager()
    : credits(2), score(0), level(1), timeLeft(60), bombs(10), ammo(0),
    timeAccum(0.0f), introTimer(0.0f),
    blinkAccum(0.0f), blinkVisible(true),
    idleTimer(0.0f), goVisible(false), goBlinkAccum(0.0f), goBlinkOn(false)
{
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

    SetTextureFilter(texHudFont2Num, TEXTURE_FILTER_POINT);
    SetTextureFilter(texHighScore, TEXTURE_FILTER_POINT);
    SetTextureFilter(texHudFont2Big, TEXTURE_FILTER_POINT);
    SetTextureFilter(texHighScoreSmall, TEXTURE_FILTER_POINT);
    SetTextureFilter(texArms, TEXTURE_FILTER_POINT);
    SetTextureFilter(texBomb, TEXTURE_FILTER_POINT);
    SetTextureFilter(texCannon, TEXTURE_FILTER_POINT);
    SetTextureFilter(texTimeLevel, TEXTURE_FILTER_POINT);
    SetTextureFilter(texGo, TEXTURE_FILTER_POINT);
    SetTextureFilter(texHpBarParts, TEXTURE_FILTER_POINT);
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
    int col = c - '0';
    Rectangle src = { (float)col * NUM_CHAR_W, 0.0f, NUM_CHAR_W, NUM_CHAR_H };
    Rectangle dst = { pos.x, pos.y, NUM_CHAR_W * scale, NUM_CHAR_H * scale };
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
        x += NUM_CHAR_W * scale;
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
    float w = 0; for (int i = 0; str[i]; ++i) w += BIG_CHAR_W * scale; return w;
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

void UiManager::DrawHpBar(Vector2 pos, int maxHP, int currentHP, float scale) const
{
    float lW = (float)texHpBarLeft.width * scale;
    float pW = (float)texHpBarParts.width * scale;

    DrawTextureEx(texHpBarLeft, { pos.x, pos.y }, 0.0f, scale, WHITE);
    float x = pos.x + lW;
    int segs = (currentHP < maxHP) ? currentHP : maxHP;
    if (segs < 0) segs = 0;
    if (segs > 12) segs = 12;
    for (int i = 0; i < segs; ++i) { DrawTextureEx(texHpBarParts, { x, pos.y }, 0.0f, scale, WHITE); x += pW; }
    DrawTextureEx(texHpBarRight, { x, pos.y }, 0.0f, scale, WHITE);
}

void UiManager::DrawHUD(Camera2D /*camera*/)
{
    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    const float hudSc = 2.0f;
    const float numSc = 2.0f;
    const float scoreSc = 1.5f;
    const float topPad = 10.0f;
    const float leftPad = 10.0f;

    // --- RAMKA LEWA (ARMS + BOMB) ---
    DrawTextureEx(texArms, { leftPad, topPad }, 0.0f, hudSc, WHITE);

    // Mocne przesuniêcie w lewo na 31.0f
    float bombOffsetX = 31.0f * hudSc;
    DrawTextureEx(texBomb, { leftPad + bombOffsetX, topPad }, 0.0f, hudSc, WHITE);

    // Pozycje cyfr (numY obni¿one o 9px w skali)
    float numY = topPad + (9.0f * hudSc);
    float armsNumX = leftPad + (6.0f * hudSc);
    float bombNumX = leftPad + bombOffsetX + (12.0f * hudSc);

    if (weaponDisplay == WeaponDisplay::MACHINEGUN && ammo > 0)
    {
        DrawHudNumber(ammo, 3, { armsNumX, numY }, numSc, WHITE);
    }
    else
    {
        const float bigCW = BIG_CHAR_W * numSc;
        DrawBigLetter('I', { armsNumX,            numY }, numSc);
        DrawBigLetter('N', { armsNumX + bigCW,     numY }, numSc);
        DrawBigLetter('F', { armsNumX + bigCW * 2,   numY }, numSc);
    }
    DrawHudNumber(bombs, 2, { bombNumX, numY }, numSc, WHITE);

    // --- PASEK HP ---
    {
        float hpY = topPad + ((float)texArms.height * hudSc) + 15.0f;
        float hpX = leftPad;
        const float lSc = 1.5f;
        const char* lbl = "SLUG";
        float lx = hpX;
        for (int i = 0; lbl[i]; ++i) { DrawBigLetter(lbl[i], { lx, hpY }, lSc); lx += BIG_CHAR_W * lSc; }
        float barY = hpY + BIG_CHAR_H * lSc + 1.0f;
        DrawHpBar({ hpX, barY }, 10, 10, 2.0f);
    }

    // --- SCORE & TIME ---
    char sBuf[16]; std::snprintf(sBuf, sizeof(sBuf), "%07d", score);
    float sCW = HSF_CHAR_W * scoreSc;
    float sTW = sCW * 7.0f;
    float sCX = (float)SW * 0.5f - sTW * 0.5f;
    float sY = topPad;
    DrawScoreText(sBuf, { sCX, sY }, scoreSc);

    float tlW = (float)texTimeLevel.width * hudSc;
    float tlH = (float)texTimeLevel.height * hudSc;
    float tlX = (float)SW * 0.5f - tlW * 0.5f;
    float tlY = sY + HSF_CHAR_H * scoreSc + 1.0f;
    DrawTexturePro(texTimeLevel, { 0,0,(float)texTimeLevel.width,(float)texTimeLevel.height },
        { tlX, tlY, tlW, tlH }, { 0,0 }, 0.0f, WHITE);

    const float tSc = 2.5f;
    const Color yellow = { 255, 220, 0, 255 };
    float numY_time = tlY + tlH + 1.0f;

    char tbuf[8]; std::snprintf(tbuf, sizeof(tbuf), "%3d", timeLeft);
    float tw = NUM_CHAR_W * tSc * 3.0f;
    float tx = tlX + tlW * 0.27f - tw * 0.5f;
    for (int i = 0; tbuf[i]; ++i) {
        if (tbuf[i] >= '0' && tbuf[i] <= '9') {
            Rectangle src = { (float)(tbuf[i] - '0') * NUM_CHAR_W, 0.0f, NUM_CHAR_W, NUM_CHAR_H };
            Rectangle dst = { tx, numY_time, NUM_CHAR_W * tSc, NUM_CHAR_H * tSc };
            DrawTexturePro(texHudFont2Num, src, dst, { 0,0 }, 0.0f, yellow);
        }
        tx += NUM_CHAR_W * tSc;
    }

    char lbuf[8]; std::snprintf(lbuf, sizeof(lbuf), "%02d", level);
    float lw = NUM_CHAR_W * tSc * 2.0f;
    float lx2 = tlX + tlW * 0.78f - lw * 0.5f;
    for (int i = 0; lbuf[i]; ++i) {
        if (lbuf[i] >= '0' && lbuf[i] <= '9') {
            Rectangle src = { (float)(lbuf[i] - '0') * NUM_CHAR_W, 0.0f, NUM_CHAR_W, NUM_CHAR_H };
            Rectangle dst = { lx2, numY_time, NUM_CHAR_W * tSc, NUM_CHAR_H * tSc };
            DrawTexturePro(texHudFont2Num, src, dst, { 0,0 }, 0.0f, yellow);
        }
        lx2 += NUM_CHAR_W * tSc;
    }

    // --- DOLNY HUD ---
    char lvlText[16]; std::snprintf(lvlText, sizeof(lvlText), "LEVEL-%d", level);
    float lSc = 1.5f;
    float lW = MeasureScoreText(lvlText, lSc);
    float lX = (float)SW * 0.5f - lW * 0.5f;
    float lY = (float)SH - HSF_CHAR_H * lSc - 10.0f;
    DrawScoreText(lvlText, { lX, lY }, lSc);

    char cText[16]; std::snprintf(cText, sizeof(cText), "CREDIT %02d", credits);
    float cSc = scoreSc;
    float cW = MeasureScoreText(cText, cSc);
    float cX = (float)SW - cW - leftPad;
    float cY = (float)SH - HSF_CHAR_H * cSc - 10.0f;
    DrawScoreText(cText, { cX, cY }, cSc);

    if (goVisible && goBlinkOn) {
        const float goSc = 3.0f;
        float goW = (float)texGo.width * goSc;
        float goH = (float)texGo.height * goSc;
        float goX = (float)SW - goW - leftPad;
        float goY = topPad;
        DrawTexturePro(texGo, { 0,0,(float)texGo.width,(float)texGo.height },
            { goX, goY, goW, goH }, { 0,0 }, 0.0f, WHITE);
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