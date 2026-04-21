#include "UiManager.hpp"
#include <cstdio>
#include <cstring>

static const float TICK_INTERVAL = 1.0f;

const float UiManager::INTRO_DURATION = 3.0f;
const float UiManager::BLINK_INTERVAL = 0.25f;

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
//  METAL NUMBERS
// ============================================================
void UiManager::DrawMetalDigit(int digit, Vector2 pos, float scale) const
{
    int col = (digit == 0) ? 9 : digit - 1;
    Rectangle src = { (float)(col * METAL_W), 0.0f, (float)METAL_W, (float)METAL_H };
    Rectangle dst = { pos.x, pos.y, METAL_W * scale, METAL_H * scale };
    DrawTexturePro(texMetalNumbers, src, dst, { 0,0 }, 0.0f, WHITE);
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
        DrawMetalDigit(buf[i] - '0', { x, pos.y }, scale);
        x += METAL_W * scale;
    }
}

float UiManager::MeasureMetalNumber(int digits, float scale) const
{
    return digits * METAL_W * scale;
}

// ============================================================
//  TIME NUMBERS
// ============================================================
void UiManager::DrawTimeDigit(char c, Vector2 pos, float scale) const
{
    int col = -1;
    if (c >= '0' && c <= '9') col = c - '0';
    else if (c == ':')             col = 10;
    else if (c == '.')             col = 11;
    if (col < 0) return;

    Rectangle src = { col * TIME_W, 0, TIME_W, TIME_H };
    Rectangle dst = { pos.x, pos.y, TIME_W * scale, TIME_H * scale };
    DrawTexturePro(texTimeNumbers, src, dst, { 0,0 }, 0.0f, WHITE);
}

void UiManager::DrawTimeString(const char* str, Vector2 pos, float scale) const
{
    float x = pos.x;
    for (int i = 0; str[i]; ++i)
    {
        DrawTimeDigit(str[i], { x, pos.y }, scale);
        x += TIME_W * scale;
    }
}

float UiManager::MeasureTimeString(const char* str, float scale) const
{
    return (int)strlen(str) * TIME_W * scale;
}

// ============================================================
//  YELLOW TEXT  (spacing: 0.78 = lekko scisniety ale czytelny)
// ============================================================
void UiManager::DrawYellowChar(char c, Vector2 pos, float scale) const
{
    int col = -1;
    if (c >= 'A' && c <= 'Z') col = c - 'A';
    else if (c >= 'a' && c <= 'z') col = c - 'a';
    else if (c >= '0' && c <= '9') col = 26 + (c - '0');
    else if (c == '!')              col = 36;
    else if (c == '?')              col = 37;
    if (col < 0) return;

    Rectangle src = { col * YELLOW_W, 0, YELLOW_W, YELLOW_H };
    Rectangle dst = { pos.x, pos.y, YELLOW_W * scale, YELLOW_H * scale };
    DrawTexturePro(texYellowLetters, src, dst, { 0,0 }, 0.0f, WHITE);
}

void UiManager::DrawYellowText(const char* str, Vector2 pos, float scale, float spacing) const
{
    float step = YELLOW_W * scale * spacing;
    float x = pos.x;
    for (int i = 0; str[i]; ++i)
    {
        if (str[i] == ' ') { x += step * 0.6f; continue; }
        DrawYellowChar(str[i], { x, pos.y }, scale);
        x += step;
    }
}

float UiManager::MeasureYellowText(const char* str, float scale, float spacing) const
{
    float step = YELLOW_W * scale * spacing;
    float w = 0.0f;
    for (int i = 0; str[i]; ++i)
    {
        if (str[i] == ' ') w += step * 0.6f;
        else                w += step;
    }
    return w;
}

