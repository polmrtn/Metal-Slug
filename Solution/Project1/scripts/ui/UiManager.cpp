#include "UiManager.hpp"
#include "GlobalManagers.hpp"
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
static const char* PATH_HIGHSCORE_YELLOW = "Graphics/new fonts and HUDs/highscorefontyellow.png";
static const char* PATH_GO_ANIM = "Graphics/new fonts and HUDs/goanimation32x31.png";
static const char* PATH_FONTBOMB = "Graphics/new fonts and HUDs/numbers01.png";
static const char* PATH_ENDING = "Graphics/screens/ending/ending.png";
static const char* PATH_ENDING_STARS = "Graphics/screens/ending/stars.png";
static const char* PATH_TIME_NUM = "Graphics/letters/time_numbers.png";
static const char* PATH_BLUE_LETTERS = "Graphics/letters/blue_numbers_and_letters.png";
static const char* PATH_YELLOW_LETTERS = "Graphics/letters/yellow_numbers_and_letters.png";
static const char* PATH_METAL_BIGNUM = "Graphics/letters/metal_numbers.png";
static const char* PATH_GAMEOVER = "Graphics/screens/gameover1.png";

static constexpr float NUM_CHAR_W = 12.0f;
static constexpr float NUM_CHAR_H = 14.0f;
static constexpr float HSF_CHAR_W = 16.0f;
static constexpr float HSF_CHAR_H = 16.0f;
static constexpr float BIG_CHAR_W = 8.0f;
static constexpr float BIG_CHAR_H = 16.0f;

static int MissionCharFrame(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a';
    if (c >= '1' && c <= '9') return 26 + (c - '1');
    if (c == '!') return 35;
    if (c == '?') return 36;
    return -1;
}

static void DrawMissionChar(Texture2D tex, char c, float x, float y, float scale) {
    int frame = MissionCharFrame(c);
    if (frame < 0) return;
    Rectangle src = { frame * 32.0f, 0, 32.0f, 32.0f };
    Rectangle dst = { x, y, 32.0f * scale, 32.0f * scale };
    DrawTexturePro(tex, src, dst, { 0,0 }, 0, WHITE);
}

UiManager::UiManager()
    : credits(0), score(0), level(1), timeLeft(60), bombs(10), ammo(0),
    timeAccum(0.0f), introTimer(0.0f),
    blinkAccum(0.0f), blinkVisible(true),
    idleTimer(0.0f), goVisible(false), goBlinkAccum(0.0f), goBlinkOn(false),
    initialized(false) // now valid, as 'initialized' is a member
{ }

void UiManager::Init() {
    if (initialized) {
        TraceLog(LOG_INFO, "UiManager::Init() called but already initialized.");
        return;
    }

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
    texGameOver    = LoadTexture(PATH_GAMEOVER);
    texHudFont3Num = LoadTexture(PATH_FONTBOMB);
    texBlueLetters = LoadTexture(PATH_BLUE_LETTERS);
    texYellowLetters = LoadTexture(PATH_YELLOW_LETTERS);
    texEnding = LoadTexture(PATH_ENDING);
    texStars = LoadTexture(PATH_ENDING_STARS);
    texHighScoreYellow = LoadTexture(PATH_HIGHSCORE_YELLOW);
    texGoAnim = LoadTexture(PATH_GO_ANIM);

    SetTextureFilter(texGoAnim, TEXTURE_FILTER_POINT);
    SetTextureFilter(texGameOver, TEXTURE_FILTER_POINT);
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
    SetTextureFilter(texHudFont3Num, TEXTURE_FILTER_POINT);
    SetTextureFilter(texBlueLetters, TEXTURE_FILTER_POINT);
    SetTextureFilter(texYellowLetters, TEXTURE_FILTER_POINT);
    SetTextureFilter(texEnding, TEXTURE_FILTER_POINT);
    SetTextureFilter(texStars, TEXTURE_FILTER_POINT);
    SetTextureFilter(texHighScoreYellow, TEXTURE_FILTER_POINT);

    initialized = true;
    TraceLog(LOG_INFO, "UiManager initialized: textures loaded.");
}

UiManager::~UiManager()
{
    UnloadTexture(texArms);   UnloadTexture(texBomb);
    UnloadTexture(texCannon); UnloadTexture(texTimeLevel);
    UnloadTexture(texHudFont2Big); UnloadTexture(texHudFont2Num); UnloadTexture(texHudFont3Num);
    UnloadTexture(texHudFont2Small); UnloadTexture(texHighScore); 
    UnloadTexture(texHighScoreSmall);
    UnloadTexture(texHpBarLeft); UnloadTexture(texHpBarRight); UnloadTexture(texHpBarParts); 
    UnloadTexture(texGo);
    UnloadTexture(texTimeNum);
    UnloadTexture(texMetalBigNum);
    UnloadTexture(texGameOver);
    UnloadTexture(texBlueLetters);
    UnloadTexture(texYellowLetters);
    UnloadTexture(texEnding);
    UnloadTexture(texStars);
    UnloadTexture(texHighScoreYellow);
    UnloadTexture(texGoAnim);
}

