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
    uiManager.StartMissionIntro();
}

Game::~Game() {}

// ─────────────────────────────────────────
//  Reset
// ─────────────────────────────────────────
void Game::Reset()
{
    player.ResetToStart();
    player.GetAnim().StopParachute();
    player.GetAnim().StartParachute();
    if (!player.IsAlive()) player.Respawn();
    uiManager.AddScore(-uiManager.GetScore());
    camera.Reset();
    musicStarted = false;
    introSkipped = false;
    howtoplayMusicStarted = false;
    continueStarted = false;
    uiManager.StartMissionIntro();
    missionCompleteTriggered = false;
    endingStarted = false;
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
    for (auto& blood : creationManager.GetBloodEffects()) blood.Draw();
    for (auto& p : creationManager.GetPrisoners()) p.Draw();
    for (auto& ft : creationManager.GetFloatingTexts()) ft.Draw();

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
        if (!musicStarted && !sceneManager.IsSplashActive()) {
            SetMusicVolume(audioManager.GetIntroMusic(), 4.0f); // przywróc volume po ewentualnym StopIntroMusic
            audioManager.PlayMusic(audioManager.GetIntroMusic());
            musicStarted = true;
        }
        audioManager.UpdateMusic(audioManager.GetIntroMusic());
        BeginDrawing();
        ClearBackground(BLACK);

        if (sceneManager.IsSplashActive()) {
            sceneManager.DrawTexts();
            return;
        }

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            audioManager.StopIntroMusic();
            musicStarted = true;
            introSkipped = true;
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
        }
        else {
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
        musicStarted = false;
        if (!howtoplayMusicStarted) {
            audioManager.PlayMusic(audioManager.GetHowtoplayMusic());
            howtoplayMusicStarted = true;
        }
        audioManager.UpdateMusic(audioManager.GetHowtoplayMusic());

        BeginDrawing();
        ClearBackground(BLACK);
        sceneManager.DrawTexts();
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

        inputManager.InputContinueScreen();
        if (sceneManager.GetGamestate() != SceneManager::CONTINUE_SCREEN) {
            BeginDrawing();
            ClearBackground(BGCOLOR);
            Draw();
            return;
        }

        if (uiManager.IsContinueOver()) {
            uiManager.StopContinue();
            audioManager.StopMusic(audioManager.GetGameMusic());
            musicStarted = false;
            gameOverTimer = 0.0f;
            sceneManager.SetGameState(SceneManager::GAME_OVER);
            return;
        }

        float camTop = camera.GetCamera().target.y - camera.GetCamera().offset.y;
        player.Update(camera.GetLeftLimit(), camTop);
        systemCollision.CollisionUpdate();
        boss.Update(player.GetPosition().x);

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
        auto& grenadesCont = creationManager.GetGrenades();
        grenadesCont.erase(std::remove_if(grenadesCont.begin(), grenadesCont.end(),
            [](const Grenade& g) { return !g.IsActive(); }), grenadesCont.end());

        for (auto& ft : creationManager.GetFloatingTexts()) ft.Update(dt);
        creationManager.GetFloatingTexts().erase(
            std::remove_if(creationManager.GetFloatingTexts().begin(), creationManager.GetFloatingTexts().end(),
                [](const FloatingText& ft) { return !ft.IsActive(); }),
            creationManager.GetFloatingTexts().end());

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

        if (gameOverTimer < 4.0f)
        {
            BeginDrawing();
            ClearBackground(BGCOLOR);
            Draw();
            uiManager.DrawGameOverOverlay(gameOverTimer);
            return;
        }

        if (!gameOverSoundPlayed) {
            audioManager.PlaySound(audioManager.GetGameOverSound());
            gameOverSoundPlayed = true;
        }

        if (gameOverTimer < 9.0f)
        {
            BeginDrawing();
            uiManager.DrawGameOverSprite(gameOverTimer - 4.0f);
            return;
        }

        shouldRestart = true;
        sceneManager.SetGameState(SceneManager::INTRO);
        BeginDrawing();
        ClearBackground(BLACK);
        return;
    }

    // ── GAME ───────────────────────────────
    float dt = GetFrameTime();

    uiManager.Update();
    timerManager.Update(dt);
    if (uiManager.IsTimeUp() && player.IsAlive())
        player.TakeDamage();

    // Jezeli animacja MISSION COMPLETE skonczyla sie (ekran czarny) — uruchom ekran THE END
    if (missionCompleteTriggered && uiManager.IsMissionCompleteDone() && !endingStarted) {
        endingStarted = true;
        uiManager.StartEnding();
        audioManager.PlayMusic(audioManager.GetTheEndMusic());
    }

    // Ekran THE END aktywny — pomijamy logike gry, tylko update/draw ending
    if (endingStarted) {
        audioManager.UpdateMusic(audioManager.GetTheEndMusic());
        uiManager.UpdateEnding(dt);

        if (!uiManager.IsEndingFinished() && IsKeyPressed(KEY_ENTER)) {
            audioManager.StopMusic(audioManager.GetTheEndMusic());
            uiManager.TriggerEndingFadeOut();
        }

        if (uiManager.IsEndingFinished()) {
            audioManager.StopMusic(audioManager.GetTheEndMusic());
            shouldRestart = true;
            sceneManager.SetGameState(SceneManager::TITLE);
        }

        BeginDrawing();
        uiManager.DrawEnding();
        return;
    }

    boss.Update(player.GetPosition().x);
    if (boss.IsDestroyed() && boss.IsDestroyAnimFinished() && !missionCompleteTriggered) {
        missionCompleteTriggered = true;
        uiManager.StartMissionComplete();
        audioManager.StopMusic(audioManager.GetGameMusic());
        audioManager.PlaySound(audioManager.GetMissionCompleteSound());
    }
    if (boss.IsDestroyed()) {
        player.StopMovingHorizontal();
        if (player.IsCrouching()) player.StopCrouching();
    }

    player.SavePreviousPosition();
    HandleInput();
    float camTop = camera.GetCamera().target.y - camera.GetCamera().offset.y;
    player.Update(camera.GetLeftLimit(), camTop);

    systemCollision.CollisionUpdate();

    if (player.HasJetpack())
    {
        uiManager.SetJetpackFuel(player.GetJetpackFuel() / player.GetJetpackMaxFuel());
        // Ensure UI shows the jetpack bar when player picked the item
        if (!uiManager.GetJetpackActive()) uiManager.SetJetpackActive(true);
    }
    else if (uiManager.GetJetpackActive())
    {
        uiManager.SetJetpackFuel(0.0f);
        uiManager.SetJetpackActive(false);
    }

    Camera2D cam = camera.GetCamera();
    float camLeft = cam.target.x - cam.offset.x;
    float camRight = cam.target.x + cam.offset.x;
    for (auto& p : creationManager.GetPrisoners())
        p.Update(player.GetPosition().x, camLeft, camRight);
    creationManager.GetPrisoners().erase(
        std::remove_if(creationManager.GetPrisoners().begin(), creationManager.GetPrisoners().end(),
            [](const Prisoner& p) { return !p.IsActive(); }),
        creationManager.GetPrisoners().end());

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
        if (soldier.WantsToShoot()) {
            Shoot(2, soldier.GetPosition(), soldier.IsFacingRight());
            soldier.ResetShootWants();
        }
    }

    for (auto& blood : creationManager.GetBloodEffects()) blood.Update();
    creationManager.GetBloodEffects().erase(
        std::remove_if(creationManager.GetBloodEffects().begin(), creationManager.GetBloodEffects().end(),
            [](const BloodEffect& b) { return !b.IsActive(); }),
        creationManager.GetBloodEffects().end());

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

    for (auto& ft : creationManager.GetFloatingTexts()) ft.Update(dt);
    creationManager.GetFloatingTexts().erase(
        std::remove_if(creationManager.GetFloatingTexts().begin(), creationManager.GetFloatingTexts().end(),
            [](const FloatingText& ft) { return !ft.IsActive(); }),
        creationManager.GetFloatingTexts().end());

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

    if (!player.IsAlive() && player.IsDisappeared() && !continueStarted)
    {
        continueStarted = true;
        uiManager.StartContinue();
        sceneManager.SetGameState(SceneManager::CONTINUE_SCREEN);
    }

    BeginDrawing();
    ClearBackground(BGCOLOR);
    Draw();

    if (!boss.IsDestroyed()) {
        Color bossTint = boss.IsFlashing() ? ORANGE : WHITE;
        backgroundManager.SetEventSpriteTint(2, bossTint);
        backgroundManager.SetEventSpriteTint(3, { 255, 255, 255, 0 });
    }
    else {
        backgroundManager.SetEventSpriteTint(2, { 255, 255, 255, 0 });
        backgroundManager.SetEventSpriteTint(3, WHITE);
        backgroundManager.SetEventSpriteFrame(3, boss.GetDestroyFrame());
    }

    backgroundManager.FollowPlayer(camera.GetCamera().target);
    backgroundManager.Update(dt);

    if (!musicStarted) {
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
    inputManager.InputEnding();

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