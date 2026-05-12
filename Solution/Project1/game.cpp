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
    player.ResetToStart();
    uiManager.SetAmmo(0);
    uiManager.SetWeaponDisplay(UiManager::WeaponDisplay::PISTOL);

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
    musicStarted           = false;
    introSkipped           = false;
    howtoplayMusicStarted  = false;
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
        if (!musicStarted) {
            audioManager.PlayMusic(audioManager.GetIntroMusic());
            musicStarted = true;
        }
        audioManager.UpdateMusic(audioManager.GetIntroMusic());
        BeginDrawing();
        ClearBackground(BLACK);

        // Jezeli Enter/Space wcisniety — skipnij intro i zatrzymaj muzyke
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            audioManager.StopIntroMusic();     // działa bezpośrednio na składowej
            musicStarted = true;
            introSkipped  = true;
            sceneManager.SetGameState(SceneManager::TITLE);
            return;
        }

        sceneManager.DrawTexts();
        return;
    }

    // ── TITLE ──────────────────────────────
    if (sceneManager.GetGamestate() == SceneManager::TITLE)
    {
        if (introSkipped) {
            audioManager.StopIntroMusic();
        } else {
            if (!musicStarted) {
                audioManager.PlayMusic(audioManager.GetIntroMusic());
                musicStarted = true;
            }
            audioManager.UpdateMusic(audioManager.GetIntroMusic());
        }

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
        return;
    }

    // ── HOWTOPLAY ─────────────────────────
    if (sceneManager.GetGamestate() == SceneManager::HOWTOPLAY)
    {
        musicStarted = false; // reset zeby GAME odpalil muzyke i dzwiek od nowa
        if (!howtoplayMusicStarted) {
            audioManager.PlayMusic(audioManager.GetHowtoplayMusic());
            howtoplayMusicStarted = true;
        }
        audioManager.UpdateMusic(audioManager.GetHowtoplayMusic());

        BeginDrawing();
        ClearBackground(BLACK);
        sceneManager.DrawTexts();
        // Jezeli HOWTOPLAY sie skonczylo naturalnie — zatrzymaj muzyke
        if (sceneManager.GetGamestate() != SceneManager::HOWTOPLAY) {
            StopMusicStream(audioManager.GetHowtoplayMusic());
            howtoplayMusicStarted = false;
        }
        return;
    }

    // ── CONTINUE SCREEN ───────────────────
    if (sceneManager.GetGamestate() == SceneManager::CONTINUE_SCREEN)
    {
        float dt = GetFrameTime();
        uiManager.UpdateContinue(dt);
        timerManager.Update(dt);

        // C -> dodaj credit i natychmiast kontynuuj
        if (IsKeyPressed(KEY_C)) {
            uiManager.SetCredits(1);
            uiManager.SetCredits(-1);
            uiManager.StopContinue();
            player.Respawn();
            sceneManager.SetGameState(SceneManager::GAME);
            BeginDrawing();
            ClearBackground(BGCOLOR);
            Draw();
            return;
        }

        // Countdown skonczony -> sekwencja GAME OVER
        if (uiManager.IsContinueOver()) {
            uiManager.StopContinue();
            audioManager.StopMusic(audioManager.GetGameMusic());
            musicStarted = false;
            gameOverTimer = 0.0f;
            sceneManager.SetGameState(SceneManager::GAME_OVER);
            return;
        }

        // Aktualizuj swiat gry (bez inputu gracza)
        player.Update(camera.GetLeftLimit());
        systemCollision.CollisionUpdate();


        for (auto& item : creationManager.GetItems()) item.Update();
        creationManager.GetItems().erase(
            std::remove_if(creationManager.GetItems().begin(), creationManager.GetItems().end(),
                [](const Item& i) { return !i.IsActive(); }),
            creationManager.GetItems().end());

        camera.Update(player.GetPosition(),
            backgroundManager.GetWidth(),
            backgroundManager.GetHeight(),
            player.GetIsGrounded());

        for (auto& soldier : creationManager.GetSoldiers()) {
            soldier.UpdateAI(player);
            soldier.Update();
        }
        for (auto& bullet : creationManager.GetBullets()) bullet.Update();
        CheckBulletsOutOfCamera();

        for (auto& grenade : creationManager.GetGrenades()) {
            grenade.Update();
            grenade.CheckCollisionWithBlocks(creationManager.GetTileMap().GetColliders());
            grenade.CheckCollisionWithSoldiers(creationManager.GetSoldiers());
        }
        auto& grenades = creationManager.GetGrenades();
        grenades.erase(std::remove_if(grenades.begin(), grenades.end(),
            [](const Grenade& g) { return !g.IsActive(); }), grenades.end());

        BeginDrawing();
        ClearBackground(BGCOLOR);
        Draw();
        if (!uiManager.IsDelayActive())
            uiManager.DrawContinueScreen();
        backgroundManager.FollowPlayer(camera.GetCamera().target);
        backgroundManager.Update(dt);
        audioManager.UpdateMusic(audioManager.GetGameMusic());
        return;
    }

    // ── GAME OVER SEQUENCE ────────────────
    if (sceneManager.GetGamestate() == SceneManager::GAME_OVER)
    {
        float dt = GetFrameTime();
        gameOverTimer += dt;

        // Faza 1+2: czerwony filtr + zaciemnienie (0-4s) — swiat gry widoczny pod filtrem
        if (gameOverTimer < 4.0f)
        {
            BeginDrawing();
            ClearBackground(BGCOLOR);
            Draw();
            uiManager.DrawGameOverOverlay(gameOverTimer);
            return;
        }

        // Faza 3: sprite game over (4-9s)
        if (gameOverTimer < 9.0f)
        {
            BeginDrawing();
            uiManager.DrawGameOverSprite(gameOverTimer - 4.0f);
            return;
        }

        // Koniec -> INTRO
        shouldRestart = true;
        sceneManager.SetGameState(SceneManager::INTRO);
        BeginDrawing();
        ClearBackground(BLACK);
        return;
    }

    // ── GAME ───────────────────────────────

    uiManager.Update();
    timerManager.Update(GetFrameTime());
    if (uiManager.IsTimeUp() && player.IsAlive())
        player.TakeDamage();
    boss.Update(player.GetPosition().x);

    player.SavePreviousPosition();
    HandleInput();
    player.Update(camera.GetLeftLimit());

    // Colisiones (todos los sistemas)
    systemCollision.CollisionUpdate();

    if (player.HasJetpack())
        uiManager.SetJetpackFuel(player.GetJetpackFuel() / player.GetJetpackMaxFuel());
    else if (uiManager.GetJetpackActive())
    {
        uiManager.SetJetpackFuel(0.0f);
        uiManager.SetJetpackActive(false);
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


    // Gracz zniknal po animacji smierci
    if (!player.IsAlive() && player.IsDisappeared()) {
        if (uiManager.GetCredits() > 0) {
            uiManager.SetCredits(-1);
        } else {
            uiManager.StartContinue();
            sceneManager.SetGameState(SceneManager::CONTINUE_SCREEN);
        }
    }

    BeginDrawing();
    ClearBackground(BGCOLOR);
    Draw();

    // Flash del sprite del boss en background
    Color bossTint = WHITE;
    if (!boss.IsDestroyed()) {
        bossTint = boss.IsFlashing() ? ORANGE : WHITE;
        backgroundManager.SetEventSpriteTint(5, bossTint);
    }
    else
        bossTint = { 255, 255, 255, 0 }; 
    backgroundManager.SetEventSpriteTint(4, bossTint);

    if (boss.IsDestroyed()) {
        backgroundManager.SetEventSpriteFrame(5, boss.GetDestroyFrame());
    }

    backgroundManager.FollowPlayer(camera.GetCamera().target);
    backgroundManager.Update(GetFrameTime());

    if (!musicStarted) {
        // Zatrzymaj intro muzykę i puść game muzykę
        audioManager.StopMusic(audioManager.GetIntroMusic());
        audioManager.PlayMusic(audioManager.GetGameMusic());
        audioManager.PlaySound(audioManager.GetGameSound());
        musicStarted = true;
    }
    audioManager.UpdateMusic(audioManager.GetGameMusic());
}

// ─────────────────────────────────────────
//  HandleInput
// ─────────────────────────────────────────
void Game::HandleInput()
{
    if (boss.IsDestroyed()) return;
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
    float hw = cam.offset.x;
    float hh = cam.offset.y;

    float left = cam.target.x - hw + 1.0f;  
    float right = cam.target.x + hw - 1.0f;  
    float top = cam.target.y - hh + 1.0f;
    float bottom = cam.target.y + hh - 1.0f;

    auto& bullets = creationManager.GetBullets();
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [&](const Bullet& b) {
            Vector2 p = b.GetPosition();
            return p.x < left || p.x > right || p.y < top || p.y > bottom;
        }), bullets.end());
}