void UiManager::Update()
{
    float dt = GetFrameTime();
    UpdateMissionIntro(dt);
    UpdateEnding(dt);
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

    float scale = 2.0f;
    char label[16] = "CREDIT ";
    char number[8];
    std::snprintf(number, sizeof(number), "%02d", credits);

    float labelW = MeasureScoreText(label, scale);
    float numW   = MeasureScoreText(number, scale);
    float totalW = labelW + numW;

    float x = (float)SW - totalW - 110.0f;
    float y = (float)SH - HSF_CHAR_H * scale - 2.0f;

    DrawScoreText(label,  { x, y }, scale);
    DrawScoreText(number, { x + labelW + 40.0f, y }, scale);

    // LEVEL napis na dole srodek (TITLE zawsze pokazuje LEVEL 4)
    char lvlText[16]; std::snprintf(lvlText, sizeof(lvlText), "LEVEL-%d", 4);
    float lSc = 2.0f;
    float lW  = MeasureScoreText(lvlText, lSc);
    float lX  = (float)SW * 0.5f - lW * 0.5f;
    float lY  = (float)SH - HSF_CHAR_H * lSc - 2.0f;
    DrawScoreText(lvlText, { lX, lY }, lSc);
}

void UiManager::UpdateGoTimer(float dt)
{
    idleTimer += dt;
    if (idleTimer < IDLE_THRESHOLD) {
        goAnimActive = false;
        goAnimFrame = 0;
        goAnimCycle = 0;
        goAnimPausing = false;
        goIdleTimer = 0.0f;
        return;
    }

    // Ciclo de 5 segundos entre pares
    goIdleTimer += dt;
    if (!goAnimActive && goIdleTimer >= GO_IDLE_INTERVAL) {
        goIdleTimer = 0.0f;
        goAnimActive = true;
        goAnimFrame = 0;
        goAnimCycle = 0;
        goAnimTimer = 0.0f;
        goAnimPausing = false;
        audioManager.PlaySound(audioManager.GetGoSignSound());
    }

    if (!goAnimActive) return;

    if (goAnimPausing) {
        goAnimPauseTimer += dt;
        if (goAnimPauseTimer >= GO_PAUSE) {
            goAnimPausing = false;
            goAnimPauseTimer = 0.0f;
            goAnimFrame = 0;
            goAnimCycle++;
            if (goAnimCycle >= 2) {
                goAnimActive = false;
                goAnimCycle = 0;
            } else {
                audioManager.PlaySound(audioManager.GetGoSignSound());
            }
        }
        return;
    }

    goAnimTimer += dt;
    if (goAnimTimer >= goAnimDelay) {
        goAnimTimer = 0.0f;
        goAnimFrame++;
        if (goAnimFrame >= GO_ANIM_FRAMES) {
            goAnimFrame = GO_ANIM_FRAMES - 1;
            goAnimPausing = true;
            goAnimPauseTimer = 0.0f;
        }
    }
}

