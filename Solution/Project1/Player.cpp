#include "Player.hpp"

Player::Player() {
    anim.LoadTextures();
    SetNormalHitbox();
}

Player::~Player() {
    anim.UnloadTextures();
}

void Player::SetNormalHitbox() {
    hitboxWidth = 20.0f * SCALE;
    hitboxHeight = 40.0f * SCALE;
    hitboxOffsetX = 0.0f * SCALE;
    hitboxOffsetY = 0.0f * SCALE;
}

void Player::SetCrouchHitbox() {
    hitboxWidth = 20.0f * SCALE;
    hitboxHeight = 25.0f * SCALE;
    hitboxOffsetX = 0.0f * SCALE;
    hitboxOffsetY = 0.0f * SCALE;
}

void Player::Update(float CameraLeftLimit) {
    // ========== ACTUALIZAR HITBOX SEGÚN ESTADO ==========
    if (crouching) {
        SetCrouchHitbox();
    }
    else {
        SetNormalHitbox();
    }

    anim.Update(grounded, inputVelX, crouching, aimingUp, GetFrameTime());

    if (mode == Mode::FULL_BODY && !crouching) {
        specialTimer += GetFrameTime();
        if (specialTimer >= specialDuration) {
            mode = Mode::SEPARATED;
            special = SpecialAnim::NONE;
        }
        pos.x += vel.x;
        if (pos.x < CameraLeftLimit) pos.x = CameraLeftLimit;
        return;
    }

    // ========== FÍSICA ==========
    if (!grounded) {
        vel.y += GRAVITY;
        pos.y += vel.y;
    }
    else {
        vel.y = 0;
        // No mover Y
    }

    // Actualizar X normalmente
    pos.x += vel.x;

    // Limitar por cámara
    if (pos.x < CameraLeftLimit) pos.x = CameraLeftLimit;
}

void Player::Draw() {
    if (crouching) {
        DrawCrouch();
        DrawHitBox();

        // DEBUG: Mostrar pos.x y pos.y
        DrawText(TextFormat("pos: (%.0f, %.0f)", pos.x, pos.y), pos.x, pos.y - 30, 20, YELLOW);
        return;
    }
    if (mode == Mode::FULL_BODY) {
        DrawFullBody();
    }
    else {
        DrawSeparated();
    }
    DrawHitBox();

    // DEBUG: Mostrar pos.x y pos.y
    DrawText(TextFormat("pos: (%.0f, %.0f)", pos.x, pos.y), pos.x, pos.y - 30, 20, YELLOW);
}

void Player::DrawCrouch() {
    Rectangle sourceRect;
    float currentHeight;
    float rowY;
    float yOffset;
    float drawX = pos.x;

    // PRIORIDAD 1: Disparo agachado
    if (anim.IsCrouchShooting()) {
        rowY = anim.GetCrouchShootRowY();
        currentHeight = 34.0f;
        yOffset = -30.0f; //65 to - 28

        // CADA SPRITE OCUPA 68 PÍXELES DE ANCHO
        sourceRect = {
            (float)(anim.GetCrouchShootFrame() * 68.0f),  // ← 68, no 34
            rowY,
            anim.GetCrouchShootW(),  // 68px de ancho
            currentHeight
        };

        if (dir == PlayerDirection::LEFT) {
            sourceRect.width = -anim.GetCrouchShootW();
            drawX = pos.x - (anim.GetCrouchShootW() - 34.0f) * SCALE;
        }
    }
    // PRIORIDAD 2: Caminar agachado
    else if (anim.IsCrouchWalking()) {
        rowY = anim.GetCrouchWalkRowY();
        currentHeight = 34.0f;
        yOffset = -15.0f;
        sourceRect = {
            (float)(anim.GetCrouchWalkFrame() * 34.0f),
            rowY,
            34.0f,
            currentHeight
        };
        if (dir == PlayerDirection::LEFT) {
            sourceRect.width = -34.0f;
        }
    }
    // PRIORIDAD 3: Transición de agachado
    else if (anim.IsCrouchTransition()) {
        rowY = 18 * 34.0f;
        currentHeight = 68.0f;
        yOffset = -165.0f;
        sourceRect = {
            (float)(anim.GetCrouchFrame() * 34.0f),
            rowY,
            34.0f,
            currentHeight
        };
        if (dir == PlayerDirection::LEFT) {
            sourceRect.width = -34.0f;
        }
    }
    // PRIORIDAD 4: Idle agachado
    else {
        rowY = 20 * 34.0f;
        currentHeight = 34.0f;
        yOffset = -15.0f;
        sourceRect = {
            (float)(anim.GetCrouchFrame() * 34.0f),
            rowY,
            34.0f,
            currentHeight
        };
        if (dir == PlayerDirection::LEFT) {
            sourceRect.width = -34.0f;
        }
    }

    Rectangle destRect = {
        drawX,
        pos.y + yOffset,
        (anim.IsCrouchShooting() ? anim.GetCrouchShootW() : 34.0f) * SCALE,
        currentHeight * SCALE
    };

    DrawTexturePro(anim.GetSheet(), sourceRect, destRect, { 0,0 }, 0, WHITE);
}

