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
    hitboxOffsetX = 2.0f * SCALE;
    hitboxOffsetY = 8.0f * SCALE;
}

void Player::SetCrouchHitbox() {
    hitboxWidth = 20.0f * SCALE;
    hitboxHeight = 25.0f * SCALE;
    hitboxOffsetX = 2.0f * SCALE;
    hitboxOffsetY = 23.0f * SCALE;
}

void Player::Update(float CameraLeftLimit) {
    // ========== ACTUALIZAR HITBOX SEGÚN ESTADO ==========
    if (crouching) {
        SetCrouchHitbox();
    }
    else {
        SetNormalHitbox();
    }

    anim.Update(grounded, vel.x, crouching, aimingUp, GetFrameTime());

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

    // Física
    if (!grounded) {
        vel.y += GRAVITY;
    }
    else if (vel.y > 0) {
        vel.y = 0;
    }

    pos.y += vel.y;
    pos.x += vel.x;

    if (pos.x < CameraLeftLimit) pos.x = CameraLeftLimit;
}

void Player::Draw() {
    if (crouching) {
        DrawCrouch();
        DrawHitBox();
        return;
    }
    if (mode == Mode::FULL_BODY) {
        DrawFullBody();
    }
    else {
        DrawSeparated();
    }
    DrawHitBox();
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
        yOffset = 65.0f;

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
        yOffset = 75.0f;
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
        yOffset = -72.0f;
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
        yOffset = 75.0f;
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
    float baseY = pos.y + GetHeight() - (h * SCALE);
    VisualOffsets off = anim.GetOffsets();

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

    float offX = off.legsX;
    if (dir == PlayerDirection::LEFT) {
        legSrc.width = -w;
        offX = -off.legsX;
    }

    DrawTexturePro(anim.GetSheet(), legSrc,
        { pos.x + offX * SCALE, baseY + off.legsY * SCALE, w * SCALE, h * SCALE },
        { 0,0 }, 0, WHITE);

    // ========== 2. TORSO ==========
    float torsoX = pos.x;
    Rectangle torsoSrc;
    Color tint = WHITE;

    // PRIORIDAD 1: Disparo normal (horizontal)
    if (anim.IsShooting()) {
        torsoSrc = { anim.GetShootFrame() * anim.GetShootW(), anim.GetRowShoot(), anim.GetShootW(), h };
        if (dir == PlayerDirection::LEFT) {
            torsoSrc.width = -anim.GetShootW();
            torsoX = pos.x - (anim.GetShootW() - w) * SCALE;
        }
        DrawTexturePro(anim.GetSheet(), torsoSrc,
            { torsoX, baseY, anim.GetShootW() * SCALE, h * SCALE },
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

        // CALCULAR POSICIÓN X SEGÚN DIRECCIÓN
        float shootUpX = pos.x;
        if (dir == PlayerDirection::LEFT) {
            torsoSrc.width = -w;  // Voltear el sprite
            // Ajustar posición X porque el sprite se voltea
            shootUpX = pos.x - (w - w) * SCALE;  // No hay compensación porque el ancho es el mismo (34)
            // Si ves desajuste, prueba con: shootUpX = pos.x - 20.0f;
        }

        float shootUpBaseY = pos.y + GetHeight() - (anim.GetShootUpH() * SCALE);

        DrawTexturePro(anim.GetSheet(), torsoSrc,
            { shootUpX, shootUpBaseY, w * SCALE, anim.GetShootUpH() * SCALE },
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
        { pos.x, baseY, w * SCALE, h * SCALE },
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
    DrawRectangleLinesEx(GetHitBox(), 2, WHITE);
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
    float hitboxX;

    if (!grounded && !crouching) {
        float jumpOffsetX = 6.0f * SCALE;
        if (dir == PlayerDirection::LEFT) {
            hitboxX = pos.x + (34.0f * SCALE - hitboxWidth - jumpOffsetX);
        }
        else {
            hitboxX = pos.x + jumpOffsetX;
        }
    }
    else {
        if (dir == PlayerDirection::LEFT) {
            hitboxX = pos.x + (34.0f * SCALE - hitboxWidth - hitboxOffsetX);
        }
        else {
            hitboxX = pos.x + hitboxOffsetX;
        }
    }

    float hitboxY = pos.y + hitboxOffsetY;
    return Rectangle{ hitboxX, hitboxY, GetWidth(), GetHeight() };
}

Vector2 Player::GetPosition() {
    return pos;
}

// ========== MOVIMIENTO E INPUT ==========
void Player::MoveLeft() {
    if (mode != Mode::FULL_BODY && !crouching) {
        vel.x = -MOVE_SPEED;
        dir = PlayerDirection::LEFT;
    }
    else if (crouching && !anim.IsCrouchShooting()) {  // ← Solo mover si NO está disparando agachado
        vel.x = -CROUCH_SPEED;
        dir = PlayerDirection::LEFT;
    }
}

void Player::MoveRight() {
    if (mode != Mode::FULL_BODY && !crouching) {
        vel.x = MOVE_SPEED;
        dir = PlayerDirection::RIGHT;
    }
    else if (crouching && !anim.IsCrouchShooting()) {  // ← Solo mover si NO está disparando agachado
        vel.x = CROUCH_SPEED;
        dir = PlayerDirection::RIGHT;
    }
}

void Player::StopMovingHorizontal() {
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
        crouching = true;
        aimingUp = false;
        SetCrouchHitbox();
        anim.ForceCrouch();
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