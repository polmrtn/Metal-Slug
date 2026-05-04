#define _CRT_SECURE_NO_WARNINGS
#include "game.hpp"
#include <raymath.h>
#include <algorithm>

static bool musicStarted = false;
static Color BGCOLOR = { 195, 195, 170 };

// ─────────────────────────────────────────
//  Constructor / Destructor
// ─────────────────────────────────────────
Game::Game() : camera({ 1200.0f / 2.0f, 896.0f / 2.0f })
{
    creationManager.LoadFromFile("level.txt");
    debug.SetGridOffset(creationManager.GetTileMap().GetGridOffset());

    player.SetGrounded(true);
    player.SetX(15000.0f);
    player.SetY(100.0f);

    boss.Init();
}

Game::~Game() {}

// ─────────────────────────────────────────
//  Reset
// ─────────────────────────────────────────
void Game::Reset()
{
    player.ResetToStart();
    if (!player.IsAlive()) player.Respawn();
    camera.Reset();
    musicStarted = false;
}

// ─────────────────────────────────────────
//  Draw
// ─────────────────────────────────────────
void Game::Draw()
{
    camera.Begin();

    backgroundManager.Draw();
    player.Draw();
    for (auto& soldier : creationManager.GetSoldiers()) soldier.Draw();
    backgroundManager.Drawfrontground();
    for (auto& bullet : creationManager.GetBullets())  bullet.Draw();
    for (auto& grenade : creationManager.GetGrenades()) grenade.Draw();
    for (auto& item : creationManager.GetItems())    item.Draw();

    // Tiles en coordenadas de mundo
    creationManager.GetTileMap().DrawTiles();
    creationManager.GetTileMap().DrawColliders();
    debug.DrawEditorGrid(camera.GetCamera());
    boss.Draw();

    camera.End();

    debug.SetEditorMode(camera.GetCamera());  // ← solo HUD en pantalla
    uiManager.DrawCredits(camera.GetCamera());
}

