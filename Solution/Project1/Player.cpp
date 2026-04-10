#include "Player.hpp"

Player::Player() {
    anim.LoadTextures();
}

Player::~Player() {
    anim.UnloadTextures();
}

void Player::Update(float CameraLeftLimit) {
    // ========== ACTUALIZAR ANIMACIONES ==========
    anim.Update(grounded, vel.x, crouching, aimingUp, GetFrameTime());
    
    // ========== MODO CUERPO COMPLETO ==========
    if (mode == Mode::FULL_BODY) {
        specialTimer += GetFrameTime();
        if (specialTimer >= specialDuration) {
            mode = Mode::SEPARATED;
            special = SpecialAnim::NONE;
            crouching = false;
        }
        pos.x += vel.x;
        
        // Límite izquierdo con cámara
        float currentWidth = GetWidth();
        if (pos.x < CameraLeftLimit) pos.x = CameraLeftLimit;
        return;
    }
    
    // ========== FÍSICA ==========
    if (!grounded) {
        vel.y += GRAVITY;
    } else if (vel.y > 0) {
        vel.y = 0;
    }
    
    pos.y += vel.y;
    pos.x += vel.x;
    
    // ========== LÍMITE IZQUIERDO CON CÁMARA ==========
    float currentWidth = GetWidth();
    if (pos.x < CameraLeftLimit) pos.x = CameraLeftLimit;
}

void Player::Draw() {
    if (mode == Mode::FULL_BODY) DrawFullBody();
    else DrawSeparated();
    DrawHitBox();
}

void Player::DrawSeparated() {
    float w = anim.GetW(), h = anim.GetH();
    float baseY = pos.y + GetHeight() - (h * SCALE);
    VisualOffsets off = anim.GetOffsets();
    
    // ========== 1. DIBUJAR PIERNAS ==========
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
    if (dir == PlayerDirection::LEFT) { legSrc.width = -w; offX = -off.legsX; }
    
    DrawTexturePro(anim.GetSheet(), legSrc,
        { pos.x + offX * SCALE, baseY + off.legsY * SCALE, w * SCALE, h * SCALE },
        {0,0}, 0, WHITE);
    
    // ========== 2. DIBUJAR TORSO ==========
    float torsoX = pos.x;
    Rectangle torsoSrc;
    
    // Color tint según estado
    Color tint = WHITE;
    if (crouching) tint = SKYBLUE;
    if (aimingUp) tint = YELLOW;
    
    if (anim.IsShooting()) {
        torsoSrc = { anim.GetShootFrame() * anim.GetShootW(), anim.GetRowShoot(), anim.GetShootW(), h };
        if (dir == PlayerDirection::LEFT) {
            torsoSrc.width = -anim.GetShootW();
            torsoX = pos.x - (anim.GetShootW() - w) * SCALE;
        }
        DrawTexturePro(anim.GetSheet(), torsoSrc,
            { torsoX, baseY, anim.GetShootW() * SCALE, h * SCALE },
            {0,0}, 0, tint);
        return;
    }
    
    // Torso normal
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
    if (dir == PlayerDirection::LEFT) torsoSrc.width = -w;
    
    DrawTexturePro(anim.GetSheet(), torsoSrc,
        { pos.x, baseY, w * SCALE, h * SCALE },
        {0,0}, 0, tint);
}

void Player::DrawFullBody() {
    Rectangle src = GetFullBodyRect();
    if (dir == PlayerDirection::LEFT) src.width = -src.width;
    float curH = GetFullBodyH();
    
    Color tint = WHITE;
    if (crouching) tint = SKYBLUE;
    
    DrawTexturePro(anim.GetSheet(), src,
        { pos.x, pos.y + (NORMAL_H - curH) * SCALE, GetWidth(), curH * SCALE },
        {0,0}, 0, tint);
}

void Player::DrawHitBox() {
    DrawRectangleLinesEx(GetHitBox(), 2, WHITE);
}

Rectangle Player::GetFullBodyRect() {
    int row = 0;
    int h = (int)NORMAL_H;
    
    switch (special) {
        case SpecialAnim::CROUCH:
            row = 0;
            h = (int)CROUCH_H;
            break;
        case SpecialAnim::CROUCH_SHOOT:
            row = 1;
            h = (int)CROUCH_H;
            break;
        case SpecialAnim::FALLING_START:
            row = 2;
            h = 48;
            break;
        default:
            break;
    }
    return { 0, row * NORMAL_H, NORMAL_H, (float)h };
}

float Player::GetFullBodyH() const {
    switch (special) {
        case SpecialAnim::CROUCH:
        case SpecialAnim::CROUCH_SHOOT:
            return CROUCH_H;
        case SpecialAnim::FALLING_START:
            return 48.0f;
        default:
            return NORMAL_H;
    }
}

float Player::GetHeight() const {
    if (mode == Mode::FULL_BODY) return GetFullBodyH() * SCALE;
    if (crouching) return CROUCH_H * SCALE;
    return NORMAL_H * SCALE;
}

Rectangle Player::GetHitBox() {
    return Rectangle{ pos.x, pos.y, GetWidth(), GetHeight() };
}

Vector2 Player::GetPosition() {
    return pos;
}

// ========== MOVIMIENTO E INPUT ==========
void Player::MoveLeft() {
    if (mode != Mode::FULL_BODY) {
        vel.x = crouching ? -CROUCH_SPEED : -MOVE_SPEED;
        if (!aimingUp) dir = PlayerDirection::LEFT;
    }
}

void Player::MoveRight() {
    if (mode != Mode::FULL_BODY) {
        vel.x = crouching ? CROUCH_SPEED : MOVE_SPEED;
        if (!aimingUp) dir = PlayerDirection::RIGHT;
    }
}

void Player::StopMovingHorizontal() {
    if (mode != Mode::FULL_BODY) vel.x = 0;
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
        special = SpecialAnim::CROUCH;
        mode = Mode::FULL_BODY;
        specialDuration = 0.1f;
        specialTimer = 0;
    }
}

void Player::StopCrouching() {
    if (crouching) {
        crouching = false;
    }
}

void Player::Shoot() {
    if (mode != Mode::FULL_BODY) anim.StartShoot();
}

PlayerDirection Player::GetAimDirection() const {
    if (aimingUp) return PlayerDirection::UP;
    if (crouching) return PlayerDirection::DOWN;
    return dir;
}