void UiManager::NotifyPlayerMoved() {
    idleTimer = 0.0f;
    goVisible = false;
    goBlinkOn = false;
    goBlinkAccum = 0.0f;
    goAnimActive = false;   // ← añade
    goAnimFrame = 0;        // ← añade
    goAnimCycle = 0;        // ← añade
    goIdleTimer = 0.0f;     // ← añade
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
    static constexpr float PART_W = 8.0f;
    static constexpr float PART_H = 8.0f;
    static constexpr float CAP_L_W = 2.0f;  // left es 2px
    static constexpr float CAP_R_W = 3.0f;  // right es 3px
    static constexpr float CAP_H = 8.0f;

    // Left cap (2x8)
    Rectangle srcL = { 0, 0, CAP_L_W, CAP_H };
    Rectangle dstL = { pos.x, pos.y, CAP_L_W * scale, CAP_H * scale };
    DrawTexturePro(texHpBarLeft, srcL, dstL, { 0,0 }, 0, WHITE);

    float offsetX = CAP_L_W * scale;

    // Segmentos (8x8 cada uno)
    for (int i = 0; i < segs; ++i)
    {
        int frame = 0;
        if (jetpackActive)
        {
            float segMin = (float)i / (float)segs;
            float segMax = (float)(i + 1) / (float)segs;

            if (jetpackFuelRatio >= segMax)
                frame = 8;
            else if (jetpackFuelRatio <= segMin)
                frame = 0;
            else
            {
                float t = (jetpackFuelRatio - segMin) / (segMax - segMin);
                frame = (int)(t * 8.0f);
            }
        }

        Rectangle srcP = { frame * PART_W, 0, PART_W, PART_H };
        Rectangle dstP = { pos.x + offsetX, pos.y, PART_W * scale, PART_H * scale };
        DrawTexturePro(texHpBarParts, srcP, dstP, { 0,0 }, 0, WHITE);
        offsetX += PART_W * scale;
    }

    // Right cap (3x8)
    Rectangle srcR = { 0, 0, CAP_R_W, CAP_H };
    Rectangle dstR = { pos.x + offsetX, pos.y, CAP_R_W * scale, CAP_H * scale };
    DrawTexturePro(texHpBarRight, srcR, dstR, { 0,0 }, 0, WHITE);
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
            // "CONTINUE X" — ta sama czcionka, cyfra bez animacji
            int contDigit = 9 - (int)(continueElapsed / 2.0f);
            if (contDigit < 0) contDigit = 0;
            char contLabel[16];
            std::snprintf(contLabel, sizeof(contLabel), "CONTINUE %d", contDigit);

            const float lblScY = 4.0f;
            const float areaW  = startXTime * 0.55f - 18.0f;
            const float baseW  = MeasureScoreText(contLabel, 1.0f);
            const float lblScX = (baseW > 0.0f && areaW > 0.0f)
                                 ? std::min(2.5f, areaW / baseW) : 2.5f;
            float x = 50.0f;
            for (int i = 0; contLabel[i]; ++i) {
                char c = contLabel[i];
                if (c == ' ') { x += HSF_CHAR_W * lblScX * 0.5f; continue; }
                int col = -1;
                if (c >= 'A' && c <= 'Z')      col = c - 'A';
                else if (c >= 'a' && c <= 'z') col = c - 'a';
                else if (c >= '0' && c <= '9') col = 26 + (c - '0');
                else if (c == '-')             col = 36;
                else if (c == '?')             col = 37;
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
            float num01Sc = 3.0f;
            float ax = armsCenterX - (3 * 8.0f * num01Sc) / 2.0f;
            for (int i = 0; ammoBuf[i]; ++i)
            {
                if (ammoBuf[i] < '0' || ammoBuf[i] > '9') { ax += 8.0f * num01Sc; continue; }
                int d = ammoBuf[i] - '0';
                Rectangle src = { d * 8.0f, 0, 8.0f, 8.0f };
                Rectangle dst = { ax, innerNumY, 8.0f * num01Sc, 8.0f * num01Sc };
                DrawTexturePro(texHudFont3Num, src, dst, { 0,0 }, 0, WHITE);
                ax += 8.0f * num01Sc;
            }
        }
        else
        {
            float num01Sc = 3.0f;
            float ax = armsCenterX - (3 * 8.0f * num01Sc) / 2.0f;
            for (int col : {10, 11, 12})
            {
                Rectangle src = { col * 8.0f, 0, 8.0f, 8.0f };
                Rectangle dst = { ax, innerNumY, 8.0f * num01Sc, 8.0f * num01Sc };
                DrawTexturePro(texHudFont3Num, src, dst, { 0,0 }, 0, WHITE);
                ax += 8.0f * num01Sc;
            }
        }

        char bombBuf[8];
        std::snprintf(bombBuf, sizeof(bombBuf), "%02d", bombs);
        float num01Sc = 3.0f;
        float bx = bombCenterX - (2 * 8.0f * num01Sc) / 2.0f - 10.0f; 
        for (int i = 0; bombBuf[i]; ++i)
        {
            if (bombBuf[i] < '0' || bombBuf[i] > '9') { bx += 8.0f * num01Sc; continue; }
            int d = bombBuf[i] - '0';
            Rectangle src = { d * 8.0f, 0, 8.0f, 8.0f };
            Rectangle dst = { bx, innerNumY, 8.0f * num01Sc, 8.0f * num01Sc };
            DrawTexturePro(texHudFont3Num, src, dst, { 0,0 }, 0, WHITE);
            bx += 8.0f * num01Sc;
        }

        DrawScoreText(sBuf, { scoreX, hudY }, scoreSc);

        float barScale = 4.0f;
        float barY = hudY + HSF_CHAR_H * scoreSc + 4.0f;
        float barTotalW = (2.0f + 6.0f * 8.0f + 2.0f) * barScale;
        float barX = scoreX + scoreW - barTotalW;  

        DrawHpBar({ barX, barY }, 6, barScale);

        continueLabelX = scoreX;
        continueLabelY = hudY;
    }

    // --- DOLNY HUD ---
    char lvlText[16]; std::snprintf(lvlText, sizeof(lvlText), "LEVEL-%d", level);
    float lSc = 2.0f;
    float lW = MeasureScoreText(lvlText, lSc);
    float lX = (float)SW * 0.5f - lW * 0.5f;
    float lY = (float)SH - HSF_CHAR_H * lSc - 2.0f;
    DrawScoreText(lvlText, { lX, lY }, lSc);

    float cSc = 2.0f;
    char cLabel[16] = "CREDIT ";
    char cNumber[8];
    std::snprintf(cNumber, sizeof(cNumber), "%02d", credits);

    float cLabelW = MeasureScoreText(cLabel, cSc);
    float cNumW   = MeasureScoreText(cNumber, cSc);
    float cTotalW = cLabelW + cNumW;

    float cX = (float)SW - cTotalW - 110.0f;
    float cY = (float)SH - HSF_CHAR_H * cSc - 2.0f;

    DrawScoreText(cLabel,  { cX, cY }, cSc);
    DrawScoreText(cNumber, { cX + cLabelW + 40.0f, cY }, cSc);

    if (goAnimActive && !goAnimPausing) {
        // No mostrar cerca del boss
        if (player.GetX() < 14000.0f) {
            const float goSc = 4.5f;
            float goW = GO_ANIM_W * goSc;
            float goX = (float)SW * 0.80f - goW * 0.5f;
            float goY = hudY + 120.0f;
            Rectangle src = { goAnimFrame * GO_ANIM_W, 0, GO_ANIM_W, GO_ANIM_H - 1.0f};
            Rectangle dst = { goX, goY, GO_ANIM_W * goSc, GO_ANIM_H * goSc };
            DrawTexturePro(texGoAnim, src, dst, { 0,0 }, 0, WHITE);
        }
    }

    DrawMissionIntroInternal();
    DrawEnding();
}

