#include "UiManager.hpp"
#include <cstdio>
#include <cstring>

// ============================================================
//  Stale
// ============================================================
static const float TICK_INTERVAL = 1.0f;   // odliczanie co sekunde

const float UiManager::INTRO_DURATION = 3.0f;
const float UiManager::BLINK_INTERVAL = 0.25f;

// ============================================================
//  Konstruktor / Destruktor
// ============================================================
UiManager::UiManager()
    : credits(0), score(0), level(1), timeLeft(60),
    timeAccum(0.0f), introTimer(0.0f),
    blinkAccum(0.0f), blinkVisible(true)
{
    texMetalNumbers = LoadTexture("Graphics/letters/metal_numbers.png");
    texTimeNumbers = LoadTexture("Graphics/letters/time_numbers.png");
    texYellowLetters = LoadTexture("Graphics/letters/yellow_numbers_and_letters.png");
}

UiManager::~UiManager()
{
    UnloadTexture(texMetalNumbers);
    UnloadTexture(texTimeNumbers);
    UnloadTexture(texYellowLetters);
}

// ============================================================
//  Update
// ============================================================
void UiManager::Update()
{
    if (introTimer < INTRO_DURATION)
    {
        introTimer += GetFrameTime();

        blinkAccum += GetFrameTime();
        if (blinkAccum >= BLINK_INTERVAL)
        {
            blinkAccum -= BLINK_INTERVAL;
            blinkVisible = !blinkVisible;
        }
        return;
    }

    if (timeLeft <= 0) return;

    timeAccum += GetFrameTime();
    if (timeAccum >= TICK_INTERVAL)
    {
        timeAccum -= TICK_INTERVAL;
        timeLeft--;
        if (timeLeft < 0) timeLeft = 0;
    }
}

bool UiManager::IsMissionIntroOver() const
{
    return introTimer >= INTRO_DURATION;
}

// ============================================================
//  Rysowanie pojedynczych znakow - metal_numbers
//  Kolejnosc w spritesheet: 1 2 3 4 5 6 7 8 9 0
//  wiec indeks cyfry 0 -> kolumna 9, cyfry 1 -> kolumna 0, itd.
// ============================================================
void UiManager::DrawMetalDigit(int digit, Vector2 pos, float scale) const
{
    // digit: 0-9
    int col = (digit == 0) ? 9 : digit - 1;

    Rectangle src = {
        (float)(col * METAL_W),
        0.0f,
        (float)METAL_W,
        (float)METAL_H
    };
    Rectangle dst = {
        pos.x,
        pos.y,
        (float)METAL_W * scale,
        (float)METAL_H * scale
    };
    DrawTexturePro(texMetalNumbers, src, dst, { 0, 0 }, 0.0f, WHITE);
}

void UiManager::DrawMetalNumber(int value, int digits, Vector2 pos, float scale) const
{
    char buf[16];
    if (digits > 0)
        std::snprintf(buf, sizeof(buf), "%0*d", digits, value);
    else
        std::snprintf(buf, sizeof(buf), "%d", value);

    float x = pos.x;
    for (int i = 0; buf[i] != '\0'; ++i)
    {
        int d = buf[i] - '0';
        DrawMetalDigit(d, { x, pos.y }, scale);
        x += METAL_W * scale;
    }
}

float UiManager::MeasureMetalNumber(int digits, float scale) const
{
    return digits * METAL_W * scale;
}

// ============================================================
//  Rysowanie pojedynczych znakow - time_numbers
//  Kolejnosc w spritesheet: 0 1 2 3 4 5 6 7 8 9 : .
// ============================================================
void UiManager::DrawTimeDigit(char c, Vector2 pos, float scale) const
{
    int col = -1;
    if (c >= '0' && c <= '9') col = c - '0';
    else if (c == ':')         col = 10;
    else if (c == '.')         col = 11;

    if (col < 0) return;

    Rectangle src = {
        (float)(col * TIME_W),
        0.0f,
        (float)TIME_W,
        (float)TIME_H
    };
    Rectangle dst = {
        pos.x,
        pos.y,
        (float)TIME_W * scale,
        (float)TIME_H * scale
    };
    DrawTexturePro(texTimeNumbers, src, dst, { 0, 0 }, 0.0f, WHITE);
}

void UiManager::DrawTimeString(const char* str, Vector2 pos, float scale) const
{
    float x = pos.x;
    for (int i = 0; str[i] != '\0'; ++i)
    {
        DrawTimeDigit(str[i], { x, pos.y }, scale);
        x += TIME_W * scale;
    }
}

