#include "Player.hpp"

// ========== CONSTRUCTOR / DESTRUCTOR ==========
Player::Player() {
    anim.LoadTextures();
    SetNormalHitbox();
}

Player::~Player() {
    anim.UnloadTextures();
}

// ========== HITBOX ==========
void Player::SetNormalHitbox() {
    hitboxWidth = 20.0f * SCALE;
    hitboxHeight = 40.0f * SCALE;
    hitboxOffsetX = 0.0f;
    hitboxOffsetY = 0.0f;
}

void Player::SetCrouchHitbox() {
    hitboxWidth = 20.0f * SCALE;
    hitboxHeight = 25.0f * SCALE;
    hitboxOffsetX = 0.0f;
    hitboxOffsetY = 0.0f;
}

// ========== ACTUALIZACIÓN PRINCIPAL ==========
void Player::Update(float CameraLeftLimit) {
    if (invincibilityTimer > 0.0f) {
        invincibilityTimer -= GetFrameTime();
    }

    // Manejar muerte
    if (!isAlive) {
        deathTimer += GetFrameTime();
        if (deathTimer >= disappearDelay && !isDisappeared) {
            isDisappeared = true;
            mode = Mode::SEPARATED;
        }
        if (!isDisappeared && mode == Mode::FULL_BODY && special == SpecialAnim::DEATH) {
            specialTimer += GetFrameTime();
            if (specialTimer >= specialDuration) {
                specialTimer = specialDuration;
            }
        }
        return;
    }

    // Actualizar hitbox según estado
    if (crouching) {
        SetCrouchHitbox();
    }
    else {
        SetNormalHitbox();
    }

    anim.Update(grounded, inputVelX, crouching, aimingUp, GetFrameTime());

    // Manejar animaciones FULL_BODY
    if (mode == Mode::FULL_BODY && !crouching) {
        specialTimer += GetFrameTime();

        if (special == SpecialAnim::RESPAWN && specialTimer >= specialDuration) {
            mode = Mode::SEPARATED;
            special = SpecialAnim::NONE;
        }

        if (special == SpecialAnim::DEATH && specialTimer >= specialDuration) {
            specialTimer = specialDuration;
        }

        if (special != SpecialAnim::RESPAWN) {
            if ((vel.x < 0 && !leftCollision) || (vel.x > 0 && !rightCollision)) {
                pos.x += vel.x;
            }
        }

        if (pos.x < CameraLeftLimit) pos.x = CameraLeftLimit;
        return;
    }

    // Física y movimiento
    if (!grounded) {
        vel.y += GRAVITY;
        pos.y += vel.y;
    }
    else {
        vel.y = 0;
    }

    if ((vel.x < 0 && !leftCollision) || (vel.x > 0 && !rightCollision)) {
        pos.x += vel.x;
    }
    else if (vel.x != 0) {
        inputVelX = 0;
    }

    if (pos.x < CameraLeftLimit) pos.x = CameraLeftLimit;
}

// ========== MOVIMIENTO ==========
void Player::MoveLeft() {
    if (mode != Mode::FULL_BODY && !crouching) {
        inputVelX = -MOVE_SPEED;
        if (!leftCollision) vel.x = -MOVE_SPEED;
        else vel.x = 0;
        dir = PlayerDirection::LEFT;
    }
    else if (crouching && !anim.IsCrouchShooting()) {
        inputVelX = -CROUCH_SPEED;
        if (!leftCollision) vel.x = -CROUCH_SPEED;
        else vel.x = 0;
        dir = PlayerDirection::LEFT;
    }
}

void Player::MoveRight() {
    if (mode != Mode::FULL_BODY && !crouching) {
        inputVelX = MOVE_SPEED;
        if (!rightCollision) vel.x = MOVE_SPEED;
        else vel.x = 0;
        dir = PlayerDirection::RIGHT;
    }
    else if (crouching && !anim.IsCrouchShooting()) {
        inputVelX = CROUCH_SPEED;
        if (!rightCollision) vel.x = CROUCH_SPEED;
        else vel.x = 0;
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
        float feetY = pos.y + GetHeight();
        crouching = true;
        aimingUp = false;
        SetCrouchHitbox();
        anim.ForceCrouch();
        pos.y = feetY - GetHeight();
    }
}