void UiManager::DrawMissionIntro() { DrawMissionIntroInternal(); }

void UiManager::DrawMissionIntroInternal() {
    if (!missionActive && exitFadeAlpha < 1.0f) return;

    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    // Pantalla negra después del fadeout completo
    if (!missionActive && exitFadeAlpha >= 1.0f) {
        DrawRectangle(0, 0, SW, SH, BLACK);
        return;
    }

    // Fade in solo para mission intro, no para complete
    static constexpr float FADE_DURATION = 1.0f;
    if (!missionCompleteActive && missionTimer < FADE_DURATION) {
        float alpha = missionTimer / FADE_DURATION;
        unsigned char a = (unsigned char)((1.0f - alpha) * 255.0f);
        DrawRectangle(0, 0, SW, SH, { 0, 0, 0, a });
    }

    for (int i = 0; i < missionLettersSpawned; ++i) {
        auto& l = missionLetters[i];

        if (l.isStart && l.arrived && !exitPhase) {
            bool allDone = (missionLettersSpawned == missionLetterCount);
            if (allDone)
                for (int j = 0; j < missionLetterCount; ++j)
                    if (!missionLetters[j].isYellow) { allDone = false; break; }
            if (allDone && !startBlinkVisible) continue;
        }

        Texture2D& tex = (l.flipProgress >= 0.5f) ? texYellowLetters : texBlueLetters;

        float scaleX;
        if (l.flipProgress < 0.5f)
            scaleX = 1.0f - (l.flipProgress / 0.5f);
        else
            scaleX = (l.flipProgress - 0.5f) / 0.5f;

        float charW = 32.0f * MISSION_SCALE * scaleX;
        float charH = 32.0f * MISSION_SCALE;
        float offsetX = 32.0f * MISSION_SCALE * (1.0f - scaleX) / 2.0f;

        if (charW > 0.5f) {
            int frame = MissionCharFrame(l.c);
            if (frame >= 0) {
                Rectangle src = { frame * 32.0f, 0, 32.0f, 32.0f };
                Rectangle dst = { l.currentX + offsetX, l.currentY, charW, charH };
                DrawTexturePro(tex, src, dst, { 0,0 }, 0, WHITE);
            }
        }
    }

    // Fade out durante la salida lenta
    if (exitPhase && slowExit && exitFadeAlpha > 0.0f) {
        unsigned char a = (unsigned char)(exitFadeAlpha * 255.0f);
        DrawRectangle(0, 0, SW, SH, { 0, 0, 0, a });
    }
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
    if (value <= 10)
    {
        float blinkRate = 0.5f;  
        if ((int)(GetTime() / blinkRate) % 2 == 0) return;
    }

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
    continueScreenActive = false;
    continueDelayActive  = true;
    continueDelay        = 0.0f;
    lastContinueDigit    = -1;
}

void UiManager::StopContinue()
{
    continueScreenActive = false;
    timeLeft = 60;
    timeAccum = 0.0f;
}

void UiManager::UpdateContinue(float dt)
{
    if (continueDelayActive)
    {
        continueDelay += dt;
        if (continueDelay >= CONTINUE_DELAY)
        {
            continueDelayActive = false;
            continueScreenActive = true;  // ← teraz sí activa
        }
        return;
    }

    continueElapsed += dt;
    continueBlinkAccum += dt;
    if (continueBlinkAccum >= 0.25f) {
        continueBlinkAccum -= 0.25f;
        continueBlinkOn = !continueBlinkOn;
    }

    // Dzwiek przy zmianie cyfry odliczania
    int currentDigit = 9 - (int)(continueElapsed / 2.0f);
    if (currentDigit < 0) currentDigit = 0;
    if (currentDigit != lastContinueDigit) {
        lastContinueDigit = currentDigit;
        audioManager.PlaySound(audioManager.GetDeathCountdownSound());
    }
}

bool UiManager::IsContinueOver() const
{
    return continueElapsed >= 20.0f;
}

