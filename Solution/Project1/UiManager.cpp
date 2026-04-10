#include "UiManager.hpp"
#include <cstdio>

static const float TICK_INTERVAL = 5.0f;

UiManager::UiManager()
{
    credits = 0;
    score = 0;
    level = 1;
    timeLeft = 60;
    timeAccum = 0.0f;
}

UiManager::~UiManager() {}

void UiManager::Update()
{
    if (timeLeft <= 0) return;

    timeAccum += GetFrameTime();

    if (timeAccum >= TICK_INTERVAL)
    {
        timeAccum -= TICK_INTERVAL;
        timeLeft--;
        if (timeLeft < 0) timeLeft = 0;
    }
}

void UiManager::DrawCredits(Camera2D camera)
{
    int fontSize = 32;
    int padding = 16;
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // --- CREDITS - prawy dolny rog ---
    char creditsText[32];
    std::snprintf(creditsText, sizeof(creditsText), "CREDITS %02d", credits);

    int cw = MeasureText(creditsText, fontSize);
    int cx = screenW - cw - padding;
    int cy = screenH - fontSize - padding;

    DrawText(creditsText, cx + 2, cy + 2, fontSize, BLACK);
    DrawText(creditsText, cx, cy, fontSize, WHITE);

    // --- SCORE - lewy gorny, ~1/4 szerokosci ---
    char scoreLabel[] = "SCORE";
    char scoreValue[32];
    std::snprintf(scoreValue, sizeof(scoreValue), "%07d", score);

    int labelSize = 24;
    int valueSize = 32;

    int sx = screenW / 4 - MeasureText(scoreValue, valueSize) / 2;
    int sy = padding;

    DrawText(scoreLabel, sx + 2, sy + 2, labelSize, BLACK);
    DrawText(scoreLabel, sx, sy, labelSize, YELLOW);
    DrawText(scoreValue, sx + 2, sy + labelSize + 4 + 2, valueSize, BLACK);
    DrawText(scoreValue, sx, sy + labelSize + 4, valueSize, WHITE);

    // --- TIMER - srodek na gorze ---
    int timerSize = 56;

    char timerText[8];
    std::snprintf(timerText, sizeof(timerText), "%d", timeLeft);

    Color timerColor = (timeLeft <= 10) ? RED : WHITE;

    int tw = MeasureText(timerText, timerSize);
    int tx = screenW / 2 - tw / 2;
    int ty = padding;

    char timeLabel[] = "TIME";
    int timeLabelSize = 24;
    int tlw = MeasureText(timeLabel, timeLabelSize);
    int tlx = screenW / 2 - tlw / 2;

    DrawText(timeLabel, tlx + 2, ty + 2, timeLabelSize, BLACK);
    DrawText(timeLabel, tlx, ty, timeLabelSize, YELLOW);
    DrawText(timerText, tx + 2, ty + timeLabelSize + 4 + 2, timerSize, BLACK);
    DrawText(timerText, tx, ty + timeLabelSize + 4, timerSize, timerColor);

    // --- LEVEL - dol na srodku ---
    char levelText[32];
    std::snprintf(levelText, sizeof(levelText), "LEVEL %d", level);

    int lw = MeasureText(levelText, fontSize);
    int lx = screenW / 2 - lw / 2;
    int ly = screenH - fontSize - padding;

    DrawText(levelText, lx + 2, ly + 2, fontSize, BLACK);
    DrawText(levelText, lx, ly, fontSize, WHITE);
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
}

int UiManager::GetLevel() const { return level; }