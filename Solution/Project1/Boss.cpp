#include "Boss.hpp"
#include "GlobalManagers.hpp"

Boss::Boss() {}
Boss::~Boss() {}

void Boss::Init()
{
    Image img = LoadImage("Graphics/boss/cannon56x96.png");
    cannonSheet = LoadTextureFromImage(img);
    UnloadImage(img);
    SetTextureFilter(cannonSheet, TEXTURE_FILTER_POINT);

    Image imgFlash = LoadImage("Graphics/boss/shootingcannon56x96.png");
    flashSheet = LoadTextureFromImage(imgFlash);
    UnloadImage(imgFlash);
    SetTextureFilter(flashSheet, TEXTURE_FILTER_POINT);

    Image imgLaser = LoadImage("Graphics/boss/laser112x96.png");
    laserSheet = LoadTextureFromImage(imgLaser);
    UnloadImage(imgLaser);
    SetTextureFilter(laserSheet, TEXTURE_FILTER_POINT);

    Image imgPlasma = LoadImage("Graphics/boss/small10x10.png");
    plasmaSheet = LoadTextureFromImage(imgPlasma);
    UnloadImage(imgPlasma);
    SetTextureFilter(plasmaSheet, TEXTURE_FILTER_POINT);

    Image imgTent = LoadImage("Graphics/boss/tent120x104.png");
    tentSheet = LoadTextureFromImage(imgTent);
    UnloadImage(imgTent);
    SetTextureFilter(tentSheet, TEXTURE_FILTER_POINT);

    Image imgLaserFlash = LoadImage("Graphics/boss/laserexplo112x96.png");
    laserFlashSheet = LoadTextureFromImage(imgLaserFlash);
    UnloadImage(imgLaserFlash);
    SetTextureFilter(laserFlashSheet, TEXTURE_FILTER_POINT);
    laserFlashDelay = CHARGE_DURATION / (float)CHARGE_PATTERN_LENGTH;

    Image imgLaserBeam = LoadImage("Graphics/boss/ziuup80x26.png");
    laserBeamSheet = LoadTextureFromImage(imgLaserBeam);
    UnloadImage(imgLaserBeam);
    SetTextureFilter(laserBeamSheet, TEXTURE_FILTER_POINT);

    Image imgBeam = LoadImage("Graphics/boss/beam16x32.png");
    beamSheet = LoadTextureFromImage(imgBeam);
    UnloadImage(imgBeam);
    SetTextureFilter(beamSheet, TEXTURE_FILTER_POINT);

    Image imgSplatter = LoadImage("Graphics/boss/splatter64x48.png");
    splatterSheet = LoadTextureFromImage(imgSplatter);
    UnloadImage(imgSplatter);
    SetTextureFilter(splatterSheet, TEXTURE_FILTER_POINT);

    Image imgCannonDestroyed = LoadImage("Graphics/boss/cannondestoyed1file.png");
    cannonDestroyedSheet = LoadTextureFromImage(imgCannonDestroyed);
    UnloadImage(imgCannonDestroyed);
    SetTextureFilter(cannonDestroyedSheet, TEXTURE_FILTER_POINT);

    Image imgExplo1 = LoadImage("Graphics/boss/explo1boss40x45.png");
    explo1Sheet = LoadTextureFromImage(imgExplo1);
    UnloadImage(imgExplo1);
    SetTextureFilter(explo1Sheet, TEXTURE_FILTER_POINT);

    Image imgExplo2 = LoadImage("Graphics/boss/explo2boss52x70.png");
    explo2Sheet = LoadTextureFromImage(imgExplo2);
    UnloadImage(imgExplo2);
    SetTextureFilter(explo2Sheet, TEXTURE_FILTER_POINT);

    Image imgDebris = LoadImage("Graphics/boss/debri32x32.png");
    debrisSheet = LoadTextureFromImage(imgDebris);
    UnloadImage(imgDebris);
    SetTextureFilter(debrisSheet, TEXTURE_FILTER_POINT);

    cannonFrame = 9;
    cannonGoingUp = true;
    cannonFrameTimer = 0.0f;
    cannonState = CannonState::MOVING;
    stateTimer = 0.0f;
    openFrame = 0;
    closeFrame = 0;
    flashActive = false;
    flashFrame = 0;
}