void UiManager::DrawContinueScreen()
{
    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    // Aktualnie wyswietlana cyfra (9 -> 0)
    int currentDigit = 9 - (int)(continueElapsed / 2.0f);
    if (currentDigit < 0) currentDigit = 0;

    // Animacja "obrotu" — skalowanie w osi X
    // sprite: "1234567890" => index cyfry d: d==0 -> 9, else d-1
    float flipPhase = std::fmod(continueElapsed / 2.0f, 1.0f);
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

    // "CONTINUE?" — CONTINUE z highscorefont, ? z highscorefontsmall
    const char* contText = "CONTINUE";
    const float textSc   = 4.0f;
    const float textY    = (float)SH * 0.25f;

    // '?' z hudfont2numbers.png — 14 znakow: 0-9 (10) + - / ? : (4)
    // Obliczamy szerokosc slotu z rzeczywistej szerokosci tekstury / 14
    const int   NUM2_TOTAL_CHARS = 14;
    const float Q_SLOT_W = (float)texHudFont2Num.width / (float)NUM2_TOTAL_CHARS;
    const float Q_SLOT_H = (float)texHudFont2Num.height;
    const float Q_SRC_X  = 12.0f * Q_SLOT_W;   // slot 12 = '?'  (0-9=sloty 0-9, -=10, /=11, ?=12)
    const float Q_DST_H  = HSF_CHAR_H * textSc;
    const float Q_DST_W  = Q_SLOT_W * (Q_DST_H / Q_SLOT_H);

    float continueW = MeasureScoreText(contText, textSc);
    float totalW    = continueW + Q_DST_W;
    float startX    = (float)SW * 0.5f - totalW * 0.5f;

    DrawScoreText(contText, { startX, textY }, textSc);

    Rectangle qSrc = { Q_SRC_X, 0.0f, Q_SLOT_W, Q_SLOT_H };
    Rectangle qDst = { startX + continueW, textY, Q_DST_W, Q_DST_H };
    DrawTexturePro(texHudFont2Num, qSrc, qDst, { 0.0f, 0.0f }, 0.0f, WHITE);

    // Liczba kreditow (dolny prawy rog)
    DrawCreditsOnly();
}

// ─────────────────────────────────────────
//  Game Over sequence
// ─────────────────────────────────────────

// Faza 1 (t 0-3s): czerwony filtr narasta
// Faza 2 (t 3-4s): ekran gaśnie do czerni (czarny overlay na wierzchu)
void UiManager::DrawGameOverOverlay(float t)
{
    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    if (t < 3.0f)
    {
        unsigned char alpha = (unsigned char)((t / 3.0f) * 180.0f);
        DrawRectangle(0, 0, SW, SH, { 180, 0, 0, alpha });
    }
    else
    {
        // czerwony pelny + czarny narasta
        DrawRectangle(0, 0, SW, SH, { 180, 0, 0, 180 });
        float progress = (t - 3.0f) / 1.0f;
        if (progress > 1.0f) progress = 1.0f;
        unsigned char blackAlpha = (unsigned char)(progress * 255.0f);
        DrawRectangle(0, 0, SW, SH, { 0, 0, 0, blackAlpha });
    }
}

// Faza 3 (t 0-5s): sprite game over na czarnym tle, fade-in przez 0.5s
void UiManager::DrawGameOverSprite(float t)
{
    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    DrawRectangle(0, 0, SW, SH, BLACK);

    // fade-in 0-0.5s, pelny 0.5-4s, fade-out 4-5s
    float alpha01;
    if (t < 0.5f)       alpha01 = t / 0.5f;
    else if (t < 4.0f)  alpha01 = 1.0f;
    else                alpha01 = 1.0f - (t - 4.0f) / 1.0f;
    if (alpha01 < 0.0f) alpha01 = 0.0f;
    unsigned char alpha = (unsigned char)(alpha01 * 255.0f);

    Rectangle src = { 0.0f, 0.0f, (float)texGameOver.width, (float)texGameOver.height };
    Rectangle dst = { 0.0f, 0.0f, (float)SW, (float)SH };
    DrawTexturePro(texGameOver, src, dst, { 0.0f, 0.0f }, 0.0f, { 255, 255, 255, alpha });
}

void UiManager::DrawInsertCoin(float y, float scale) const
{
    if ((int)(GetTime() * 2.5f) % 2 != 0) return;

    const char* txt = "INSERT COINT!";
    int SW = GetScreenWidth();

    // medir ancho total
    float totalW = 0;
    for (int i = 0; txt[i]; ++i)
        if (txt[i] != '!') totalW += BIG_CHAR_W * scale;

    float x = SW * 0.5f - totalW * 0.5f;
    for (int i = 0; txt[i]; ++i)
    {
        if (txt[i] == ' ') { x += BIG_CHAR_W * scale * 0.5f; continue; }
        if (txt[i] == '!') { x += BIG_CHAR_W * scale; continue; }
        DrawBigLetter(txt[i], { x, y }, scale);
        x += BIG_CHAR_W * scale;
    }
}

void UiManager::DrawFooter(float y, float scale) const
{
    int SW = GetScreenWidth();
    const char* txt = "2026 KURVVA PRODUCTIONS";
    float w = MeasureScoreText(txt, scale);
    DrawScoreText(txt, { SW * 0.5f - w * 0.5f, y }, scale);
}

