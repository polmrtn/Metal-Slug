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
        // Hit flash funciona aunque esté en intro
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

    // Hit flash
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

    if (!phase2 && !phase2Pending && health <= 5)
        phase2Pending = true;

    if (phase2)
        UpdateLaser(dt);
    else
        UpdateCannon(dt);

    UpdatePlasma(dt);
    UpdateLaserFlash(dt);

    if (phase2 && laserState == LaserState::FIRING && !cannonGoingUp && laserBeamActive) {
        laserBeamX -= laserBeamSpeed * dt;
        laserBeamTimer += dt;
        if (laserBeamTimer >= laserBeamDelay) {
            laserBeamTimer = 0.0f;
            laserBeamFrame = (laserBeamFrame + 1) % LASER_BEAM_FRAMES;
        }
        if (laserBeamX < -500.0f) laserBeamActive = false;
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
                }
                cannonFrameTimer = 0.0f;
            }
        }
        break;

        // ── ABAJO ────────────────────────────────────────────────
    case CannonState::OPENING_DOWN:
        flashActive = false;
        if (cannonFrameTimer < cannonFrameDelay) return;
        cannonFrameTimer = 0.0f;
        openFrame++;
        if (openFrame >= OPEN_DOWN_FRAMES - 1) {
            openFrame = 0;
            cannonState = CannonState::SHOOTING_DOWN;
            cannonFrameTimer = 0.0f;
        }
        break;

    case CannonState::SHOOTING_DOWN:
        if (cannonFrameTimer < cannonFrameDelay) return;
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
        if (cannonFrameTimer < cannonFrameDelay) return;
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
        if (cannonFrameTimer < cannonFrameDelay) return;
        cannonFrameTimer = 0.0f;
        openFrame++;
        if (openFrame >= OPEN_UP_FRAMES - 1) {
            openFrame = 0;
            cannonState = CannonState::SHOOTING_UP;
            cannonFrameTimer = 0.0f;
        }
        break;

    case CannonState::SHOOTING_UP:
        if (cannonFrameTimer < cannonFrameDelay) return;
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
        if (cannonFrameTimer < cannonFrameDelay) return;
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
        // Alternar entre frame 0 y 1
        if (laserFrameTimer >= laserFrameDelay) {
            laserFrameTimer = 0.0f;
            laserFrame = (laserFrame + 1) % CHARGE_FRAMES;
        }
        if (chargeTimer >= CHARGE_DURATION) {
            laserState = LaserState::FIRING;
            laserFrame = 0;
            laserTimer = 0.0f;
            laserFrameTimer = 0.0f;
            laserFlashFrame = cannonGoingUp ? 15 : 0;
            laserFlashTimer = 0.0f;
            laserFlashDelay = LASER_DURATION / (float)LASER_FIRE_FRAMES;
            // Activar beam desde la boca del cañón
            laserBeamX = posX + laserOffsetX;
            laserBeamActive = true;
            flashLoopTimer = 0.0f;
        }
        break;

    case LaserState::FIRING:
        laserTimer += dt;

        if (cannonGoingUp) {
            // Fase disparo: durante LASER_DURATION, cañón congelado en frame 5
            if (laserTimer <= LASER_DURATION) {
                laserFrame = 5;  // congelado

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

                if (laserFrame >= 10) {
                    cannonGoingUp = !cannonGoingUp;
                    laserState = LaserState::MOVING;
                    laserFrame = 0;
                    laserFrameTimer = 0.0f;
                    beamFrame = 0;
                    beamRetracting = false;
                }
            }
        }
        else {
            // Abajo — sin cambios
            if (laserFrameTimer >= laserFrameDelay) {
                laserFrameTimer = 0.0f;
                laserFrame = (laserFrame + 1) % FIRE_FRAMES;
            }
            if (laserTimer >= LASER_DURATION) {
                cannonGoingUp = !cannonGoingUp;
                laserState = LaserState::MOVING;
                laserFrame = 0;
                laserFrameTimer = 0.0f;
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

    // Durante la intro dibuja el cañón en reposo y la manta encima
    if (introState != IntroState::DONE) {
        // Cañón en reposo — frame del medio
        Rectangle src = { cannonFrame * CANNON_FRAME_W, 0.0f, CANNON_FRAME_W, CANNON_FRAME_H };
        Rectangle dst = { posX, posY, CANNON_FRAME_W * CANNON_SCALE, CANNON_FRAME_H * CANNON_SCALE };
        DrawTexturePro(cannonSheet, src, dst, { 0, 0 }, 0.0f, WHITE);
        DrawTent();
        return;
    }

    if (phase2) {
        DrawLaser();
    }
    else {
        // ── Cañón ─────────────────────────────────────────────────
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
        DrawTexturePro(cannonSheet, src, dst, { 0, 0 }, 0.0f, tint);




        // ── Destello ──────────────────────────────────────────────
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
    
    DrawPlasma();
}

void Boss::FirePlasma()
{
    for (int i = 0; i < MAX_PLASMA; i++) {
        if (plasma[i].active) continue;

        float spawnX = posX + (CANNON_FRAME_W * CANNON_SCALE) * 0.3f;
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
            if (col.type == TileType::CEILING) continue;
            if (col.rect.x <= targetX && col.rect.x + col.rect.width >= targetX) {
                if (col.rect.y > spawnY && col.rect.y < groundY)
                    groundY = col.rect.y;
            }
        }

        float dy = groundY - spawnY;
        float timeToLand = 1.5f;
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
            if (col.type == TileType::CEILING) continue;
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
            - (BEAM_H * CANNON_SCALE) / 2.0f + 20.0f;
        float beamRowY = beamRetracting ? BEAM_H : 0.0f;
        int   maxFrames = beamRetracting ? BEAM_RET_FRAMES : BEAM_FRAMES;
        int   drawFrame = (beamFrame < maxFrames) ? beamFrame : maxFrames - 1;
        Rectangle beamSrc = { drawFrame * BEAM_W, beamRowY, BEAM_W, BEAM_H };

        float x = posX + laserOffsetX + 200.0f;
        while (x > -500.0f) {
            x -= BEAM_W * CANNON_SCALE;
            Rectangle beamDst = { x, beamY, BEAM_W * CANNON_SCALE, BEAM_H * CANNON_SCALE };
            DrawTexturePro(beamSheet, beamSrc, beamDst, { 0,0 }, 0.0f, WHITE);
        }
    }

    // 2. Beam abajo — detrás de todo
    if (laserState == LaserState::FIRING && !cannonGoingUp && laserBeamActive) {
        float beamY = posY + laserOffsetDownY + (LASER_FRAME_H * CANNON_SCALE) / 2.0f
            - (LASER_BEAM_H * CANNON_SCALE) / 2.0f;
        Rectangle beamSrc = { laserBeamFrame * LASER_BEAM_W, 0.0f, LASER_BEAM_W, LASER_BEAM_H };
        Rectangle beamDst = { laserBeamX, beamY, LASER_BEAM_W * CANNON_SCALE, LASER_BEAM_H * CANNON_SCALE };
        DrawTexturePro(laserBeamSheet, beamSrc, beamDst, { 0,0 }, 0.0f, WHITE);
    }

    // 3. sprite 15 — detrás del cañón
    if (laserState == LaserState::FIRING && cannonGoingUp && laserFlashFrame == 15)
        DrawLaserFlash();

    // 4. Cañón
    Rectangle src;
    if (laserState == LaserState::FIRING && cannonGoingUp && laserFrame == 5)
        src = { 5 * LASER_FRAME_W, FIRE_UP_ROW_Y, LASER_FRAME_W, LASER_FRAME_H };
    else
        src = { laserFrame * LASER_FRAME_W, rowY, LASER_FRAME_W, LASER_FRAME_H };

    bool isDown = !cannonGoingUp;
    float laserDrawY = posY + (isDown ? laserOffsetDownY : laserOffsetUpY);
    Rectangle dst = { posX + laserOffsetX, laserDrawY, LASER_FRAME_W * CANNON_SCALE, LASER_FRAME_H * CANNON_SCALE };
    Color tint = (isFlashing && hitFlashCount % 2 == 0) ? Color{ 255,220,100,255 } : WHITE;
    DrawTexturePro(laserSheet, src, dst, { 0,0 }, 0.0f, tint);

    // 5. sprite 16 y CHARGING — delante del cañón
    if (laserState == LaserState::CHARGING ||
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
        // Patrón de carga fila 0
        Rectangle src = { laserFlashFrame * LASER_FLASH_FRAME_W, 0.0f,
                          LASER_FLASH_FRAME_W, LASER_FLASH_FRAME_H };
        DrawTexturePro(laserFlashSheet, src, dst, { 0,0 }, 0.0f, WHITE);
    }
    else if (laserState == LaserState::FIRING && cannonGoingUp) {
        Rectangle src = { laserFlashFrame * LASER_FLASH_FRAME_W, laserFlashRowY * LASER_FLASH_FRAME_H,
                          LASER_FLASH_FRAME_W, LASER_FLASH_FRAME_H };
        DrawTexturePro(laserFlashSheet, src, dst, { 0,0 }, 0.0f, WHITE);
    }
}