float UiManager::MeasureTimeString(const char* str, float scale) const
{
    int len = 0;
    for (int i = 0; str[i] != '\0'; ++i) ++len;
    return len * TIME_W * scale;
}

// ============================================================
//  Rysowanie pojedynczych znakow - yellow_letters
//  Kolejnosc: A-Z (26) + 0-9 (10) + ! ?   = 38 znakow, 32px kazdy
// ============================================================
void UiManager::DrawYellowChar(char c, Vector2 pos, float scale) const
{
    int col = -1;

    if (c >= 'A' && c <= 'Z')        col = c - 'A';          // 0-25
    else if (c >= 'a' && c <= 'z')   col = c - 'a';          // tez 0-25 (male jak duze)
    else if (c >= '0' && c <= '9')   col = 26 + (c - '0');   // 26-35
    else if (c == '!')                col = 36;
    else if (c == '?')                col = 37;
    // spacja i inne: pomijamy

    if (col < 0) return;

    Rectangle src = {
        (float)(col * YELLOW_W),
        0.0f,
        (float)YELLOW_W,
        (float)YELLOW_H
    };
    Rectangle dst = {
        pos.x,
        pos.y,
        (float)YELLOW_W * scale,
        (float)YELLOW_H * scale
    };
    DrawTexturePro(texYellowLetters, src, dst, { 0, 0 }, 0.0f, WHITE);
}

void UiManager::DrawYellowText(const char* str, Vector2 pos, float scale) const
{
    float x = pos.x;
    for (int i = 0; str[i] != '\0'; ++i)
    {
        if (str[i] == ' ')
        {
            x += YELLOW_W * scale * 0.6f;  // spacja = 60% szerokosci znaku
            continue;
        }
        DrawYellowChar(str[i], { x, pos.y }, scale);
        x += YELLOW_W * scale;
    }
}

float UiManager::MeasureYellowText(const char* str, float scale) const
{
    float w = 0.0f;
    for (int i = 0; str[i] != '\0'; ++i)
    {
        if (str[i] == ' ')
            w += YELLOW_W * scale * 0.6f;
        else
            w += YELLOW_W * scale;
    }
    return w;
}

// ============================================================
//  DrawMissionIntro
// ============================================================
void UiManager::DrawMissionIntro()
{
    if (IsMissionIntroOver()) return;
    if (!blinkVisible)        return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // "MISSION X" - skalujemy yellow_letters do rozmiaru ~72px (scale = 72/32 = 2.25)
    float bigScale = 2.25f;
    float smallScale = 1.125f;  // ~36px

    char missionText[16];
    std::snprintf(missionText, sizeof(missionText), "MISSION %d", level);

    float mw = MeasureYellowText(missionText, bigScale);
    float mx = screenW / 2.0f - mw / 2.0f;
    float my = screenH / 2.0f - YELLOW_H * bigScale;

    DrawYellowText(missionText, { mx, my }, bigScale);

    // "Start!" pod spodem
    const char* startText = "START";
    float sw = MeasureYellowText(startText, smallScale);
    float sx = screenW / 2.0f - sw / 2.0f;
    float sy = my + YELLOW_H * bigScale + 16.0f;

    DrawYellowText(startText, { sx, sy }, smallScale);
}