void Player::StopCrouching() {
    if (crouching) {
        float currentY = pos.y;
        crouching = false;
        SetNormalHitbox();
        pos.y = currentY - 60.0f;
    }
}

void Player::Shoot() {
    if (mode != Mode::FULL_BODY) {
        if (crouching) {
            vel.x = 0;
            anim.StartCrouchShoot();
        }
        else if (aimingUp) {
            if (anim.IsShooting()) anim.ForceStopShoot();
            anim.StartShootUp();
        }
        else {
            anim.StartShoot();
        }
    }
}

// ========== GETTERS ==========
PlayerDirection Player::GetAimDirection() const {
    if (aimingUp) return PlayerDirection::UP;
    return dir;
}

// ========== GRANADA ==========
GrenadeThrowData Player::ThrowGrenade() {
    anim.StartThrow();

    GrenadeThrowData data;
    data.startPos = pos;
    data.power = 1500.0f;
    data.valid = true;

    switch (dir) {
    case PlayerDirection::LEFT:
        data.targetPos = { pos.x - 300, 0 };
        break;
    case PlayerDirection::RIGHT:
        data.targetPos = { pos.x + 300, 0 };
        break;
    case PlayerDirection::UP:
        data.targetPos = { pos.x, pos.y - 150 };
        break;
    default:
        data.targetPos = { pos.x + 250, 0 };
        break;
    }
    return data;
}

// ========== ARMAS ==========
void Player::EquipMachinegun() {
    currentWeapon = WeaponType::MACHINEGUN;
    machinegunAmmo = MACHINEGUN_MAX_AMMO;
}

void Player::UseAmmo() {
    if (currentWeapon == WeaponType::MACHINEGUN) {
        machinegunAmmo--;
        if (machinegunAmmo <= 0) {
            currentWeapon = WeaponType::PISTOL;
            machinegunAmmo = 0;
        }
    }
}

// ========== MUERTE Y RESPAWN ==========
void Player::TakeDamage() {
    if (!isAlive) return;
    if (IsInvincible()) return;

    isAlive = false;
    deathPosition = pos;
    deathTimer = 0.0f;
    isDisappeared = false;

    mode = Mode::FULL_BODY;
    special = SpecialAnim::DEATH;
    specialTimer = 0.0f;
    specialDuration = 1.0f;

    vel = { 0.0f, 0.0f };
    inputVelX = 0;
}

void Player::Respawn() {
    isAlive = true;
    isDisappeared = false;
    deathTimer = 0.0f;

    mode = Mode::FULL_BODY;
    special = SpecialAnim::RESPAWN;
    specialTimer = 0.0f;
    specialDuration = 0.6f;

    pos = deathPosition;
    vel = { 0.0f, 0.0f };
    invincibilityTimer = invincibilityDuration;
}

// ========== HITBOXES ==========
Rectangle Player::GetHitBox() {
    float spriteTotalWidth = 34.0f * SCALE;
    float hitboxX = pos.x + (spriteTotalWidth - hitboxWidth) / 2.0f;
    float hitboxY = pos.y + hitboxOffsetY;
    return Rectangle{ hitboxX, hitboxY, hitboxWidth, hitboxHeight };
}

Rectangle Player::GetLeftHitBox() {
    Rectangle mainHitBox = GetHitBox();
    float reducedHeight = mainHitBox.height * 0.6f;
    float offsetY = (mainHitBox.height - reducedHeight) / 2.0f;
    float hitboxX = mainHitBox.x - (mainHitBox.width * 0.3f);
    return Rectangle{ hitboxX, mainHitBox.y + offsetY, mainHitBox.width * 0.4f, reducedHeight };
}