void Player::DrawSeparated() {
    float w = anim.GetW(), h = anim.GetH();
    float baseY = pos.y + GetHeight() - (h * SCALE) - 35.0f;
    VisualOffsets off = anim.GetOffsets();

    // ========== INVERTIR OFFSETS PARA IZQUIERDA (APLICA A TODO) ==========
    float legsOffsetX = off.legsX;
    float torsoOffsetX = off.torsoX;

    if (dir == PlayerDirection::LEFT) {
        legsOffsetX = -off.legsX;
        torsoOffsetX = -off.torsoX;  // ← INVERTIR PARA TODOS LOS CASOS
    }

    // ========== 1. PIERNAS ==========
    Rectangle legSrc;
    switch (anim.GetLegsAnim()) {
    case LegsAnim::WALKING:
        legSrc = { anim.GetWalkLegsFrame() * w, anim.GetRowWalkLegs(), w, h };
        break;
    case LegsAnim::JUMPING:
        legSrc = { anim.GetJumpLegsFrame() * w, anim.GetRowJumpLegs(), w, h };
        break;
    default:
        legSrc = { 4 * w, anim.GetRowIdle(), w, h };
    }

    if (dir == PlayerDirection::LEFT) {
        legSrc.width = -w;
    }

    DrawTexturePro(anim.GetSheet(), legSrc,
        { pos.x + legsOffsetX * SCALE, baseY + off.legsY * SCALE, w * SCALE, h * SCALE },
        { 0,0 }, 0, WHITE);

    // ========== 2. TORSO ==========
    float torsoDrawX = pos.x + torsoOffsetX * SCALE;
    float torsoDrawY = baseY + off.torsoY * SCALE;
    Rectangle torsoSrc;
    Color tint = WHITE;

    // DEBUG
    TraceLog(LOG_INFO, "TORSO DEBUG - dir: %s, torsoOffsetX: %.2f, torsoDrawX: %.2f",
        dir == PlayerDirection::LEFT ? "LEFT" : "RIGHT", torsoOffsetX, torsoDrawX);

    // PRIORIDAD 1: Disparo normal (horizontal)
    if (anim.IsShooting()) {
        torsoSrc = { anim.GetShootFrame() * anim.GetShootW(), anim.GetRowShoot(), anim.GetShootW(), h };

        float shootDrawX = torsoDrawX;
        float shootDrawY = torsoDrawY;

        if (dir == PlayerDirection::LEFT) {
            torsoSrc.width = -anim.GetShootW();
            float compensacion = -140.0;
            shootDrawX = torsoDrawX + compensacion;
        }

        float subirTorsoY = 5.0f;  
        shootDrawY = torsoDrawY - subirTorsoY;

        DrawTexturePro(anim.GetSheet(), torsoSrc,
            { shootDrawX, shootDrawY, anim.GetShootW() * SCALE, h * SCALE },
            { 0,0 }, 0, tint);
        return;
    }

    // PRIORIDAD 2: Disparo hacia arriba (shooting up)
    if (aimingUp && anim.IsShootingUp()) {
        torsoSrc = {
            (float)(anim.GetShootUpFrame() * w),
            anim.GetShootUpRowY(),
            w,
            anim.GetShootUpH()
        };
        if (dir == PlayerDirection::LEFT) {
            torsoSrc.width = -w;
        }
        float shootUpBaseY = baseY - 140 + (off.torsoY * SCALE);
        DrawTexturePro(anim.GetSheet(), torsoSrc,
            { torsoDrawX, shootUpBaseY, w * SCALE, anim.GetShootUpH() * SCALE },
            { 0,0 }, 0, tint);
        return;
    }

    // PRIORIDAD 3: Aiming up (solo apuntar)
    if (aimingUp) {
        if (anim.IsAimingTransition()) {
            torsoSrc = {
                (float)(anim.GetAimingFrame() * w),
                anim.GetAimingTransitionRowY(),
                w, h
            };
        }
        else {
            torsoSrc = {
                (float)(anim.GetAimingFrame() * w),
                anim.GetAimingIdleRowY(),
                w, h
            };
        }
    }
    // PRIORIDAD 4: Torso normal
    else {
        switch (anim.GetTorsoAnim()) {
        case TorsoAnim::WALKING:
            torsoSrc = { anim.GetWalkTorsoFrame() * w, anim.GetRowWalkTorso(), w, h };
            break;
        case TorsoAnim::JUMPING:
            torsoSrc = { anim.GetJumpTorsoFrame() * w, anim.GetRowJumpTorso(), w, h };
            break;
        default:
            torsoSrc = { anim.GetIdleFrame() * w, anim.GetRowIdle(), w, h };
        }
    }

    if (dir == PlayerDirection::LEFT) {
        torsoSrc.width = -w;
    }

    DrawTexturePro(anim.GetSheet(), torsoSrc,
        { torsoDrawX, torsoDrawY, w * SCALE, h * SCALE },
        { 0,0 }, 0, tint);
}

