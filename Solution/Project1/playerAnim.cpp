#include "playerAnim.hpp"

PlayerAnim::PlayerAnim() : spriteSheet{ 0 } {}

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

void PlayerAnim::Update(bool grounded, float velX, bool crouchingInput, bool aimingUpInput, float dt) {
    // ========== AIMING UP (prioridad alta) ==========
    if (aimingUpInput && !aimingUp) {
        aimingUp = true;
        aimingTransition = true;
        aimingFrame = 0;
        aimingTimer = 0.0f;
    }
    else if (!aimingUpInput && aimingUp) {
        aimingUp = false;
        aimingTransition = true;
        aimingFrame = 0;
        aimingTimer = 0.0f;
    }

    if (aimingUp) {
        aimingTimer += dt;
        if (aimingTimer >= aimingDelay) {
            aimingTimer = 0.0f;
            aimingFrame++;

            if (aimingTransition) {
                if (aimingFrame >= 2) {
                    aimingTransition = false;
                    aimingFrame = 0;
                }
            }
            else {
                if (aimingFrame >= 4) {
                    aimingFrame = 0;
                }
            }
        }
        // No return para que las piernas sigan actualizándose
    }

    // ========== AGACHADO (prioridad máxima) ==========
    if (crouchingInput && !crouching) {
        crouching = true;
        crouchTransition = true;
        crouchFrame = 0;
        crouchTimer = 0.0f;
    }
    else if (!crouchingInput && crouching) {
        crouching = false;
        crouchTransition = false;
        crouchFrame = 0;
        crouchTimer = 0.0f;
    }

    if (crouching) {
        crouchTimer += dt;
        if (crouchTimer >= crouchDelay) {
            crouchTimer = 0.0f;
            crouchFrame++;

            if (crouchTransition) {
                if (crouchFrame >= 3) {
                    crouchTransition = false;
                    crouchFrame = 0;
                }
            }
            else {
                if (crouchFrame >= 4) {
                    crouchFrame = 0;
                }
            }
        }
        return;
    }

    // ========== ANIMACIONES NORMALES ==========
    if (!grounded) {
        legsAnim = LegsAnim::JUMPING;
        torsoAnim = TorsoAnim::JUMPING;
    }
    else if (velX != 0 && !crouchingInput) {
        legsAnim = LegsAnim::WALKING;
        torsoAnim = TorsoAnim::WALKING;
    }
    else {
        legsAnim = LegsAnim::IDLE;
        torsoAnim = TorsoAnim::IDLE;
    }
    if (aimingUpInput) torsoAnim = TorsoAnim::SHOOTING;

    // IDLE
    if (torsoAnim == TorsoAnim::IDLE) {
        idleTimer += dt;
        if (idleTimer >= 0.15f) {
            idleTimer = 0.0f;
            idleFrame = (idleFrame + 1) % 4;
        }
    }

    // WALKING
    if (legsAnim == LegsAnim::WALKING) {
        walkLegsTimer += dt;
        if (walkLegsTimer >= 0.02f) {
            walkLegsTimer = 0.0f;
            walkLegsFrame = (walkLegsFrame + 1) % 12;
        }
        walkTorsoTimer += dt;
        if (walkTorsoTimer >= 0.02f) {
            walkTorsoTimer = 0.0f;
            walkTorsoFrame = (walkTorsoFrame + 1) % 12;
        }
    }

    // JUMPING
    if (legsAnim == LegsAnim::JUMPING && !jumpComplete) {
        jumpLegsTimer += dt;
        if (jumpLegsTimer >= 0.08f) {
            jumpLegsTimer = 0.0f;
            if (++jumpLegsFrame >= 6) {
                jumpLegsFrame = 5;
                jumpComplete = true;
            }
        }
        jumpTorsoTimer += dt;
        if (jumpTorsoTimer >= 0.08f) {
            jumpTorsoTimer = 0.0f;
            if (++jumpTorsoFrame >= 6) {
                jumpTorsoFrame = 5;
            }
        }
    }

    // SHOOTING
    if (shooting) {
        shootTimer += dt;
        if (shootTimer >= 0.05f) {
            shootTimer = 0.0f;
            if (++shootFrame >= 10) {
                shootFrame = 0;
                shooting = false;
            }
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
    shootTimer = 0.0f;
}

void PlayerAnim::ResetJump() {
    jumpComplete = false;
    jumpLegsFrame = 0;
    jumpTorsoFrame = 0;
}

void PlayerAnim::ForceCrouch() {
    crouching = true;
    crouchTransition = true;
    crouchFrame = 0;
    crouchTimer = 0.0f;
}