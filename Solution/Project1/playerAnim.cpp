#include "playerAnim.hpp"

PlayerAnim::PlayerAnim() {}

PlayerAnim::~PlayerAnim() {}

void PlayerAnim::LoadTextures() {
    Image img = LoadImage("Graphics/MarcoRossi.png");
    spriteSheet = LoadTextureFromImage(img);
    UnloadImage(img);
    SetTextureFilter(spriteSheet, TEXTURE_FILTER_POINT);
}

void PlayerAnim::UnloadTextures() {
    UnloadTexture(spriteSheet);
}

void PlayerAnim::Update(bool grounded, float velX, bool crouching, bool aimingUp, float dt) {
    // ========== 1. ACTUALIZAR ESTADOS SEGÚN MOVIMIENTO ==========
    if (!grounded) {
        legsAnim = LegsAnim::JUMPING;
        torsoAnim = TorsoAnim::JUMPING;
    } else if (velX != 0 && !crouching) {
        legsAnim = LegsAnim::WALKING;
        torsoAnim = TorsoAnim::WALKING;
    } else {
        legsAnim = LegsAnim::IDLE;
        torsoAnim = TorsoAnim::IDLE;
    }
    if (aimingUp) torsoAnim = TorsoAnim::SHOOTING;
    
    // ========== 2. ANIMACIÓN IDLE (loop de 4 frames) ==========
    if (torsoAnim == TorsoAnim::IDLE) {
        idleTimer += dt;
        if (idleTimer >= 0.15f) {
            idleTimer = 0;
            idleFrame = (idleFrame + 1) % 4;
        }
    }
    
    // ========== 3. ANIMACIÓN WALKING (loop de 12 frames) ==========
    if (legsAnim == LegsAnim::WALKING) {
        walkLegsTimer += dt;
        if (walkLegsTimer >= 0.02f) {
            walkLegsTimer = 0;
            walkLegsFrame = (walkLegsFrame + 1) % 12;
        }
        walkTorsoTimer += dt;
        if (walkTorsoTimer >= 0.02f) {
            walkTorsoTimer = 0;
            walkTorsoFrame = (walkTorsoFrame + 1) % 12;
        }
    }
    
    // ========== 4. ANIMACIÓN JUMPING (SIN LOOP) ==========
    if (legsAnim == LegsAnim::JUMPING && !jumpComplete) {
        jumpLegsTimer += dt;
        if (jumpLegsTimer >= 0.08f) {
            jumpLegsTimer = 0;
            if (++jumpLegsFrame >= 6) { jumpLegsFrame = 5; jumpComplete = true; }
        }
        jumpTorsoTimer += dt;
        if (jumpTorsoTimer >= 0.08f) {
            jumpTorsoTimer = 0;
            if (++jumpTorsoFrame >= 6) { jumpTorsoFrame = 5; }
        }
    }
    
    // ========== 5. ANIMACIÓN DISPARO (10 frames) ==========
    if (shooting) {
        shootTimer += dt;
        if (shootTimer >= 0.05f) {
            shootTimer = 0;
            if (++shootFrame >= 10) { shootFrame = 0; shooting = false; }
        }
    }
}

VisualOffsets PlayerAnim::GetOffsets() const {
    if (shooting && legsAnim == LegsAnim::JUMPING) return jumpShootOffset;
    switch (legsAnim) {
        case LegsAnim::WALKING: return walkOffset;
        case LegsAnim::JUMPING: return jumpOffset;
        default: return idleOffset;
    }
}

void PlayerAnim::StartShoot() { 
    shooting = true; 
    shootFrame = 0; 
    shootTimer = 0; 
}

void PlayerAnim::ResetJump() { 
    jumpComplete = false; 
    jumpLegsFrame = 0; 
    jumpTorsoFrame = 0; 
}