void Player::DrawFullBody() {
    Rectangle sourceRect = GetFullBodyRect();
    if (dir == PlayerDirection::LEFT) sourceRect.width = -sourceRect.width;
    float currentHeight = GetFullBodyH();

    Rectangle destRect = {
        pos.x,
        pos.y + (34.0f - currentHeight) * SCALE,
        GetWidth(),
        currentHeight * SCALE
    };

    DrawTexturePro(anim.GetSheet(), sourceRect, destRect, { 0,0 }, 0, aimingUp ? YELLOW : WHITE);
}

void Player::DrawHitBox() {
    // Hitbox principal (blanca)
    DrawRectangleLinesEx(GetHitBox(), 2, WHITE);

    // Hitbox izquierda (roja)
    DrawRectangleLinesEx(GetLeftHitBox(), 2, RED);

    // Hitbox derecha (azul)
    DrawRectangleLinesEx(GetRightHitBox(), 2, BLUE);
}

Rectangle Player::GetFullBodyRect() {
    int row = 0;
    int h = 34;
    switch (special) {
    case SpecialAnim::CROUCH: row = 0; h = 20; break;
    case SpecialAnim::CROUCH_SHOOT: row = 1; h = 20; break;
    case SpecialAnim::FALLING_START: row = 2; h = 48; break;
    default: break;
    }
    return { 0, row * 34.0f, 34.0f, (float)h };
}

float Player::GetFullBodyH() const {
    switch (special) {
    case SpecialAnim::CROUCH:
    case SpecialAnim::CROUCH_SHOOT:
        return 20.0f;
    case SpecialAnim::FALLING_START:
        return 48.0f;
    default:
        return 34.0f;
    }
}

float Player::GetHeight() const {
    if (mode == Mode::FULL_BODY) return GetFullBodyH() * SCALE;
    return hitboxHeight;
}

Rectangle Player::GetHitBox() {
    // Calcular hitbox centrada en el personaje
    float spriteTotalWidth = 34.0f * SCALE;  // 136 píxeles
    float hitboxX = pos.x + (spriteTotalWidth - hitboxWidth) / 2.0f;
    float hitboxY = pos.y + hitboxOffsetY;

    // DEBUG
    TraceLog(LOG_INFO, "=== GET HITBOX (NUEVA CENTRADA) ===");
    TraceLog(LOG_INFO, "dir: %s", dir == PlayerDirection::LEFT ? "LEFT" : "RIGHT");
    TraceLog(LOG_INFO, "grounded: %d, crouching: %d", grounded, crouching);
    TraceLog(LOG_INFO, "pos: (%.1f, %.1f)", pos.x, pos.y);
    TraceLog(LOG_INFO, "spriteTotalWidth: %.1f, hitboxWidth: %.1f", spriteTotalWidth, hitboxWidth);
    TraceLog(LOG_INFO, "hitboxX: %.1f, hitboxY: %.1f", hitboxX, hitboxY);
    TraceLog(LOG_INFO, "hitboxWidth: %.1f, hitboxHeight: %.1f", hitboxWidth, hitboxHeight);

    return Rectangle{ hitboxX, hitboxY, hitboxWidth, hitboxHeight };
}

