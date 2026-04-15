#include "UiManager.hpp"
#include <cstdio>

static const float TICK_INTERVAL = 5.0f;
const float UiManager::INTRO_DURATION = 3.0f;
const float UiManager::BLINK_INTERVAL = 0.25f;  // co ile sekund zmienia widocznosc

UiManager::UiManager()
{
    credits = 0;
    score = 0;
    level = 1;
    timeLeft = 60;
    timeAccum = 0.0f;
    introTimer = 0.0f;
    blinkAccum = 0.0f;
    blinkVisible = true;
}

UiManager::~UiManager() {}

void UiManager::Update()
{
    if (introTimer < INTRO_DURATION)
    {
        introTimer += GetFrameTime();

        // miganie tylko podczas intro
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

void UiManager::DrawMissionIntro()
{
    if (IsMissionIntroOver()) return;
    if (!blinkVisible) return;      // miganie - co druga klatka nie rysuj

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    char missionText[32];
    std::snprintf(missionText, sizeof(missionText), "MISSION %d", level);

    int bigSize = 72;
    int mw = MeasureText(missionText, bigSize);
    int mx = screenW / 2 - mw / 2;
    int my = screenH / 2 - bigSize;

    DrawText(missionText, mx + 3, my + 3, bigSize, BLACK);
    DrawText(missionText, mx, my, bigSize, WHITE);

    char startText[] = "Start!";
    int smallSize = 36;
    int sw = MeasureText(startText, smallSize);
    int sx = screenW / 2 - sw / 2;
    int sy = my + bigSize + 16;

    DrawText(startText, sx + 2, sy + 2, smallSize, BLACK);
    DrawText(startText, sx, sy, smallSize, YELLOW);
}

void UiManager::DrawCredits(Camera2D camera)
{
    int fontSize = 32;
    int labelSize = 24;
    int padding = 16;
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // offset od krawedzi - taki sam dla SCORE i CREDITS
    char scoreValue[32];
    std::snprintf(scoreValue, sizeof(scoreValue), "%07d", score);
    int scoreBlockW = MeasureText(scoreValue, fontSize);
    int offsetFromEdge = screenW / 4 - scoreBlockW / 2;

    // --- SCORE - lewy gorny ---
    char scoreLabel[] = "SCORE";
    int sx = offsetFromEdge;
    int sy = padding;

    DrawText(scoreLabel, sx + 2, sy + 2, labelSize, BLACK);
    DrawText(scoreLabel, sx, sy, labelSize, YELLOW);
    DrawText(scoreValue, sx + 2, sy + labelSize + 4 + 2, fontSize, BLACK);
    DrawText(scoreValue, sx, sy + labelSize + 4, fontSize, WHITE);

    // --- AMMO - miedzy SCORE a TIME ---
    int ammoX = screenW / 4 + screenW / 8 - MeasureText("AMMO", labelSize) / 2;
    int ammoY = padding;

    char ammoLabel[] = "AMMO";
    char ammoSymbol[] = "\xe2\x88\x9e";

    DrawText(ammoLabel, ammoX + 2, ammoY + 2, labelSize, BLACK);
    DrawText(ammoLabel, ammoX, ammoY, labelSize, YELLOW);
    DrawText(ammoSymbol, ammoX + 2, ammoY + labelSize + 4 + 2, fontSize, BLACK);
    DrawText(ammoSymbol, ammoX, ammoY + labelSize + 4, fontSize, WHITE);

    // --- TIMER - srodek na gorze ---
    int timerSize = 56;

    char timerText[8];
    std::snprintf(timerText, sizeof(timerText), "%d", timeLeft);
    Color timerColor = (timeLeft <= 10) ? RED : WHITE;

    int tw = MeasureText(timerText, timerSize);
    int tx = screenW / 2 - tw / 2;
    int ty = padding;

    char timeLabel[] = "TIME";
    int  tlw = MeasureText(timeLabel, labelSize);
    int  tlx = screenW / 2 - tlw / 2;

    DrawText(timeLabel, tlx + 2, ty + 2, labelSize, BLACK);
    DrawText(timeLabel, tlx, ty, labelSize, YELLOW);
    DrawText(timerText, tx + 2, ty + labelSize + 4 + 2, timerSize, BLACK);
    DrawText(timerText, tx, ty + labelSize + 4, timerSize, timerColor);

    // --- CREDITS - dol po prawej, symetrycznie do SCORE ---
    char creditsLabel[] = "CREDITS";
    char creditsValue[8];
    std::snprintf(creditsValue, sizeof(creditsValue), "%02d", credits);

    int creditsValueW = MeasureText(creditsValue, fontSize);
    int cx = screenW - offsetFromEdge - creditsValueW;
    int cy = screenH - labelSize - fontSize - padding - 4;

    DrawText(creditsLabel, cx + 2, cy + 2, labelSize, BLACK);
    DrawText(creditsLabel, cx, cy, labelSize, YELLOW);
    DrawText(creditsValue, cx + 2, cy + labelSize + 4 + 2, fontSize, BLACK);
    DrawText(creditsValue, cx, cy + labelSize + 4, fontSize, WHITE);

    // --- LEVEL - dol na srodku ---
    char levelText[32];
    std::snprintf(levelText, sizeof(levelText), "LEVEL %d", level);

    int lw = MeasureText(levelText, fontSize);
    int lx = screenW / 2 - lw / 2;
    int ly = screenH - fontSize - padding;

    DrawText(levelText, lx + 2, ly + 2, fontSize, BLACK);
    DrawText(levelText, lx, ly, fontSize, WHITE);

    // --- MISSION INTRO ---
    DrawMissionIntro();
}

void UiManager::SetCredits(int amount)
{
    credits += amount;
    if (credits > 99) credits = 99;
    if (credits < 0) credits = 0;
}

int UiManager::GetCredits() const { return credits; }

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