void Boss::Update(float playerX)
{
    float dt = GetFrameTime();

    if (introState != IntroState::DONE) {
        UpdateIntro(playerX, dt);
        if (isFlashing) {
            hitFlashTimer += dt;
            if (hitFlashTimer >= HIT_FLASH_DURATION) {
                hitFlashTimer = 0.0f;
                hitFlashCount++;
                if (hitFlashCount >= HIT_FLASH_COUNT) {
                    isFlashing = false;
                    hitFlashCount = 0;
                }
            }
        }
        return;
    }

    if (isFlashing) {
        hitFlashTimer += dt;
        if (hitFlashTimer >= HIT_FLASH_DURATION) {
            hitFlashTimer = 0.0f;
            hitFlashCount++;
            if (hitFlashCount >= HIT_FLASH_COUNT) {
                isFlashing = false;
                hitFlashCount = 0;
            }
        }
    }

    if (splatterActive) {
        splatterTimer += dt;
        if (splatterTimer >= splatterDelay) {
            splatterTimer = 0.0f;
            splatterFrame++;
            if (splatterFrame >= SPLATTER_FRAMES) {
                splatterFrame = 0;
                splatterActive = false;
                splatterDone = false;
            }
        }
    }

    if (!phase2 && !phase2Pending && health <= 150)
        phase2Pending = true;

    // Guardar posición del cañón
    if (phase2) {
        bool isDown = !cannonGoingUp;
        lastCannonDrawX = posX + laserOffsetX;
        lastCannonDrawY = posY + (isDown ? laserOffsetDownY : laserOffsetUpY);
    }
    else {
        lastCannonDrawX = posX;
        bool up = (cannonState == CannonState::OPENING_UP ||
            cannonState == CannonState::SHOOTING_UP ||
            cannonState == CannonState::CLOSING_UP);
        lastCannonDrawY = up ? posY - 150.0f : posY;
    }

    if (destroyed) {
        static float dfTimer = 0.0f;
        dfTimer += dt;
        if (dfTimer >= 0.5f && destroyFrame < 4) {
            dfTimer = 0.0f;
            destroyFrame++;
        }
        if (!cannonDestroyedVisible) {
            cannonDestroyedVisible = true;
            cannonDestroyedTimer = 0.0f;
            isFlashing = false;
            flashActive = false;
        }
        cannonDestroyedTimer += dt;
        destroyedTimer += dt;

        // Spawn explosiones Y debris durante 8 segundos
        if (destroyedTimer < DESTROY_EXPLOSION_DURATION) {
            explosionSpawnTimer += dt;
            if (explosionSpawnTimer >= explosionSpawnDelay) {
                explosionSpawnTimer = 0.0f;

                int leftLimit = 0;
                if (destroyFrame >= 3) leftLimit = 56;
                if (destroyFrame >= 4) leftLimit = 161;

                // Spawn explosión
                for (int i = 0; i < MAX_BOSS_EXPLOSIONS; i++) {
                    if (!bossExplosions[i].active) {
                        bossExplosions[i].active = true;
                        bossExplosions[i].frame = 0;
                        bossExplosions[i].timer = 0.0f;
                        bossExplosions[i].type = GetRandomValue(0, 1);
                        int zone = GetRandomValue(0, 1);
                        if (zone == 0) {
                            bossExplosions[i].pos = {
                                TETSU_X + (float)GetRandomValue(leftLimit, 384) * CANNON_SCALE,
                                TETSU_Y + (float)GetRandomValue(0, 144) * CANNON_SCALE
                            };
                        }
                        else {
                            bossExplosions[i].pos = {
                                TETSU_X + (float)GetRandomValue(254, 384) * CANNON_SCALE,
                                TETSU_Y + (float)GetRandomValue(144, 288) * CANNON_SCALE
                            };
                        }
                        break;
                    }
                }

                // Spawn debris
                for (int i = 0; i < MAX_BOSS_DEBRIS; i++) {
                    if (!bossDebris[i].active) {
                        bossDebris[i].active = true;
                        bossDebris[i].frame = 0;
                        bossDebris[i].timer = 0.0f;
                        bossDebris[i].spriteRow = GetRandomValue(0, 8);
                        bossDebris[i].frameDelay = 0.05f + GetRandomValue(0, 3) * 0.01f;
                        int zone2 = GetRandomValue(0, 1);
                        if (zone2 == 0) {
                            bossDebris[i].pos = {
                                TETSU_X + (float)GetRandomValue(leftLimit, 384) * CANNON_SCALE,
                                TETSU_Y + (float)GetRandomValue(0, 144) * CANNON_SCALE
                            };
                        }
                        else {
                            bossDebris[i].pos = {
                                TETSU_X + (float)GetRandomValue(254, 384) * CANNON_SCALE,
                                TETSU_Y + (float)GetRandomValue(144, 288) * CANNON_SCALE
                            };
                        }
                        float angle = (float)GetRandomValue(-150, -30);
                        float speed = (float)GetRandomValue(200, 600);
                        float rad = angle * 3.14159f / 180.0f;
                        bossDebris[i].vel = { cosf(rad) * speed, sinf(rad) * speed };
                        break;
                    }
                }
            }
        }

        // Actualizar explosiones
        for (int i = 0; i < MAX_BOSS_EXPLOSIONS; i++) {
            if (!bossExplosions[i].active) continue;
            bossExplosions[i].timer += dt;
            if (bossExplosions[i].timer >= EXPLO_FRAME_DURATION) {
                bossExplosions[i].timer = 0.0f;
                bossExplosions[i].frame++;
                if (bossExplosions[i].frame >= EXPLO_FRAMES)
                    bossExplosions[i].active = false;
            }
        }

        // Actualizar debris
        float screenLeft = player.GetPosition().x - (GetScreenWidth() / 2.0f) - 100.0f;
        float screenRight = player.GetPosition().x + (GetScreenWidth() / 2.0f) + 100.0f;
        float screenBot = player.GetPosition().y + (GetScreenHeight() / 2.0f) + 100.0f;

        for (int i = 0; i < MAX_BOSS_DEBRIS; i++) {
            if (!bossDebris[i].active) continue;
            bossDebris[i].vel.y += bossDebris[i].gravity * dt;
            bossDebris[i].pos.x += bossDebris[i].vel.x * dt;
            bossDebris[i].pos.y += bossDebris[i].vel.y * dt;
            bossDebris[i].timer += dt;
            if (bossDebris[i].timer >= bossDebris[i].frameDelay) {
                bossDebris[i].timer = 0.0f;
                int maxFrames = (bossDebris[i].spriteRow < 4) ? 8 : 10;
                bossDebris[i].frame = (bossDebris[i].frame + 1) % maxFrames;
            }
            if (bossDebris[i].pos.x < screenLeft || bossDebris[i].pos.x > screenRight ||
                bossDebris[i].pos.y > screenBot)
                bossDebris[i].active = false;
        }

        return;
    }

    if (phase2)
        UpdateLaser(dt);
    else
        UpdateCannon(dt);

    UpdatePlasma(dt);
    UpdateLaserFlash(dt);

    if (phase2 && laserBeamVisible) {
        laserBeamX -= laserBeamSpeed * dt;
        laserBeamTimer += dt;
        if (laserBeamTimer >= laserBeamDelay) {
            laserBeamTimer = 0.0f;
            laserBeamFrame = (laserBeamFrame + 1) % LASER_BEAM_FRAMES;
        }
        float screenLeft = player.GetPosition().x - (GetScreenWidth() / 2.0f) - LASER_BEAM_W * CANNON_SCALE;
        if (laserBeamX < screenLeft) {
            laserBeamVisible = false;
            laserBeamActive = false;
        }
    }
}