Rectangle Player::GetRightHitBox() {
    Rectangle mainHitBox = GetHitBox();
    float reducedHeight = mainHitBox.height * 0.6f;
    float offsetY = (mainHitBox.height - reducedHeight) / 2.0f;
    float hitboxX = mainHitBox.x + mainHitBox.width - (mainHitBox.width * 0.1f);
    return Rectangle{ hitboxX, mainHitBox.y + offsetY, mainHitBox.width * 0.4f, reducedHeight };
}

Vector2 Player::GetPosition() {
    return pos;
}

float Player::GetHeight() const {
    if (mode == Mode::FULL_BODY) return GetFullBodyH() * SCALE;
    return hitboxHeight;
}

// ========== MÉTODOS DE ANIMACIÓN FULL_BODY ==========
float Player::GetFullBodyH() const {
    switch (special) {
    case SpecialAnim::CROUCH:
    case SpecialAnim::CROUCH_SHOOT:
        return 20.0f;
    case SpecialAnim::FALLING_START:
        return 48.0f;
    case SpecialAnim::DEATH:
        return 64.0f;
    case SpecialAnim::RESPAWN:
        return 238.0f;
    default:
        return 34.0f;
    }
}

Rectangle Player::GetFullBodyRect() {
    int row = 0;
    int h = 34;
    switch (special) {
    case SpecialAnim::CROUCH: row = 0; h = 20; break;
    case SpecialAnim::CROUCH_SHOOT: row = 1; h = 20; break;
    case SpecialAnim::FALLING_START: row = 2; h = 48; break;
    case SpecialAnim::DEATH: row = 32; h = 64; break;
    case SpecialAnim::RESPAWN: row = 6; h = 238; break;
    default: break;
    }
    return { 0, row * 34.0f, 34.0f, (float)h };
}

// ========== DIBUJO PRINCIPAL ==========
void Player::Draw() {
    if (!isAlive && isDisappeared) return;

    bool shouldDraw = true;

    if (!isAlive && !isDisappeared) {
        float timeUntilDisappear = disappearDelay - deathTimer;
        if (timeUntilDisappear < 0.5f && timeUntilDisappear > 0.0f) {
            int blinkPhase = (int)(deathTimer * 20);
            shouldDraw = (blinkPhase % 2 == 0);
        }
    }

    if (isAlive && IsInvincible()) {
        int blinkPhase = (int)(GetTime() * 15);
        shouldDraw = (blinkPhase % 2 == 0);
    }

    if (!shouldDraw) return;

    if (mode == Mode::FULL_BODY) {
        DrawFullBody();
        DrawHitBox();
        return;
    }
    if (crouching) {
        DrawCrouch();
        DrawHitBox();
        return;
    }
    DrawSeparated();
    DrawHitBox();
}

void Player::DrawHitBox() {
    DrawRectangleLinesEx(GetHitBox(), 2, WHITE);
    DrawRectangleLinesEx(GetLeftHitBox(), 2, RED);
    DrawRectangleLinesEx(GetRightHitBox(), 2, BLUE);
}

// ========== DIBUJO AGACHADO ==========
void Player::DrawCrouch() {
    Rectangle sourceRect;
    float currentHeight;
    float rowY;
    float yOffset;
    float drawX = pos.x;

    if (anim.IsCrouchShooting()) {
        rowY = anim.GetCrouchShootRowY();
        currentHeight = 34.0f;
        yOffset = -30.0f;
        sourceRect = {
            (float)(anim.GetCrouchShootFrame() * 68.0f),
            rowY,
            anim.GetCrouchShootW(),
            currentHeight
        };
        if (dir == PlayerDirection::LEFT) {
            sourceRect.width = -anim.GetCrouchShootW();
            drawX = pos.x - (anim.GetCrouchShootW() - 34.0f) * SCALE;
        }
    }
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
        if (dir == PlayerDirection::LEFT) sourceRect.width = -34.0f;
    }
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
        if (dir == PlayerDirection::LEFT) sourceRect.width = -34.0f;
    }
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
        if (dir == PlayerDirection::LEFT) sourceRect.width = -34.0f;
    }

    Rectangle destRect = {
        drawX,
        pos.y + yOffset,
        (anim.IsCrouchShooting() ? anim.GetCrouchShootW() : 34.0f) * SCALE,
        currentHeight * SCALE
    };
    DrawTexturePro(anim.GetSheet(), sourceRect, destRect, { 0,0 }, 0, WHITE);
}