// ─────────────────────────────────────────
//  Update
// ─────────────────────────────────────────
void Game::Update()
{
    // ── INTRO ──────────────────────────────
    if (sceneManager.GetGamestate() == SceneManager::INTRO)
    {
        BeginDrawing();
        ClearBackground(BLACK);
        sceneManager.DrawTexts();
        return;
    }

    // ── TITLE ──────────────────────────────
    if (sceneManager.GetGamestate() == SceneManager::TITLE)
    {
        BeginDrawing();
        ClearBackground(BLACK);
        sceneManager.DrawTexts();

        if (IsKeyPressed(KEY_C) && timerManager.IsReady(TimerType::CREDIT_COOLDOWN)) {
            if (uiManager.GetCredits() < 99) {
                uiManager.SetCredits(1);
                timerManager.StartTimer(TimerType::CREDIT_DELAY);
            }
        }
        timerManager.Update(GetFrameTime());

        if (!musicStarted) {
            audioManager.PlayMusic(audioManager.GetTitleMusic());
            musicStarted = true;
        }
        audioManager.UpdateMusic(audioManager.GetTitleMusic());
        return;
    }

    // ── GAME ───────────────────────────────
    uiManager.Update();
    timerManager.Update(GetFrameTime());
    boss.Update(player.GetPosition().x);

    player.SavePreviousPosition();
    HandleInput();
    player.Update(camera.GetLeftLimit());

    // Colisiones (todos los sistemas)
    systemCollision.CollisionUpdate();

    // Zona de victoria
    Rectangle winZone = { 16190.0f, -9999.0f, 200.0f, 99999.0f };
    if (CheckCollisionRecs(player.GetHitBox(), winZone))
    {
        audioManager.StopMusic(audioManager.GetGameMusic());
        musicStarted = false;
        shouldRestart = true;
        sceneManager.SetGameState(SceneManager::TITLE);
    }

    // Limpiar items inactivos
    for (auto& item : creationManager.GetItems()) item.Update();
    creationManager.GetItems().erase(
        std::remove_if(creationManager.GetItems().begin(), creationManager.GetItems().end(),
            [](const Item& i) { return !i.IsActive(); }),
        creationManager.GetItems().end());

    camera.Update(player.GetPosition(),
        backgroundManager.GetWidth(),
        backgroundManager.GetHeight(),
        player.GetIsGrounded());

    // IA y disparo de soldados
    for (auto& soldier : creationManager.GetSoldiers()) {
        soldier.UpdateAI(player);
        soldier.Update();
        if (soldier.WantsToShoot()) {
            Shoot(2, soldier.GetPosition(), soldier.IsFacingRight());
            soldier.ResetShootWants();
        }
    }

    // Actualizar balas
    for (auto& bullet : creationManager.GetBullets()) bullet.Update();
    CheckBulletsOutOfCamera();

    // Actualizar granadas
    for (auto& grenade : creationManager.GetGrenades()) {
        grenade.Update();
        grenade.CheckCollisionWithBlocks(creationManager.GetTileMap().GetColliders());
        grenade.CheckCollisionWithSoldiers(creationManager.GetSoldiers());
    }
    auto& grenades = creationManager.GetGrenades();
    grenades.erase(std::remove_if(grenades.begin(), grenades.end(),
        [](const Grenade& g) { return !g.IsActive(); }), grenades.end());

    // ── RÁFAGA MACHINEGUN ──────────────────
    if (machinegunBurst)
    {
        if (timerManager.IsReady(TimerType::MACHINEGUN_BURST_TIMER))
        {
            timerManager.StartTimer(TimerType::MACHINEGUN_BURST_DELAY);
            if (player.GetAmmo() > 0) {
                ShootMachinegun(burstOffsets[machinegunBurstCount]);
                player.UseAmmo();
                uiManager.UseAmmo();
            }
            machinegunBurstCount++;
            if (machinegunBurstCount >= MACHINEGUN_BURST_SIZE || player.GetAmmo() <= 0) {
                machinegunBurst = false;
                machinegunBurstCount = 0;
                if (!IsKeyDown(KEY_UP)) player.StopAimingUp();
                if (player.GetAmmo() <= 0) {
                    uiManager.SetAmmo(0);
                    uiManager.SetWeaponDisplay(UiManager::WeaponDisplay::PISTOL);
                }
            }
        }
    }

    // ── SONIDOS MACHINEGUN ─────────────────
    if (machinegunSoundActive)
    {
        if (timerManager.IsReady(TimerType::MACHINEGUN_SOUND_TIMER))
        {
            timerManager.StartTimer(TimerType::MACHINEGUN_SOUND_DELAY);
            audioManager.PlaySound(audioManager.GetMachinegunShootSound());
            int count = static_cast<int>(timerManager.GetTimer(TimerType::MACHINEGUN_SOUND_COUNT)) + 1;
            timerManager.SetTimerValue(TimerType::MACHINEGUN_SOUND_COUNT, static_cast<float>(count));
            if (count >= static_cast<int>(timerManager.GetTimer(TimerType::MACHINEGUN_SOUND_SHOTS_CONST))) {
                machinegunSoundActive = false;
                timerManager.ResetTimer(TimerType::MACHINEGUN_SOUND_COUNT);
            }
        }
    }

    BeginDrawing();
    ClearBackground(BGCOLOR);
    Draw();

    backgroundManager.FollowPlayer(camera.GetCamera().target);
    backgroundManager.Update(GetFrameTime());

    if (!musicStarted) {
        audioManager.PlayMusic(audioManager.GetGameMusic());
        musicStarted = true;
    }
    audioManager.UpdateMusic(audioManager.GetGameMusic());
}

// ─────────────────────────────────────────
//  HandleInput
// ─────────────────────────────────────────
void Game::HandleInput()
{
    debug.EditorModeInput(camera.GetCamera());

    if (debug.GetEditorMode()) return; 

    inputManager.InputChangeScene();
    inputManager.InputCreditsPlayer();
    inputManager.InputUi();

    if (player.IsFalling()) return;

    if (machinegunBurst) {
        inputManager.InputMachinegunBurst();
        return;
    }

    inputManager.InputPlayer();
}