void Boss::UpdateIntro(float playerX, float dt)
{
    switch (introState)
    {
    case IntroState::IDLE:
        if (playerX >= TENT_ACTIVATE_X) {
            playerInRange = true;
        }
        if (playerInRange) {
            preIntroTimer += dt;
            if (preIntroTimer >= PRE_INTRO_DELAY) {
                introState = IntroState::WAITING;
                introTimer = 0.0f;
            }
        }
        break;

    case IntroState::WAITING:
        introTimer += dt;
        if (introTimer >= TENT_WAIT) {
            introState = IntroState::UNVEILING;
            tentFrame = 0;
            tentFrameTimer = 0.0f;
        }
        break;

    case IntroState::UNVEILING:
        tentFrameTimer += dt;
        if (tentFrameTimer >= tentFrameDelay) {
            tentFrameTimer = 0.0f;
            tentFrame++;
            tentOffsetX += 25.0f;  // ← píxeles que se mueve a la derecha por frame
            if (tentFrame >= TENT_FRAMES) {
                introState = IntroState::PAUSE;
                introTimer = 0.0f;
            }
        }
        break;

    case IntroState::PAUSE:
        introTimer += dt;
        if (introTimer >= TENT_PAUSE) {
            introState = IntroState::DONE;
            active = true;  // ahora recibe daño y ataca
        }
        break;

    case IntroState::DONE:
        break;
    }
}

void Boss::UpdateCannon(float dt)
{
    cannonFrameTimer += dt;

    switch (cannonState)
    {
    case CannonState::MOVING:
        flashActive = false;
        if (cannonFrameTimer < cannonFrameDelay) return;
        cannonFrameTimer = 0.0f;

        if (cannonGoingUp) {
            cannonFrame++;
            if (cannonFrame >= CANNON_FRAMES - 1) {
                cannonFrame = CANNON_FRAMES - 1;
                if (phase2Pending) {
                    phase2 = true;
                    phase2Pending = false;
                    laserState = LaserState::CHARGING;
                    laserFrame = 0;
                    chargeTimer = 0.0f;
                    laserFrameTimer = 0.0f;
                }
                else {
                    cannonState = CannonState::OPENING_UP;
                    openFrame = 0;
                    audioManager.PlaySound(audioManager.GetBossOpenSound());
                }
                cannonFrameTimer = 0.0f;
            }
        }
        else {
            cannonFrame--;
            if (cannonFrame <= 0) {
                cannonFrame = 0;
                if (phase2Pending) {
                    phase2 = true;
                    phase2Pending = false;
                    laserState = LaserState::CHARGING;
                    laserFrame = 0;
                    chargeTimer = 0.0f;
                    laserFrameTimer = 0.0f;
                }
                else {
                    cannonState = CannonState::OPENING_DOWN;
                    openFrame = 0;
                    audioManager.PlaySound(audioManager.GetBossOpenSound());
                }
                cannonFrameTimer = 0.0f;
            }
        }
        break;

        // ── ABAJO ────────────────────────────────────────────────
    case CannonState::OPENING_DOWN:
        flashActive = false;
        if (cannonFrameTimer < openFrameDelay) return;
        cannonFrameTimer = 0.0f;
        openFrame++;
        if (openFrame >= OPEN_DOWN_FRAMES - 1) {
            openFrame = 0;
            cannonState = CannonState::SHOOTING_DOWN;
            cannonFrameTimer = 0.0f;
        }
        break;

    case CannonState::SHOOTING_DOWN:
        if (cannonFrameTimer < shootFrameDelay) return;
        cannonFrameTimer = 0.0f;
        openFrame++;

        if (openFrame < FLASH_FRAMES) {
            flashActive = true;
            flashFrame = openFrame;
        }
        else {
            flashActive = false;
        }

        if (openFrame >= SHOOT_DOWN_FRAMES - 2) {
            if (shootRepeatCount == 0)
                capturedPlayerX = player.GetPosition().x;
            FirePlasma();
            shootRepeatCount++;
            openFrame = 0;
            if (shootRepeatCount >= 3) {
                shootRepeatCount = 0;
                openFrame = OPEN_DOWN_FRAMES - 1;
                cannonState = CannonState::CLOSING_DOWN;
                flashActive = false;
            }
            cannonFrameTimer = 0.0f;
        }
        break;

    case CannonState::CLOSING_DOWN:
        flashActive = false;
        if (cannonFrameTimer < openFrameDelay) return;
        cannonFrameTimer = 0.0f;
        openFrame--;
        if (openFrame <= 0) {
            openFrame = 0;
            cannonGoingUp = true;
            cannonFrame = 0;
            cannonState = CannonState::MOVING;
            cannonFrameTimer = 0.0f;
        }
        break;

        // ── ARRIBA ───────────────────────────────────────────────
    case CannonState::OPENING_UP:
        flashActive = false;
        if (cannonFrameTimer < openFrameDelay) return;
        cannonFrameTimer = 0.0f;
        openFrame++;
        if (openFrame >= OPEN_UP_FRAMES - 1) {
            openFrame = 0;
            cannonState = CannonState::SHOOTING_UP;
            cannonFrameTimer = 0.0f;
        }
        break;

    case CannonState::SHOOTING_UP:
        if (cannonFrameTimer < shootFrameDelay) return;
        cannonFrameTimer = 0.0f;
        openFrame++;

        if (openFrame < FLASH_FRAMES) {
            flashActive = true;
            flashFrame = openFrame;
        }
        else {
            flashActive = false;
        }

        if (openFrame >= SHOOT_UP_FRAMES - 2) {
            if (shootRepeatCount == 0)
                capturedPlayerX = player.GetPosition().x;
            FirePlasma();
            shootRepeatCount++;
            openFrame = 0;
            if (shootRepeatCount >= 3) {
                shootRepeatCount = 0;
                openFrame = OPEN_UP_FRAMES - 1;
                cannonState = CannonState::CLOSING_UP;
                flashActive = false;
            }
            cannonFrameTimer = 0.0f;
        }
        break;

    case CannonState::CLOSING_UP:
        flashActive = false;
        if (cannonFrameTimer < openFrameDelay) return;
        cannonFrameTimer = 0.0f;
        openFrame--;
        if (openFrame <= 0) {
            openFrame = 0;
            cannonGoingUp = false;
            cannonFrame = CANNON_FRAMES - 1;
            cannonState = CannonState::MOVING;
            cannonFrameTimer = 0.0f;
        }
        break;
    }
}