void UiManager::StartMissionIntro() {
    missionTimer = 0.0f;
    missionActive = true;
    exitPhase = false;
    exitTimer = 0.0f;
    startBlinkTimer = 0.0f;
    startBlinkCount = 0;
    startBlinkVisible = true;
    missionLettersSpawned = 0;
    letterSpawnTimer = 0.0f;
    missionLetterCount = 0;


    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    // ── Fila 1: MISSION 1 ──────────────────────────────────
    char text[16];
    std::snprintf(text, sizeof(text), "MISSION %d", level);

    float totalW = 0;
    for (int i = 0; text[i]; ++i)
        totalW += (text[i] == ' ') ? 32.0f * MISSION_SCALE * 0.5f : 25.0f * MISSION_SCALE;

    float row1X = SW * 0.5f - totalW * 0.5f;
    float row1Y = SH * 0.45f;
    float x = row1X;

    for (int i = 0; text[i] && missionLetterCount < MAX_MISSION_LETTERS; ++i) {
        if (text[i] == ' ') { x += 25.0f * MISSION_SCALE * 0.5f; continue; }
        auto& l = missionLetters[missionLetterCount++];
        l.c = text[i];
        l.targetX = x;
        l.targetY = row1Y;
        l.currentX = -300.0f;
        l.currentY = -200.0f;
        l.arrived = false;
        l.isYellow = false;
        l.flipTimer = 0.0f;
        l.isStart = false;
        x += 25.0f * MISSION_SCALE;
        l.flipProgress = 0.0f;
        l.isYellow = false;
    }

    // ── Fila 2: START! ─────────────────────────────────────
    const char* startTxt = "START!";
    float startTotalW = 6 * 25.0f * MISSION_SCALE;
    float row2X = SW * 0.5f - startTotalW * 0.5f;
    float row2Y = row1Y + 25.0f * MISSION_SCALE + 10.0f;
    x = row2X;

    for (int i = 0; startTxt[i] && missionLetterCount < MAX_MISSION_LETTERS; ++i) {
        auto& l = missionLetters[missionLetterCount++];
        l.c = startTxt[i];
        l.targetX = x;
        l.targetY = row2Y;
        l.currentX = -300.0f;
        l.currentY = -200.0f;
        l.arrived = false;
        l.isYellow = false;
        l.flipTimer = 0.0f;
        l.isStart = true;
        x += 25.0f * MISSION_SCALE;
        l.flipProgress = 0.0f;
        l.isYellow = false;
    }
}

void UiManager::UpdateMissionIntro(float dt) {
    if (!missionActive) return;
    missionTimer += dt;

    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    if (exitPhase) {
        exitTimer += dt;
        float speed = slowExit ? 200.0f * dt : 2000.0f * dt;
        for (int i = 0; i < missionLetterCount; ++i) {
            auto& l = missionLetters[i];
            float cx = SW * 0.5f, cy = SH * 0.5f;
            float dx = l.targetX - cx, dy = l.targetY - cy;
            float len = sqrtf(dx * dx + dy * dy);
            if (len > 0) { dx /= len; dy /= len; }
            l.currentX += dx * speed;
            l.currentY += dy * speed;
        }
        if (slowExit) {
            // Fade zaczyna sie od razu gdy litery sie rozjezdzaja, trwa 4s
            float fadeStart = 0.0f;
            float fadeDuration = 4.0f;
            if (exitTimer > fadeStart)
                exitFadeAlpha = (exitTimer - fadeStart) / fadeDuration;
            if (exitFadeAlpha > 1.0f) exitFadeAlpha = 1.0f;
            if (exitTimer >= fadeStart + fadeDuration) {
                missionActive = false;
                missionCompleteActive = false;
                missionCompleteDone = true;  // game.cpp wykryje i przejdzie do INTRO
            }
        }
        else {
            if (exitTimer >= 0.8f) missionActive = false;
        }
        return;
    }

    // Fase entrada: spawna letras una a una
    float spawnInterval = 0.08f;
    letterSpawnTimer += dt;
    while (missionLettersSpawned < missionLetterCount &&
        letterSpawnTimer >= spawnInterval) {
        letterSpawnTimer -= spawnInterval;
        missionLettersSpawned++;
    }

    // Mover letras hacia su posición
    for (int i = 0; i < missionLettersSpawned; ++i) {
        auto& l = missionLetters[i];
        if (!l.arrived) {
            float speed = 1500.0f * dt;
            float dx = l.targetX - l.currentX;
            float dy = l.targetY - l.currentY;
            float len = sqrtf(dx * dx + dy * dy);
            if (len <= speed) {
                l.currentX = l.targetX;
                l.currentY = l.targetY;
                l.arrived = true;
            }
            else {
                l.currentX += (dx / len) * speed;
                l.currentY += (dy / len) * speed;
            }
        }
        // Flip a amarillo cuando llega
        if (l.arrived && l.flipProgress < 1.0f) {
            l.flipProgress += dt * 3.0f;
            if (l.flipProgress >= 1.0f) {
                l.flipProgress = 1.0f;
                l.isYellow = true;
            }
        }
    }

    // Todas llegaron — fase reposo con parpadeo
    bool allArrived = (missionLettersSpawned == missionLetterCount);
    for (int i = 0; i < missionLetterCount && allArrived; ++i)
        if (!missionLetters[i].isYellow) allArrived = false;

    if (allArrived) {
        startBlinkTimer += dt;
        if (startBlinkTimer >= 0.25f) {
            startBlinkTimer = 0.0f;
            startBlinkVisible = !startBlinkVisible;
            if (!startBlinkVisible) startBlinkCount++;
        }
        if (startBlinkCount >= 3 && !exitPhase) {
            exitPhase = true;
            exitTimer = 0.0f;
        }
    }
}