// ─────────────────────────────────────────
//  Shoot
// ─────────────────────────────────────────
void Game::Shoot(int bulletType, Vector2 startPos, bool faceRight)
{
    Vector2 bulletPos = {};
    float dirX = 0.0f, dirY = 0.0f;
    float bulletSpeed = 1000.0f;

    if (bulletType == 1)
    {
        Vector2 pPos = player.GetPosition();
        float   pW = player.GetWidth();
        float   pH = player.GetHeight();
        bool    crouch = player.IsCrouching();
        float   yOff = crouch ? -40.0f : -50.0f;

        switch (player.GetAimDirection()) {
        case PlayerDirection::LEFT:
            bulletPos = { pPos.x,          pPos.y + pH / 2.0f + yOff };
            dirX = -1.0f; break;
        case PlayerDirection::RIGHT:
            bulletPos = { pPos.x + pW,     pPos.y + pH / 2.0f + yOff };
            dirX = 1.0f; break;
        case PlayerDirection::UP:
            bulletPos = { pPos.x + pW / 2.0f, pPos.y - 20.0f };
            dirY = -1.0f; break;
        case PlayerDirection::DOWN:
            bulletPos = { pPos.x + pW / 2.0f, pPos.y + pH };
            dirY = 1.0f; break;
        }
    }
    else if (bulletType == 2)
    {
        bulletPos = startPos;
        bulletSpeed = 400.0f;
        dirX = faceRight ? 1.0f : -1.0f;
        dirY = -5.0f;
    }

    creationManager.GetBullets().emplace_back(
        bulletPos, (int)bulletSpeed, (int)dirX, (int)dirY, bulletType);
}

// ─────────────────────────────────────────
//  ShootMachinegun
// ─────────────────────────────────────────
void Game::ShootMachinegun(float yOffset)
{
    Vector2 pPos = player.GetPosition();
    float   pW = player.GetWidth();
    float   pH = player.GetHeight();
    float   baseY = pPos.y + pH / 2.0f - 50.0f + yOffset;

    Vector2 bulletPos;
    float dirX = 0.0f, dirY = 0.0f;

    switch (player.GetAimDirection()) {
    case PlayerDirection::LEFT:
        bulletPos = { pPos.x - 120.0f, baseY };
        dirX = -1.0f; break;
    case PlayerDirection::RIGHT:
        bulletPos = { pPos.x + pW + 70.0f, baseY };
        dirX = 1.0f; break;
    case PlayerDirection::UP:
        bulletPos = { pPos.x + pW / 2.0f + yOffset - 30.0f, pPos.y - 180.0f };
        dirY = -1.0f; break;
    default:
        bulletPos = { pPos.x + pW, baseY };
        dirX = 1.0f; break;
    }

    creationManager.GetBullets().emplace_back(bulletPos, 1000, (int)dirX, (int)dirY, 3);
}

void Game::StartMachinegunBurst() {
    machinegunBurst = true;
    machinegunBurstCount = 0;
    machinegunBurstDir = player.GetAimDirection();
    machinegunSoundActive = true;
    timerManager.ResetTimer(TimerType::MACHINEGUN_SOUND_COUNT);
}

// ─────────────────────────────────────────
//  ThrowGrenade
// ─────────────────────────────────────────
void Game::ThrowGrenade()
{
    GrenadeThrowData data = player.ThrowGrenade();
    if (data.valid)
        creationManager.GetGrenades().emplace_back(data.startPos, data.initialVelocity);
}

// ─────────────────────────────────────────
//  CheckBulletsOutOfCamera
// ─────────────────────────────────────────
void Game::CheckBulletsOutOfCamera()
{
    Camera2D cam = camera.GetCamera();
    float hw = GetScreenWidth() / 2.0f;
    float hh = GetScreenHeight() / 2.0f;
    const float margin = 200.0f;

    float left = cam.target.x - hw - margin;
    float right = cam.target.x + hw + margin;
    float top = cam.target.y - hh - margin;
    float bottom = cam.target.y + hh + margin;

    auto& bullets = creationManager.GetBullets();
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [&](const Bullet& b) {
            Vector2 p = b.GetPosition();
            return p.x < left || p.x > right || p.y < top || p.y > bottom;
        }), bullets.end());
}