void Boss::UpdateLaser(float dt)
{
    laserFrameTimer += dt;

    switch (laserState)
    {
    case LaserState::MOVING:
        if (splatterActive) return;
        if (laserFrameTimer < cannonFrameDelay) return;
        laserFrameTimer = 0.0f;

        if (cannonGoingUp) {
            cannonFrame++;
            if (cannonFrame >= CANNON_FRAMES - 1) {
                cannonFrame = CANNON_FRAMES - 1;
                laserState = LaserState::CHARGING;
                laserFrame = 0;
                chargeTimer = 0.0f;
                laserFrameTimer = 0.0f;
                laserFlashActive = true;
                laserFlashStep = 0;
                laserFlashFrame = CHARGE_PATTERN[0];
                laserFlashTimer = 0.0f;
                laserFlashDelay = CHARGE_DURATION / (float)CHARGE_PATTERN_LENGTH;
            }
        }
        else {
            cannonFrame--;
            if (cannonFrame <= 0) {
                cannonFrame = 0;
                laserState = LaserState::CHARGING;
                laserFrame = 0;
                chargeTimer = 0.0f;
                laserFrameTimer = 0.0f;
                laserFlashActive = true;
                laserFlashStep = 0;
                laserFlashFrame = CHARGE_PATTERN[0];
                laserFlashTimer = 0.0f;
                laserFlashDelay = CHARGE_DURATION / (float)CHARGE_PATTERN_LENGTH;
            }
        }
        break;

    case LaserState::CHARGING:
        chargeTimer += dt;
        if (laserFrameTimer >= laserFrameDelay) {
            laserFrameTimer = 0.0f;
            laserFrame = (laserFrame + 1) % CHARGE_FRAMES;
        }
        if (chargeTimer >= CHARGE_DURATION) {
            if (cannonGoingUp) {
                laserState = LaserState::CHARGING2;
                laserFrame = 0;
                laserFrameTimer = 0.0f;
                laserBeamActive = false;
            }
            else {
                laserState = LaserState::FIRING;
                laserFrame = 0;
                laserTimer = 0.0f;
                laserFrameTimer = 0.0f;
                laserFlashFrame = 0;
                laserFlashTimer = 0.0f;
                laserFlashDelay = LASER_DURATION / (float)LASER_FIRE_FRAMES;
                laserBeamX = posX + laserOffsetX;
                flashLoopTimer = 0.0f;
                laserBeamActive = true;
                laserBeamVisible = true;
                beamUpVisible = false;
            }
        }
        break;

    case LaserState::CHARGING2:
        if (laserFrameTimer >= charging2FrameDelay) {
            laserFrameTimer = 0.0f;
            laserFrame++;
            if (laserFrame >= CHARGE2_FRAMES) {
                laserState = LaserState::FIRING;
                laserFrame = 0;
                laserTimer = 0.0f;
                laserFrameTimer = 0.0f;
                laserFlashFrame = cannonGoingUp ? 15 : 0;
                laserFlashTimer = 0.0f;
                laserFlashDelay = LASER_DURATION / (float)LASER_FIRE_FRAMES;
                laserBeamX = posX + laserOffsetX;
                laserBeamActive = false;
                flashLoopTimer = 0.0f;
                laserBeamVisible = false;
            }
        }
        break;

    case LaserState::FIRING:
        laserTimer += dt;

        if (cannonGoingUp) {
            beamUpVisible = true;
            // Fase disparo: durante LASER_DURATION, cañón congelado en frame 5
            if (laserTimer <= LASER_DURATION) {
                laserFrame = 4;  // congelado

                // beam loop 0-1
                beamRetracting = false;
                beamTimer += dt;
                if (beamTimer >= beamDelay) {
                    beamTimer = 0.0f;
                    beamFrame = (beamFrame + 1) % BEAM_FRAMES;
                }

                // explo loop 16-17 — timer separado
                flashLoopTimer += dt;
                laserFlashRowY = 1.0f;
                if (flashLoopTimer >= 0.1f) {
                    flashLoopTimer = 0.0f;
                    laserFlashFrame = (laserFlashFrame == 15) ? 16 : 15;
                }
            }
            else {
                // Fase recogida: animar frames 6-10
                if (laserFrameTimer >= laserFrameDelay) {
                    laserFrameTimer = 0.0f;
                    if (laserFrame < 10)
                        laserFrame++;
                    else if (laserFrame == 5)  // primera vez que entra
                        laserFrame = 6;
                }

                beamRetracting = true;
                beamDelay = (5.0f * laserFrameDelay) / (float)BEAM_RET_FRAMES;
                beamTimer += dt;
                if (beamTimer >= beamDelay) {
                    beamTimer = 0.0f;
                    if (beamFrame < BEAM_RET_FRAMES - 1) beamFrame++;
                }
                // Mapeo flash según frame del beam de recogida
                if (beamFrame <= 3) { laserFlashFrame = 15; laserFlashRowY = 1.0f; }
                else if (beamFrame <= 5) { laserFlashFrame = 16; laserFlashRowY = 1.0f; }
                else if (beamFrame == 6) { laserFlashFrame = 3;  laserFlashRowY = 0.0f; }
                else { laserFlashFrame = 1;  laserFlashRowY = 0.0f; }

                if (laserFrame >= 10 && beamFrame >= BEAM_RET_FRAMES - 1) {
                    if (!splatterActive && !splatterDone) {
                        splatterActive = true;
                        splatterFrame = 0;
                        splatterTimer = 0.0f;
                        // Cañón se pone en posición de inicio de bajada pero no se mueve
                        cannonGoingUp = false;
                        cannonFrame = CANNON_FRAMES - 1;
                        laserState = LaserState::MOVING;
                        laserFrame = 0;
                        laserFrameTimer = 0.0f;
                        beamFrame = 0;
                        beamRetracting = false;
                        beamUpVisible = false;
                        laserBeamActive = false;
                        laserBeamVisible = false;
                    }
                }
            }
        }
        else {
            // Abajo — animar los 10 frames mientras dura el beam
            if (laserFrameTimer >= laserFrameDelay) {
                laserFrameTimer = 0.0f;
                if (laserFrame < FIRE_FRAMES - 1)
                    laserFrame++;
                // cuando llega al último frame se queda ahí hasta que el beam desaparezca
            }
            if (laserTimer >= LASER_DURATION && !laserBeamActive) {
                cannonGoingUp = !cannonGoingUp;
                laserState = LaserState::MOVING;
                laserFrame = 0;
                laserFrameTimer = 0.0f;
                laserBeamVisible = false;
                beamUpVisible = false;
            }
        }
        break;
    }
}