void UiManager::StartMissionComplete() {
    missionTimer = 0.0f;
    missionActive = true;
    missionCompleteActive = true;
    exitPhase = false;
    exitTimer = 0.0f;
    exitFadeAlpha = 0.0f;
    slowExit = true;
    startBlinkTimer = 0.0f;
    startBlinkCount = 0;
    startBlinkVisible = true;
    missionLettersSpawned = 0;
    letterSpawnTimer = 0.0f;
    missionLetterCount = 0;

    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    // ── Fila 1: MISSION 1 ──────────────────────────────────
    char text[16];
    std::snprintf(text, sizeof(text), "MISSION %d", level);

    float totalW = 0;
    for (int i = 0; text[i]; ++i)
        totalW += (text[i] == ' ') ? 28.0f * MISSION_SCALE * 0.5f : 28.0f * MISSION_SCALE;

    float row1X = SW * 0.5f - totalW * 0.5f;
    float row1Y = SH * 0.45f;
    float x = row1X;

    for (int i = 0; text[i] && missionLetterCount < MAX_MISSION_LETTERS; ++i) {
        if (text[i] == ' ') { x += 28.0f * MISSION_SCALE * 0.5f; continue; }
        auto& l = missionLetters[missionLetterCount++];
        l.c = text[i];
        l.targetX = x;
        l.targetY = row1Y;
        l.currentX = -300.0f;
        l.currentY = -200.0f;
        l.arrived = false;
        l.isYellow = false;
        l.flipTimer = 0.0f;
        l.flipProgress = 0.0f;
        l.isStart = false;
        x += 28.0f * MISSION_SCALE;
    }

    // ── Fila 2: COMPLETE! ──────────────────────────────────
    const char* completeTxt = "COMPLETE!";
    TraceLog(LOG_INFO, "Frame para !: %d", MissionCharFrame('!'));
    int completeLen = 0;
    for (int i = 0; completeTxt[i]; ++i) completeLen++;

    float completeTotalW = completeLen * 28.0f * MISSION_SCALE;
    float row2X = SW * 0.5f - completeTotalW * 0.5f;
    float row2Y = row1Y + 28.0f * MISSION_SCALE + 10.0f;
    x = row2X;

    for (int i = 0; completeTxt[i] && missionLetterCount < MAX_MISSION_LETTERS; ++i) {
        auto& l = missionLetters[missionLetterCount++];
        l.c = completeTxt[i];
        l.targetX = x;
        l.targetY = row2Y;
        l.currentX = -300.0f;
        l.currentY = -200.0f;
        l.arrived = false;
        l.isYellow = false;
        l.flipTimer = 0.0f;
        l.flipProgress = 0.0f;
        l.isStart = true;   // ← usa el mismo flag para parpadeo
        x += 28.0f * MISSION_SCALE;
    }
}

void UiManager::StartEnding() {
    endingActive = true;
    endingTimer = 0.0f;
    endingFadeOut = false;
    endingFadeAlpha = 0.0f;
    endingFinished = false;

    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    for (int i = 0; i < MAX_ENDING_STARS; ++i) {
        auto& s = endingStars[i];
        s.x = (float)(GetRandomValue(0, SW));
        s.y = (float)(GetRandomValue(0, SH));
        s.row = GetRandomValue(0, 1);
        s.frame = 0;
        s.timer = 0.0f;
        s.delay = 0.05f + (float)GetRandomValue(0, 10) * 0.01f;
    }
}
void UiManager::UpdateEnding(float dt) {
    if (!endingActive) return;
    endingTimer += dt;

    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    for (int i = 0; i < MAX_ENDING_STARS; ++i) {
        auto& s = endingStars[i];
        int maxFrames = (s.row == 0) ? 2 : 10;
        s.timer += dt;
        if (s.timer >= s.delay) {
            s.timer = 0.0f;
            s.frame++;
            if (s.frame >= maxFrames) {
                s.frame = 0;
                // ← randomiza posición al completar la animación
                s.x = (float)GetRandomValue(0, SW);
                s.y = (float)GetRandomValue(0, SH);
                s.row = GetRandomValue(0, 1);  // también randomiza fila si quieres
            }
        }
    }
    if (endingFadeOut) {
        endingFadeAlpha += dt * 0.5f;
        if (endingFadeAlpha >= 1.0f) {
            endingFadeAlpha = 1.0f;
            endingFinished = true;
        }
    }
}