// ========== DIBUJO SEPARADO (PIERNAS + TORSO) ==========
void Player::DrawSeparated() {
    float w = anim.GetW(), h = anim.GetH();
    float baseY = pos.y + GetHeight() - (h * SCALE) - 35.0f;
    VisualOffsets off = anim.GetOffsets();

    float legsOffsetX = off.legsX;
    float torsoOffsetX = off.torsoX;

    if (dir == PlayerDirection::LEFT) {
        legsOffsetX = -off.legsX;
        torsoOffsetX = -off.torsoX;
    }

    // Piernas
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
    if (dir == PlayerDirection::LEFT) legSrc.width = -w;
    DrawTexturePro(anim.GetSheet(), legSrc,
        { pos.x + legsOffsetX * SCALE, baseY + off.legsY * SCALE, w * SCALE, h * SCALE },
        { 0,0 }, 0, WHITE);

    // Torso
    float torsoDrawX = pos.x + torsoOffsetX * SCALE;
    float torsoDrawY = baseY + off.torsoY * SCALE;
    Rectangle torsoSrc;
    Color tint = WHITE;

    // Prioridad: Lanzar granada
    if (anim.IsThrowing()) {
        torsoSrc = { anim.GetThrowFrame() * w, anim.GetThrowRowY(), w, h };
        if (dir == PlayerDirection::LEFT) torsoSrc.width = -w;
        DrawTexturePro(anim.GetSheet(), torsoSrc,
            { torsoDrawX, torsoDrawY, w * SCALE, h * SCALE },
            { 0,0 }, 0, tint);
        return;
    }

    // Prioridad: Disparo normal
    if (anim.IsShooting()) {
        torsoSrc = { anim.GetShootFrame() * anim.GetShootW(), anim.GetRowShoot(), anim.GetShootW(), h };
        float shootDrawX = torsoDrawX;
        float shootDrawY = torsoDrawY;
        if (dir == PlayerDirection::LEFT) {
            torsoSrc.width = -anim.GetShootW();
            shootDrawX = torsoDrawX - 140.0f;
        }
        shootDrawY = torsoDrawY - 5.0f;
        DrawTexturePro(anim.GetSheet(), torsoSrc,
            { shootDrawX, shootDrawY, anim.GetShootW() * SCALE, h * SCALE },
            { 0,0 }, 0, tint);
        return;
    }

    // Prioridad: Disparo hacia arriba
    if (aimingUp && anim.IsShootingUp()) {
        torsoSrc = { (float)(anim.GetShootUpFrame() * w), anim.GetShootUpRowY(), w, anim.GetShootUpH() };
        if (dir == PlayerDirection::LEFT) torsoSrc.width = -w;
        float shootUpBaseY = baseY - 140 + (off.torsoY * SCALE);
        DrawTexturePro(anim.GetSheet(), torsoSrc,
            { torsoDrawX, shootUpBaseY, w * SCALE, anim.GetShootUpH() * SCALE },
            { 0,0 }, 0, tint);
        return;
    }

    // Aiming up
    if (aimingUp) {
        if (anim.IsAimingTransition()) {
            torsoSrc = { (float)(anim.GetAimingFrame() * w), anim.GetAimingTransitionRowY(), w, h };
        }
        else {
            torsoSrc = { (float)(anim.GetAimingFrame() * w), anim.GetAimingIdleRowY(), w, h };
        }
    }
    // Torso normal
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
    if (dir == PlayerDirection::LEFT) torsoSrc.width = -w;
    DrawTexturePro(anim.GetSheet(), torsoSrc,
        { torsoDrawX, torsoDrawY, w * SCALE, h * SCALE },
        { 0,0 }, 0, tint);
}