void Boss::UpdateLaserFlash(float dt)
{
    laserFlashTimer += dt;
    if (laserFlashTimer < laserFlashDelay) return;
    laserFlashTimer = 0.0f;

    if (laserState == LaserState::CHARGING) {
        laserFlashStep++;
        if (laserFlashStep >= CHARGE_PATTERN_LENGTH)
            laserFlashStep = CHARGE_PATTERN_LENGTH - 1;
        laserFlashFrame = CHARGE_PATTERN[laserFlashStep];
    }
    else if (laserState == LaserState::FIRING && !cannonGoingUp) {
        // Solo abajo — arriba lo maneja UpdateLaser directamente
        laserFlashFrame++;
        if (laserFlashFrame >= LASER_FIRE_FRAMES)
            laserFlashFrame = LASER_FIRE_FRAMES - 1;
    }
}

void Boss::Draw()
{
    if (introState == IntroState::IDLE && !playerInRange) return;
    if (introState != IntroState::DONE) {
        Rectangle src = { cannonFrame * CANNON_FRAME_W, 0.0f, CANNON_FRAME_W, CANNON_FRAME_H };
        Rectangle dst = { posX, posY, CANNON_FRAME_W * CANNON_SCALE, CANNON_FRAME_H * CANNON_SCALE };
        DrawTexturePro(cannonSheet, src, dst, { 0, 0 }, 0.0f, WHITE);
        DrawTent();
        return;
    }

    if (destroyed) {
        if (cannonDestroyedVisible && cannonDestroyedTimer < CANNON_DESTROYED_DURATION) {
            Rectangle src = { 0, 0, CANNON_DESTROYED_W, CANNON_DESTROYED_H };
            float centerOffsetX = (CANNON_FRAME_W * CANNON_SCALE - CANNON_DESTROYED_W * CANNON_SCALE) / 2.0f;
            float centerOffsetY = (CANNON_FRAME_H * CANNON_SCALE - CANNON_DESTROYED_H * CANNON_SCALE) / 2.0f;
            Rectangle dst = { 16160.0f, 220.0f, CANNON_DESTROYED_W * CANNON_SCALE, CANNON_DESTROYED_H * CANNON_SCALE };
            DrawTexturePro(cannonDestroyedSheet, src, dst, { 0,0 }, 0.0f, WHITE);
        }

        // Dibujar explosiones
        for (int i = 0; i < MAX_BOSS_EXPLOSIONS; i++) {
            if (!bossExplosions[i].active) continue;
            if (bossExplosions[i].type == 0) {
                Rectangle src = { bossExplosions[i].frame * EXPLO1_W, 0, EXPLO1_W, EXPLO1_H };
                Rectangle dst = { bossExplosions[i].pos.x, bossExplosions[i].pos.y,
                                  EXPLO1_W * CANNON_SCALE, EXPLO1_H * CANNON_SCALE };
                DrawTexturePro(explo1Sheet, src, dst, { 0,0 }, 0.0f, WHITE);
            }
            else {
                Rectangle src = { bossExplosions[i].frame * EXPLO2_W, 0, EXPLO2_W, EXPLO2_H };
                Rectangle dst = { bossExplosions[i].pos.x, bossExplosions[i].pos.y,
                                  EXPLO2_W * CANNON_SCALE, EXPLO2_H * CANNON_SCALE };
                DrawTexturePro(explo2Sheet, src, dst, { 0,0 }, 0.0f, WHITE);
            }
        }
        // Dibujar debris
        for (int i = 0; i < MAX_BOSS_DEBRIS; i++) {
            if (!bossDebris[i].active) continue;
            int maxFrames = (bossDebris[i].spriteRow < 4) ? 8 : 10;
            Rectangle src = {
                bossDebris[i].frame * DEBRIS_W,
                bossDebris[i].spriteRow * DEBRIS_H,
                DEBRIS_W, DEBRIS_H
            };
            Rectangle dst = {
                bossDebris[i].pos.x, bossDebris[i].pos.y,
                DEBRIS_W * CANNON_SCALE, DEBRIS_H * CANNON_SCALE
            };
            DrawTexturePro(debrisSheet, src, dst, { 0,0 }, 0.0f, WHITE);
        }

        DrawPlasma();
        return;
    }

    if (phase2) {
        DrawLaser();
    }
    else {
        Rectangle src;
        switch (cannonState)
        {
        case CannonState::OPENING_DOWN:
        case CannonState::CLOSING_DOWN:
            src = { openFrame * CANNON_FRAME_W, OPEN_DOWN_ROW_Y,  CANNON_FRAME_W, CANNON_FRAME_H };
            break;
        case CannonState::SHOOTING_DOWN:
            src = { openFrame * CANNON_FRAME_W, SHOOT_DOWN_ROW_Y, CANNON_FRAME_W, CANNON_FRAME_H };
            break;
        case CannonState::OPENING_UP:
        case CannonState::CLOSING_UP:
            src = { openFrame * CANNON_FRAME_W, OPEN_UP_ROW_Y,    CANNON_FRAME_W, CANNON_FRAME_H };
            break;
        case CannonState::SHOOTING_UP:
            src = { openFrame * CANNON_FRAME_W, SHOOT_UP_ROW_Y,   CANNON_FRAME_W, CANNON_FRAME_H };
            break;
        default:
            src = { cannonFrame * CANNON_FRAME_W, 0.0f, CANNON_FRAME_W, CANNON_FRAME_H };
            break;
        }
        float drawY = posY;
        if (cannonState == CannonState::OPENING_UP ||
            cannonState == CannonState::SHOOTING_UP ||
            cannonState == CannonState::CLOSING_UP)
            drawY = posY - 150.0f;
        Rectangle dst = { posX, drawY, CANNON_FRAME_W * CANNON_SCALE, CANNON_FRAME_H * CANNON_SCALE };
        Color tint = (isFlashing && hitFlashCount % 2 == 0) ? Color{ 255, 220, 100, 255 } : WHITE;
        lastCannonDrawX = posX;
        lastCannonDrawY = drawY;
        DrawTexturePro(cannonSheet, src, dst, { 0, 0 }, 0.0f, tint);
        if (flashActive) {
            Rectangle flashSrc = {
                flashFrame * FLASH_FRAME_W,
                FLASH_ROW_Y,
                FLASH_FRAME_W,
                FLASH_FRAME_H
            };
            bool isDown = (cannonState == CannonState::SHOOTING_DOWN);
            float flashDrawY = posY;
            if (!isDown) flashDrawY += offsetUpY;
            Rectangle flashDst = { posX + offsetDownX, flashDrawY, CANNON_FRAME_W * CANNON_SCALE, CANNON_FRAME_H * CANNON_SCALE };
            DrawTexturePro(flashSheet, flashSrc, flashDst, { 0, 0 }, 0.0f, WHITE);
        }
    }

    if (splatterActive) {
        float beamY = posY + laserOffsetUpY + (LASER_FRAME_H * CANNON_SCALE) / 2.0f
            - (SPLATTER_H * CANNON_SCALE) / 2.0f + 30.0f;
        Rectangle splSrc = { splatterFrame * SPLATTER_W, 0.0f, SPLATTER_W, SPLATTER_H };
        float x = posX + laserOffsetX + 250.0f;
        while (x > -500.0f) {
            x -= SPLATTER_W * CANNON_SCALE;
            Rectangle splDst = { x, beamY, SPLATTER_W * CANNON_SCALE, SPLATTER_H * CANNON_SCALE };
            DrawTexturePro(splatterSheet, splSrc, splDst, { 0,0 }, 0.0f, WHITE);
        }
    }

    DrawPlasma();
}