Rectangle Player::GetLeftHitBox() {
    Rectangle mainHitBox = GetHitBox();

    // La hitbox izquierda es más pequeña en altura
    float reducedHeight = hitboxHeight * 0.6f;  // 60% de la altura original
    float offsetY = (hitboxHeight - reducedHeight) / 2.0f;  // Centrada verticalmente

    // Posición X: dentro de la hitbox principal pero sobresaliendo hacia la izquierda
    float hitboxX = GetHitBox().x - (hitboxWidth * 0.3f);  // 30% fuera hacia la izquierda
    float hitboxY = GetHitBox().y + offsetY;
    float hitboxW = hitboxWidth * 0.4f;  // 40% del ancho de la hitbox principal
    float hitboxH = reducedHeight;

    return Rectangle{ hitboxX, hitboxY, hitboxW, hitboxH };
}

Rectangle Player::GetRightHitBox() {
    Rectangle mainHitBox = GetHitBox();
    // La hitbox derecha es más pequeña en altura
    float reducedHeight = hitboxHeight * 0.6f;
    float offsetY = (hitboxHeight - reducedHeight) / 2.0f;

    // Posición X: dentro de la hitbox principal pero sobresaliendo hacia la derecha
    float hitboxX = GetHitBox().x + hitboxWidth - (hitboxWidth * 0.1f);  // 10% fuera hacia la derecha
    float hitboxY = GetHitBox().y + offsetY;
    float hitboxW = hitboxWidth * 0.4f;
    float hitboxH = reducedHeight;

    return Rectangle{ hitboxX, hitboxY, hitboxW, hitboxH };
}

Vector2 Player::GetPosition() {
    return pos;
}

// ========== MOVIMIENTO E INPUT ==========
void Player::MoveLeft() {
    if (mode != Mode::FULL_BODY && !crouching) {
        inputVelX = -MOVE_SPEED;  // Guardar velocidad deseada para animación
        if (!leftCollision) {
            vel.x = -MOVE_SPEED;
        }
        else {
            vel.x = 0;
        }
        dir = PlayerDirection::LEFT;
    }
    else if (crouching && !anim.IsCrouchShooting()) {
        inputVelX = -CROUCH_SPEED;
        if (!leftCollision) {
            vel.x = -CROUCH_SPEED;
        }
        else {
            vel.x = 0;
        }
        dir = PlayerDirection::LEFT;
    }
}

void Player::MoveRight() {
    if (mode != Mode::FULL_BODY && !crouching) {
        inputVelX = MOVE_SPEED;
        if (!rightCollision) {
            vel.x = MOVE_SPEED;
        }
        else {
            vel.x = 0;
        }
        dir = PlayerDirection::RIGHT;
    }
    else if (crouching && !anim.IsCrouchShooting()) {
        inputVelX = CROUCH_SPEED;
        if (!rightCollision) {
            vel.x = CROUCH_SPEED;
        }
        else {
            vel.x = 0;
        }
        dir = PlayerDirection::RIGHT;
    }
}

void Player::StopMovingHorizontal() {
    inputVelX = 0;
    vel.x = 0;
}

void Player::Jump() {
    if (grounded && !crouching && mode != Mode::FULL_BODY) {
        vel.y = JUMP_FORCE;
        grounded = false;
        anim.ResetJump();
    }
}

void Player::StartAimingUp() {
    if (!crouching && mode != Mode::FULL_BODY) aimingUp = true;
}

void Player::StopAimingUp() {
    aimingUp = false;
}

void Player::StartCrouching() {
    if (grounded && !crouching && mode != Mode::FULL_BODY) {
        // Guardar la posición de los pies antes de agacharse
        float feetY = pos.y + GetHeight();

        crouching = true;
        aimingUp = false;
        SetCrouchHitbox();
        anim.ForceCrouch();

        // Mantener los pies en la misma posición después de agacharse
        pos.y = feetY - GetHeight();
    }
}

void Player::StopCrouching() {
    if (crouching) {
        crouching = false;
        SetNormalHitbox();
    }
}

void Player::Shoot() {
    if (mode != Mode::FULL_BODY) {
        if (crouching) {
            // Al disparar agachado, detener el movimiento horizontal
            vel.x = 0;
            anim.StartCrouchShoot();
        }
        else if (aimingUp) {
            // Forzar la interrupción del disparo horizontal si está activo
            if (anim.IsShooting()) {
                anim.ForceStopShoot();  // ← Forzar fin del disparo horizontal
            }
            anim.StartShootUp();
        }
        else {
            anim.StartShoot();
        }
    }
}

PlayerDirection Player::GetAimDirection() const {
    if (aimingUp) return PlayerDirection::UP;
    return dir;
}