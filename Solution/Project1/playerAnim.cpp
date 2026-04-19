#include "playerAnim.hpp"

PlayerAnim::PlayerAnim() : spriteSheet{ 0 } {}

PlayerAnim::~PlayerAnim() {}

void PlayerAnim::LoadTextures() {
    Image img = LoadImage("Graphics/marco_sprites.png");
    spriteSheet = LoadTextureFromImage(img);
    UnloadImage(img);
    SetTextureFilter(spriteSheet, TEXTURE_FILTER_POINT);
}

void PlayerAnim::UnloadTextures() {
    UnloadTexture(spriteSheet);
}

void PlayerAnim::Update(bool grounded, float velX, bool crouchingInput, bool aimingUpInput, float dt) {
    // ========== DISPARO HACIA ARRIBA (shooting up) ==========
    if (shootingUp) {
        shootUpTimer += dt;
        if (shootUpTimer >= shootUpDelay) {
            shootUpTimer = 0.0f;
            shootUpFrame++;
            if (shootUpFrame >= shootUpFrameCount) {
                shootUpFrame = 0;
                shootingUp = false;
            }
        }
    }

    // ========== AIMING UP (apuntar, sin disparar) ==========
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

    if (aimingUp && !shootingUp) {
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
    }

    // ========== DISPARO NORMAL (horizontal) ==========
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

    // ========== LANZAR GRANADA ==========
    if (isThrowing) {
        throwTimer += dt;
        if (throwTimer >= throwDelay) {
            throwTimer = 0.0f;
            throwFrame++;
            if (throwFrame >= throwFrameCount) {
                throwFrame = 0;
                isThrowing = false;
            }
        }
    }

    // ========== AGACHADO ==========
    if (crouchingInput && !crouching) {
        crouching = true;
        crouchTransition = true;
        crouchWalking = false;
        crouchShooting = false;
        crouchFrame = 0;
        crouchTimer = 0.0f;
    }
    else if (!crouchingInput && crouching) {
        crouching = false;
        crouchTransition = false;
        crouchWalking = false;
        crouchShooting = false;
        crouchFrame = 0;
        crouchTimer = 0.0f;
        crouchWalkFrame = 0;
        crouchShootFrame = 0;
    }

    if (crouching) {
        // ========== DISPARO AGACHADO ==========
        if (crouchShooting) {
            crouchShootTimer += dt;
            if (crouchShootTimer >= crouchShootDelay) {
                crouchShootTimer = 0.0f;
                crouchShootFrame++;
                if (crouchShootFrame >= crouchShootFrameCount) {
                    crouchShootFrame = 0;
                    crouchShooting = false;
                }
            }
        }

        // ========== CAMINAR AGACHADO ==========
        if (velX != 0 && !crouchTransition && !crouchShooting) {
            crouchWalking = true;
            crouchWalkTimer += dt;
            if (crouchWalkTimer >= crouchWalkDelay) {
                crouchWalkTimer = 0.0f;
                crouchWalkFrame++;
                if (crouchWalkFrame >= crouchWalkFrameCount) {
                    crouchWalkFrame = 0;
                }
            }
        }
        else if (!crouchShooting) {
            crouchWalking = false;
            crouchWalkFrame = 0;
            crouchWalkTimer = 0.0f;
        }

        // ========== TRANSICIÓN E IDLE AGACHADO ==========
        if (!crouchShooting) {
            crouchTimer += dt;
            float currentDelay = crouchTransition ? crouchTransitionDelay : crouchIdleDelay;
            if (crouchTimer >= currentDelay) {
                crouchTimer = 0.0f;
                crouchFrame++;

                if (crouchTransition) {
                    if (crouchFrame >= 3) {
                        crouchTransition = false;
                        crouchFrame = 0;
                    }
                }
                else {
                    if (!crouchWalking && crouchFrame >= 4) {
                        crouchFrame = 0;
                    }
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
        if (walkLegsTimer >= walkLegsDelay) {
            walkLegsTimer = 0.0f;
            walkLegsFrame = (walkLegsFrame + 1) % 12;
        }
        walkTorsoTimer += dt;
        if (walkTorsoTimer >= walkTorsoDelay) {
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

void PlayerAnim::StartShootUp() {
    shootingUp = true;
    shootUpFrame = 0;
    shootUpTimer = 0.0f;
}

void PlayerAnim::StartCrouchShoot() {
    crouchShooting = true;
    crouchShootFrame = 0;
    crouchShootTimer = 0.0f;
    crouchWalking = false;
    crouchWalkFrame = 0;
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

void PlayerAnim::ForceStopShoot() {
    shooting = false;
    shootFrame = 0;
    shootTimer = 0.0f;
}

void PlayerAnim::StartThrow() {
    if (isThrowing) return;
    isThrowing = true;
    throwFrame = 0;
    throwTimer = 0.0f;
}