void Boss::FirePlasma()
{
    audioManager.PlaySound(audioManager.GetBossBulletSound());
    for (int i = 0; i < MAX_PLASMA; i++) {
        if (plasma[i].active) continue;

        float spawnX = posX + (CANNON_FRAME_W * CANNON_SCALE) * 0.3f +30.0f;
        float spawnY = (cannonState == CannonState::SHOOTING_DOWN ||
            cannonState == CannonState::OPENING_DOWN ||
            cannonState == CannonState::CLOSING_DOWN)
            ? 285.0f
            : posY;

        plasma[i].pos = { spawnX, spawnY };

        float targetX = capturedPlayerX +
            (float)GetRandomValue(-(int)plasmaSpread, (int)plasmaSpread);

        // Detectar suelo bajo targetX
        float groundY = player.GetPosition().y + 300.0f;
        const auto& colliders = creationManager.GetTileMap().GetColliders();
        for (const auto& col : colliders) {
            if (col.type != TileType::SOLID) continue;
            if (col.rect.x <= targetX && col.rect.x + col.rect.width >= targetX) {
                if (col.rect.y > spawnY && col.rect.y < groundY)
                    groundY = col.rect.y;
            }
        }

        float dy = groundY - spawnY;
        float timeToLand = 2.0f;
        float g = plasma[i].gravity;

        plasma[i].vel.x = (targetX - spawnX) / timeToLand;
        plasma[i].vel.y = (dy - 0.5f * g * timeToLand * timeToLand) / timeToLand;
        plasma[i].active = true;
        break;
    }
}