// ============================================================
//  DrawCredits  (glowne HUD)
// ============================================================
void UiManager::DrawCredits(Camera2D camera)
{
    int   screenW = GetScreenWidth();
    int   screenH = GetScreenHeight();

    Color silver = { 192, 192, 192, 255 };

    // Duze skale - wyraznie widoczne
    float labelScale = 1.1f;   // ARMS, BOMB, INF
    float scoreScale = 1.1f;   // 1UP
    float metalScale = 0.75f;  // cyfry score - mniejsze
    float timerScale = 4.5f;   // timer
    float bottomScale = 1.1f;   // LEVEL, CREDIT

    float topY = 4.0f;
    float padL = 30.0f;
    float padR = 20.0f;
    float bottomY = (float)screenH - YELLOW_H * bottomScale - 8.0f;

    // --- pomocnicza lambda do srebrnego tekstu ---
    auto DrawSilver = [&](const char* str, float x, float y, float scale)
        {
            for (int i = 0; str[i] != '\0'; ++i)
            {
                char c = str[i];
                if (c == ' ') { x += YELLOW_W * scale * 0.6f; continue; }
                if (c == '-') { x += YELLOW_W * scale * 0.4f; continue; }
                int col = -1;
                if (c >= 'A' && c <= 'Z')      col = c - 'A';
                else if (c >= 'a' && c <= 'z') col = c - 'a';
                else if (c >= '0' && c <= '9') col = 26 + (c - '0');
                else if (c == '!')             col = 36;
                else if (c == '?')             col = 37;
                if (col < 0) { x += YELLOW_W * scale; continue; }
                Rectangle src = { (float)(col * YELLOW_W), 0.0f, (float)YELLOW_W, (float)YELLOW_H };
                Rectangle dst = { x, y, YELLOW_W * scale, YELLOW_H * scale };
                DrawTexturePro(texYellowLetters, src, dst, { 0, 0 }, 0.0f, silver);
                x += YELLOW_W * scale;
            }
        };

    // --------------------------------------------------------
    //  1UP + SCORE  - lewy gorny
    // --------------------------------------------------------
    {
        float lx = padL;
        float ly = topY;
        DrawYellowText("1UP", { lx, ly }, scoreScale);
        float scoreY = ly + YELLOW_H * scoreScale + 2.0f;
        DrawMetalNumber(score, 7, { lx, scoreY }, metalScale);
    }

    // --------------------------------------------------------
    //  ARMS + BOMB  - po lewej stronie (1/4 ekranu)
    // --------------------------------------------------------
    {
        float cx = padL + MeasureMetalNumber(7, metalScale) + 8.0f;
        float ly = topY;
        float ammoY = ly + YELLOW_H * labelScale + 2.0f;

        DrawSilver("ARMS", cx, ly, labelScale);
        DrawSilver("INF", cx, ammoY, labelScale);

        float bombX = cx + MeasureYellowText("ARMS", labelScale) + 16.0f;
        DrawSilver("BOMB", bombX, ly, labelScale);
        DrawMetalNumber(10, 2, { bombX, ammoY }, metalScale * 0.75f);
    }

    // --------------------------------------------------------
    //  TIMER - dokladnie na srodku
    // --------------------------------------------------------
    {
        char timerStr[8];
        std::snprintf(timerStr, sizeof(timerStr), "%d", timeLeft);
        float timerW = MeasureTimeString(timerStr, timerScale);
        float tx = screenW / 2.0f - timerW / 2.0f;
        float ty = topY;

        Color timerTint = (timeLeft <= 10) ? RED : WHITE;
        float x = tx;
        for (int i = 0; timerStr[i] != '\0'; ++i)
        {
            int col = timerStr[i] - '0';
            Rectangle src = { (float)(col * TIME_W), 0.0f, (float)TIME_W, (float)TIME_H };
            Rectangle dst = { x, ty, (float)TIME_W * timerScale, (float)TIME_H * timerScale };
            DrawTexturePro(texTimeNumbers, src, dst, { 0, 0 }, 0.0f, timerTint);
            x += TIME_W * timerScale;
        }
    }

    // --------------------------------------------------------
    //  LEVEL  - dol lewy, srebrny
    // --------------------------------------------------------
    {
        char levelText[16];
        std::snprintf(levelText, sizeof(levelText), "LEVEL-%d", level);
        DrawSilver(levelText, padL, bottomY, bottomScale);
    }

    // --------------------------------------------------------
    //  CREDIT + numer  - dol prawy, srebrny
    // --------------------------------------------------------
    {
        char credStr[8];
        std::snprintf(credStr, sizeof(credStr), "%02d", credits);
        float totalW = MeasureYellowText("CREDIT", bottomScale)
            + 8.0f
            + MeasureYellowText(credStr, bottomScale);
        float cx = (float)screenW - totalW - padR;
        DrawSilver("CREDIT", cx, bottomY, bottomScale);
        DrawSilver(credStr, cx + MeasureYellowText("CREDIT", bottomScale) + 8.0f, bottomY, bottomScale);
    }

    // --------------------------------------------------------
    //  MISSION INTRO
    // --------------------------------------------------------
    DrawMissionIntro();
}

// ============================================================
//  Settery / Gettery
// ============================================================
void UiManager::SetCredits(int amount)
{
    credits += amount;
    if (credits > 99) credits = 99;
    if (credits < 0)  credits = 0;
}

int  UiManager::GetCredits()  const { return credits; }

void UiManager::AddScore(int amount)
{
    score += amount;
    if (score > 9999999) score = 9999999;
    if (score < 0)       score = 0;
}

int  UiManager::GetScore()    const { return score; }
int  UiManager::GetTimeLeft() const { return timeLeft; }
bool UiManager::IsTimeUp()    const { return timeLeft <= 0; }

void UiManager::NextLevel()
{
    level++;
    introTimer = 0.0f;
    blinkAccum = 0.0f;
    blinkVisible = true;
    timeLeft = 60;
    timeAccum = 0.0f;
}

int UiManager::GetLevel() const { return level; }