void UiManager::DrawEnding() const {
    if (!endingActive && !endingFadeOut) return;

    int SW = GetScreenWidth();
    int SH = GetScreenHeight();

    // Fondo
    Rectangle src = { 0, 0, (float)texEnding.width, (float)texEnding.height };
    Rectangle dst = { 0, 0, (float)SW, (float)SH };
    DrawTexturePro(texEnding, src, dst, { 0,0 }, 0, WHITE);

    // Estrellas
    float starW = 11.0f, starH = 11.0f;
    float starScale = 3.0f;
    for (int i = 0; i < MAX_ENDING_STARS; ++i) {
        auto& s = endingStars[i];
        float rowY = s.row * starH;
        Rectangle starSrc = { s.frame * starW, rowY, starW, starH };
        Rectangle starDst = { s.x, s.y, starW * starScale, starH * starScale };
        DrawTexturePro(texStars, starSrc, starDst, { 0,0 }, 0, WHITE);
    }

    // Fade in: primero 1.5s desde negro
    static constexpr float ENDING_FADE_IN = 1.5f;
    if (endingTimer < ENDING_FADE_IN) {
        float alpha = endingTimer / ENDING_FADE_IN;
        unsigned char a = (unsigned char)((1.0f - alpha) * 255.0f);
        DrawRectangle(0, 0, SW, SH, { 0, 0, 0, a });
    }


    // THE END — centrado un poco arriba del centro
    const char* theEnd = "THE END";
    float theEndScale = endingTextScale;
    float theEndW = MeasureScoreText(theEnd, theEndScale);
    float theEndX = SW * 0.5f - theEndW * 0.5f;
    float theEndY = SH * 0.38f;
    float px = theEndX;
    for (int i = 0; theEnd[i]; ++i) {
        if (theEnd[i] == ' ') { px += HSF_CHAR_W * theEndScale * 0.5f; continue; }
        int col = -1;
        if (theEnd[i] >= 'A' && theEnd[i] <= 'Z') col = theEnd[i] - 'A';
        else if (theEnd[i] >= 'a' && theEnd[i] <= 'z') col = theEnd[i] - 'a';
        if (col < 0) { px += HSF_CHAR_W * theEndScale; continue; }
        Rectangle src = { (float)col * HSF_CHAR_W, 0.0f, HSF_CHAR_W, HSF_CHAR_H };
        Rectangle dst = { px, theEndY, HSF_CHAR_W * theEndScale, HSF_CHAR_H * theEndScale };
        DrawTexturePro(texHighScoreYellow, src, dst, { 0,0 }, 0, WHITE);
        px += HSF_CHAR_W * theEndScale;
    }

    // PRESS ENTER TO CONTINUE — centro bajo
    if ((int)(endingTimer * 2.0f) % 2 == 0) {  // parpadeo
        const char* pressEnter = "PRESS ENTER TO CONTINUE";
        float peScale = 2.0f;
        float peW = 0;
        for (int i = 0; pressEnter[i]; ++i)
            peW += (pressEnter[i] == ' ') ? BIG_CHAR_W * peScale * 0.5f : BIG_CHAR_W * peScale;
        float peX = SW * 0.5f - peW * 0.5f;
        float peY = SH * 0.75f;
        float x = peX;
        for (int i = 0; pressEnter[i]; ++i) {
            if (pressEnter[i] == ' ') { x += BIG_CHAR_W * peScale * 0.5f; continue; }
            DrawBigLetter(pressEnter[i], { x, peY }, peScale);
            x += BIG_CHAR_W * peScale;
        }
    }

    // Fade out al pulsar ENTER
    if (endingFadeOut) {
        unsigned char a = (unsigned char)(endingFadeAlpha * 255.0f);  // ← solo lee, no modifica
        DrawRectangle(0, 0, SW, SH, { 0, 0, 0, a });
    }
}

void UiManager::FullReset() {
    credits = 0;
    score = 0;
    level = 1;
    timeLeft = 60;
    bombs = 10;
    ammo = 0;
    timeAccum = 0.0f;
    introTimer = 0.0f;
    blinkAccum = 0.0f;
    blinkVisible = true;
    idleTimer = 0.0f;
    goVisible = false;
    goBlinkOn = false;
    goBlinkAccum = 0.0f;
    weaponDisplay = WeaponDisplay::PISTOL;
    jetpackActive = false;
    jetpackFuelRatio = 0.0f;
    continueElapsed      = 0.0f;
    lastContinueDigit    = -1;
    continueBlinkAccum = 0.0f;
    continueBlinkOn = true;
    continueScreenActive = false;
    continueDelayActive = false;
    continueDelay = 0.0f;
    missionActive = false;
    missionCompleteActive = false;
    missionTimer = 0.0f;
    missionLetterCount = 0;
    missionLettersSpawned = 0;
    letterSpawnTimer = 0.0f;
    exitPhase = false;
    exitTimer = 0.0f;
    exitFadeAlpha = 0.0f;
    slowExit = false;
    startBlinkTimer = 0.0f;
    startBlinkCount = 0;
    startBlinkVisible = true;
    endingActive = false;
    endingTimer = 0.0f;
    endingFadeOut = false;
    endingFadeAlpha = 0.0f;
    endingFinished = false;
    goAnimActive = false;
    goAnimFrame = 0;
    goAnimCycle = 0;
    goAnimTimer = 0.0f;
    goAnimPausing = false;
    goAnimPauseTimer = 0.0f;
    goIdleTimer = 0.0f;
    missionCompleteDone = false;
}