// ========== DIBUJO CUERPO COMPLETO ==========
void Player::DrawFullBody() {
    Rectangle sourceRect = GetFullBodyRect();

    // Animación de reaparición
    if (special == SpecialAnim::RESPAWN) {
        const float RESPAWN_FRAME_WIDTH = 34.0f;
        const float RESPAWN_FRAME_HEIGHT = 238.0f;
        const int RESPAWN_TOTAL_FRAMES = 7;
        const float RESPAWN_ANIM_SPEED = 20.0f;

        int currentFrame = (int)(specialTimer * RESPAWN_ANIM_SPEED);
        if (currentFrame >= RESPAWN_TOTAL_FRAMES) {
            currentFrame = RESPAWN_TOTAL_FRAMES - 1;
            if (specialTimer >= specialDuration) {
                mode = Mode::SEPARATED;
                special = SpecialAnim::NONE;
            }
        }

        float startRowY = 0.0f;
        sourceRect = { currentFrame * RESPAWN_FRAME_WIDTH, startRowY, RESPAWN_FRAME_WIDTH, RESPAWN_FRAME_HEIGHT };
        if (dir == PlayerDirection::LEFT) sourceRect.width = -RESPAWN_FRAME_WIDTH;

        float offsetY = -RESPAWN_FRAME_HEIGHT * SCALE + 165.0f;
        Rectangle destRect = { pos.x, pos.y + offsetY, RESPAWN_FRAME_WIDTH * SCALE, RESPAWN_FRAME_HEIGHT * SCALE };
        DrawTexturePro(anim.GetSheet(), sourceRect, destRect, { 0, 0 }, 0, WHITE);
        return;
    }

    // Animación de muerte
    if (special == SpecialAnim::DEATH) {
        const float DEATH_FRAME_WIDTH = 68.0f;
        const float DEATH_FRAME_HEIGHT = 68.0f;
        const int DEATH_TOTAL_FRAMES = 19;
        const float DEATH_ANIM_SPEED = 12.0f;

        int currentFrame = (int)(specialTimer * DEATH_ANIM_SPEED);
        if (currentFrame >= DEATH_TOTAL_FRAMES) currentFrame = DEATH_TOTAL_FRAMES - 1;

        float startRowY = 31.0f * 34.0f;
        sourceRect = { currentFrame * DEATH_FRAME_WIDTH, startRowY, DEATH_FRAME_WIDTH, DEATH_FRAME_HEIGHT };

        float offsetX, offsetY = -110.0f;
        if (dir == PlayerDirection::RIGHT) {
            sourceRect.width = DEATH_FRAME_WIDTH;
            offsetX = 10.0f;
        }
        else {
            sourceRect.width = -DEATH_FRAME_WIDTH;
            offsetX = -150.0f;
        }
        Rectangle destRect = { pos.x + offsetX, pos.y + offsetY, DEATH_FRAME_WIDTH * SCALE, DEATH_FRAME_HEIGHT * SCALE };
        DrawTexturePro(anim.GetSheet(), sourceRect, destRect, { 0, 0 }, 0, WHITE);
        return;
    }

    // Animación normal FULL_BODY
    if (dir == PlayerDirection::LEFT) sourceRect.width = -sourceRect.width;
    float currentHeight = GetFullBodyH();
    Rectangle destRect = { pos.x, pos.y + (34.0f - currentHeight) * SCALE, GetWidth(), currentHeight * SCALE };
    DrawTexturePro(anim.GetSheet(), sourceRect, destRect, { 0,0 }, 0, aimingUp ? YELLOW : WHITE);
}