void Boss::UpdatePlasma(float dt)
{
    const auto& colliders = creationManager.GetTileMap().GetColliders();

    for (int i = 0; i < MAX_PLASMA; i++) {
        if (!plasma[i].active) continue;

        // Guardar posición en el trail
        plasma[i].trailTimer += dt;
        if (plasma[i].trailTimer >= plasma[i].trailDelay) {
            plasma[i].trailTimer = 0.0f;
            // Desplazar trail hacia atrás
            for (int t = PlasmaBall::TRAIL_LENGTH - 1; t > 0; t--)
                plasma[i].trail[t] = plasma[i].trail[t - 1];
            plasma[i].trail[0] = plasma[i].pos;
            if (plasma[i].trailCount < PlasmaBall::TRAIL_LENGTH)
                plasma[i].trailCount++;
        }

        // Actualizar animación de cada punto del trail
        for (int t = 0; t < plasma[i].trailCount; t++) {
            plasma[i].trailAnimTimer[t] += dt;
            if (plasma[i].trailAnimTimer[t] >= PlasmaBall::TRAIL_ANIM_DELAY) {
                plasma[i].trailAnimTimer[t] = 0.0f;
                plasma[i].trailAnimFrame[t] = (plasma[i].trailAnimFrame[t] + 1) % PlasmaBall::TRAIL_ANIM_ROWS;
            }
        }

        plasma[i].vel.y += plasma[i].gravity * dt;
        plasma[i].pos.x += plasma[i].vel.x * dt;
        plasma[i].pos.y += plasma[i].vel.y * dt;

        Rectangle ball = {
            plasma[i].pos.x - plasmaRadius,
            plasma[i].pos.y - plasmaRadius,
            plasmaRadius * 2,
            plasmaRadius * 2
        };

        for (const auto& col : colliders) {
            if (col.type != TileType::SOLID) continue;
            if (CheckCollisionRecs(ball, col.rect)) {
                plasma[i].active = false;
                plasma[i].trailCount = 0;
                break;
            }
        }
    }
}

void Boss::DrawPlasma() const
{
    for (int i = 0; i < MAX_PLASMA; i++) {
        if (!plasma[i].active) continue;

        // Dibujar trail — del más viejo al más nuevo
        for (int t = plasma[i].trailCount - 1; t >= 0; t--) {
            Rectangle src = {
                (float)t * PLASMA_FRAME_W,  // cada punto del trail = un sprite diferente
                plasma[i].trailAnimFrame[t] * PLASMA_FRAME_H, 
                PLASMA_FRAME_W, 
                PLASMA_FRAME_H 
            };

            Rectangle dst = {
                plasma[i].trail[t].x - (PLASMA_FRAME_W * PLASMA_SCALE) / 2.0f,
                plasma[i].trail[t].y - (PLASMA_FRAME_H * PLASMA_SCALE) / 2.0f,
                PLASMA_FRAME_W * PLASMA_SCALE,
                PLASMA_FRAME_H * PLASMA_SCALE
            };
            DrawTexturePro(plasmaSheet, src, dst, { 0, 0 }, 0.0f, WHITE);
        }

        // Dibujar cabeza — sprite 0 (el más brillante)
        Rectangle src = { 0.0f, plasma[i].trailAnimFrame[0] * PLASMA_FRAME_H, PLASMA_FRAME_W, PLASMA_FRAME_H };
        Rectangle dst = {
            plasma[i].pos.x - (PLASMA_FRAME_W * PLASMA_SCALE) / 2.0f,
            plasma[i].pos.y - (PLASMA_FRAME_H * PLASMA_SCALE) / 2.0f,
            PLASMA_FRAME_W * PLASMA_SCALE,
            PLASMA_FRAME_H * PLASMA_SCALE
        };
        DrawTexturePro(plasmaSheet, src, dst, { 0, 0 }, 0.0f, WHITE);
    }
}