// ============================================================
//  DRAW HUD
// ============================================================
void UiManager::DrawCredits(Camera2D camera)
{
    int   screenW = GetScreenWidth();
    int   screenH = GetScreenHeight();

    float labelScale = 1.1f;
    float metalScale = 0.75f;
    float timerScale = 4.5f;
    float bottomScale = 1.1f;
    float sp = 0.78f;  // spacing liter - lekko scisniety

    float topY = 35.0f;
    float padL = 30.0f;
    float padR = 20.0f;
    float bottomY = (float)screenH - YELLOW_H * bottomScale - 4.0f;
    float ly = topY;

    // ===== SCORE - wyrownany do prawej =====
    float scoreRightEdge = padL + MeasureMetalNumber(7, metalScale);

    char scoreBuf[16];
    std::snprintf(scoreBuf, sizeof(scoreBuf), "%d", score);
    int   scoreDigits = (int)strlen(scoreBuf);
    float scoreW = MeasureMetalNumber(scoreDigits, metalScale);
    float scoreX = scoreRightEdge - scoreW;

    DrawMetalNumber(score, 0, { scoreX, topY }, metalScale);

    // ===== 3UP =====
    DrawYellowText("3UP", { padL + 10.0f, ly + 30.0f }, 1.3f, sp);

    // ===== RAMKA (ARMS / BOMB) =====
    // Padding wewnetrzny - rowny z obu stron
    float innerPad = 10.0f;
    float sectionGap = 10.0f;  // odstep miedzy sekcjami ARMS i BOMB

    float armsW = MeasureYellowText("ARMS", labelScale, sp);
    float bombW = MeasureYellowText("BOMB", labelScale, sp);

    float contentW = armsW + sectionGap + bombW;
    float totalH = YELLOW_H * labelScale * 2 + 2.0f;

    float frameX = scoreRightEdge + 4.0f;
    float frameW = innerPad + contentW + innerPad;

    int left = (int)(frameX);
    int right = (int)(frameX + frameW);
    int top = (int)(ly - 2);
    int bottom = (int)(ly + totalH + 2);

    // ramka
    DrawRectangle(left, top, right - left, 3, WHITE);
    DrawRectangle(left, top, 3, bottom - top, WHITE);
    DrawRectangle(left, bottom - 3, right - left, 3, GRAY);
    DrawRectangle(right - 3, top, 3, bottom - top, GRAY);

    // content od innerPad po lewej
    float armsX = frameX + innerPad - 6.0f;
    float ammoY = ly + YELLOW_H * labelScale + 2.0f;

    DrawYellowText("ARMS", { armsX, ly }, labelScale, sp);

    float infW = MeasureYellowText("INF", labelScale, sp);
    DrawYellowText("INF", { armsX + (armsW - infW) / 2.0f, ammoY }, labelScale, sp);

    float bombX = armsX + armsW + sectionGap;
    DrawYellowText("BOMB", { bombX, ly }, labelScale, sp);

    float bombNumScale = metalScale * 0.65f;
    float numW = MeasureMetalNumber(2, bombNumScale);
    DrawMetalNumber(10, 2, { bombX + (bombW - numW) / 2.0f, ammoY }, bombNumScale);

    // ===== TIMER =====
    char timerStr[8];
    std::snprintf(timerStr, sizeof(timerStr), "%d", timeLeft);

    float timerW = MeasureTimeString(timerStr, timerScale);
    float x = screenW / 2.0f - timerW / 2.0f;

    for (int i = 0; timerStr[i]; ++i)
    {
        int       col = timerStr[i] - '0';
        Rectangle src = { col * TIME_W, 0, TIME_W, TIME_H };
        Rectangle dst = { x, topY, TIME_W * timerScale, TIME_H * timerScale };
        DrawTexturePro(texTimeNumbers, src, dst, { 0,0 }, 0, WHITE);
        x += TIME_W * timerScale;
    }

    // ===== LEVEL - srodek dolu =====
    char levelText[16];
    std::snprintf(levelText, sizeof(levelText), "LEVEL-%d", level);

    float levelW = MeasureYellowText(levelText, bottomScale, sp);
    DrawYellowText(levelText, { (float)screenW / 2.0f - levelW / 2.0f, bottomY }, bottomScale, sp);

    // ===== CREDIT - prawy dol =====
    char credStr[8];
    std::snprintf(credStr, sizeof(credStr), "%02d", credits);

    float creditLabelW = MeasureYellowText("CREDIT", bottomScale, sp);
    float creditNumW = MeasureYellowText(credStr, bottomScale, sp);
    float cx2 = (float)screenW - creditLabelW - 8.0f - creditNumW - padR;

    DrawYellowText("CREDIT", { cx2, bottomY }, bottomScale, sp);
    DrawYellowText(credStr, { cx2 + creditLabelW + 8.0f, bottomY }, bottomScale, sp);

    DrawMissionIntro();
}

// ============================================================
//  SETTERS / GETTERS
// ============================================================
void UiManager::SetCredits(int amount)
{
    credits += amount;
    if (credits > 99) credits = 99;
    if (credits < 0) credits = 0;
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

// ============================================================
//  MISSION INTRO
// ============================================================
void UiManager::DrawMissionIntro()
{
    if (IsMissionIntroOver()) return;
    if (!blinkVisible)        return;

    int   screenW = GetScreenWidth();
    int   screenH = GetScreenHeight();
    float scale = 2.0f;
    float sp = 0.78f;

    char text[32];
    std::snprintf(text, sizeof(text), "MISSION %d", level);

    float w = MeasureYellowText(text, scale, sp);
    DrawYellowText(text, { screenW / 2.0f - w / 2.0f, screenH / 2.0f }, scale, sp);
}