void Boss::DrawLaser() const
{
    if (destroyed) return;

    if (laserState == LaserState::MOVING) {
        Rectangle src = { cannonFrame * CANNON_FRAME_W, 0.0f, CANNON_FRAME_W, CANNON_FRAME_H };
        Rectangle dst = { posX, posY, CANNON_FRAME_W * CANNON_SCALE, CANNON_FRAME_H * CANNON_SCALE };
        Color tint = (isFlashing && hitFlashCount % 2 == 0) ? Color{ 255,220,100,255 } : WHITE;
        DrawTexturePro(cannonSheet, src, dst, { 0,0 }, 0.0f, tint);
        return;
    }

    float rowY = 0.0f;
    if (laserState == LaserState::CHARGING)
        rowY = cannonGoingUp ? CHARGE_UP_ROW_Y : CHARGE_DOWN_ROW_Y;
    else
        rowY = cannonGoingUp ? FIRE_UP_ROW_Y : FIRE_DOWN_ROW_Y;

    // 1. Beam arriba — detrás de todo
    if (laserState == LaserState::FIRING && cannonGoingUp) {
        float beamY = posY + laserOffsetUpY + (LASER_FRAME_H * CANNON_SCALE) / 2.0f
            - (BEAM_H * CANNON_SCALE) / 2.0f + 30.0f;
        float beamRowY = beamRetracting ? BEAM_H : 0.0f;
        int   maxFrames = beamRetracting ? BEAM_RET_FRAMES : BEAM_FRAMES;
        int   drawFrame = (beamFrame < maxFrames) ? beamFrame : maxFrames - 1;
        Rectangle beamSrc = { drawFrame * BEAM_W, beamRowY, BEAM_W, BEAM_H };

        float x = posX + laserOffsetX + 250.0f;
        while (x > -500.0f) {
            x -= BEAM_W * CANNON_SCALE;
            Rectangle beamDst = { x, beamY, BEAM_W * CANNON_SCALE, BEAM_H * CANNON_SCALE };
            DrawTexturePro(beamSheet, beamSrc, beamDst, { 0,0 }, 0.0f, WHITE);
        }
    }
    if (laserState == LaserState::FIRING && cannonGoingUp && !beamRetracting) {
        Rectangle beamBox = GetBeamUpHitBox();
        DrawRectangleLinesEx(beamBox, 2, RED);
    }

    // 2. Beam abajo — detrás de todo
    if (laserBeamVisible) {
        float beamY = posY + laserOffsetDownY + (LASER_FRAME_H * CANNON_SCALE) / 2.0f
            - (LASER_BEAM_H * CANNON_SCALE) / 2.0f;
        Rectangle beamSrc = { laserBeamFrame * LASER_BEAM_W, 0.0f, LASER_BEAM_W, LASER_BEAM_H };
        Rectangle beamDst = { laserBeamX, beamY, LASER_BEAM_W * CANNON_SCALE, LASER_BEAM_H * CANNON_SCALE };
        DrawTexturePro(laserBeamSheet, beamSrc, beamDst, { 0,0 }, 0.0f, WHITE);
    }

    if (laserBeamVisible) {
    Rectangle beamBox = GetLaserBeamHitBox();
    DrawRectangleLinesEx(beamBox, 2, RED);
}

    // 3. sprite 15 — detrás del cañón
    if (laserState == LaserState::FIRING && cannonGoingUp && laserFlashFrame == 15)
        DrawLaserFlash();

    // 4. Cañón
    Rectangle src;
    if (laserState == LaserState::FIRING && cannonGoingUp && laserTimer <= LASER_DURATION)
        src = { 4 * LASER_FRAME_W, FIRE_UP_ROW_Y, LASER_FRAME_W, LASER_FRAME_H };
    else if (laserState == LaserState::CHARGING2)
        src = { (laserFrame % 2) * LASER_FRAME_W, CHARGE_UP_ROW_Y, LASER_FRAME_W, LASER_FRAME_H };
    else
        src = { laserFrame * LASER_FRAME_W, rowY, LASER_FRAME_W, LASER_FRAME_H };

        bool isDown = !cannonGoingUp;
        float laserDrawY = posY + (isDown ? laserOffsetDownY : laserOffsetUpY);
        Rectangle dst = { posX + laserOffsetX, laserDrawY, LASER_FRAME_W * CANNON_SCALE, LASER_FRAME_H * CANNON_SCALE };
        Color tint = (isFlashing && hitFlashCount % 2 == 0) ? Color{ 255,220,100,255 } : WHITE;
        DrawTexturePro(laserSheet, src, dst, { 0,0 }, 0.0f, tint);
    

    // 5. sprite 16 y CHARGING — delante del cañón
    if (laserState == LaserState::CHARGING ||
        laserState == LaserState::CHARGING2 ||
        (laserState == LaserState::FIRING && cannonGoingUp && laserFlashFrame != 15))
        DrawLaserFlash();
}

void Boss::DrawTent() const
{
    if (introState == IntroState::DONE) return;

    int frame = (introState == IntroState::IDLE || introState == IntroState::WAITING)
        ? 0  // fila 0, sprite estático
        : tentFrame;  // fila 1, animación

    float rowY = (introState == IntroState::IDLE || introState == IntroState::WAITING)
        ? 0.0f
        : TENT_FRAME_H;  // fila 1

    Rectangle src = { frame * TENT_FRAME_W, rowY, TENT_FRAME_W, TENT_FRAME_H };
    Rectangle dst = { posX + tentOffsetX +50.0f, posY -40.0f, TENT_FRAME_W * TENT_SCALE, TENT_FRAME_H * TENT_SCALE };
    DrawTexturePro(tentSheet, src, dst, { 0, 0 }, 0.0f, WHITE);
}

void Boss::DrawLaserFlash() const
{
    bool isDown = !cannonGoingUp;
    float flashDrawY = posY + (isDown ? laserOffsetDownY - 5.0f : laserOffsetUpY);
    Rectangle dst = { posX + laserOffsetX, flashDrawY,
                      LASER_FLASH_FRAME_W * CANNON_SCALE, LASER_FLASH_FRAME_H * CANNON_SCALE };

    if (laserState == LaserState::CHARGING) {
        Rectangle src = { laserFlashFrame * LASER_FLASH_FRAME_W, 0.0f,
                          LASER_FLASH_FRAME_W, LASER_FLASH_FRAME_H };
        DrawTexturePro(laserFlashSheet, src, dst, { 0,0 }, 0.0f, WHITE);
    }
    else if (laserState == LaserState::CHARGING2) {
        // fila 1, sprites 0-14
        Rectangle src = { laserFrame * LASER_FLASH_FRAME_W, LASER_FLASH_FRAME_H,
                          LASER_FLASH_FRAME_W, LASER_FLASH_FRAME_H };
        DrawTexturePro(laserFlashSheet, src, dst, { 0,0 }, 0.0f, WHITE);
    }
    else if (laserState == LaserState::FIRING && cannonGoingUp) {
        Rectangle src = { laserFlashFrame * LASER_FLASH_FRAME_W, laserFlashRowY * LASER_FLASH_FRAME_H,
                          LASER_FLASH_FRAME_W, LASER_FLASH_FRAME_H };
        DrawTexturePro(laserFlashSheet, src, dst, { 0,0 }, 0.0f, WHITE);
    }
}
