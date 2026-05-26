#include "playerAnim.hpp"

PlayerAnim::PlayerAnim() : spriteSheet{ 0 }, hasLandingPosition(false) {}

PlayerAnim::~PlayerAnim() {}

void PlayerAnim::LoadTextures() {
    Image img = LoadImage("Graphics/marco_sprites.png");
    spriteSheet = LoadTextureFromImage(img);
    UnloadImage(img);
    SetTextureFilter(spriteSheet, TEXTURE_FILTER_POINT);
    LoadParachute();
    LoadParachute2();

    Image imgP1 = LoadImage("Graphics/new fonts and HUDs/p1anim31x30.png");
    texP1Anim = LoadTextureFromImage(imgP1);
    UnloadImage(imgP1);
    SetTextureFilter(texP1Anim, TEXTURE_FILTER_POINT);
}

void PlayerAnim::UnloadTextures() {
    UnloadTexture(spriteSheet);
    UnloadParachute();
    UnloadParachute2();
    UnloadTexture(texP1Anim);
}

void PlayerAnim::Update(bool grounded, float velX, bool crouchingInput, bool aimingUpInput, bool hasMachinegun, float dt) {
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

        // ========== MACHINEGUN IDLE ==========
        if (machinegunIdle) {
            machinegunIdleTimer += dt;
            if (machinegunIdleTimer >= machinegunIdleDelay) {
                machinegunIdleTimer = 0.0f;
                machinegunIdleFrame++;
                if (machinegunIdleFrame >= machinegunIdleFrameCount) {
                    machinegunIdleFrame = 0;
                }
            }
            if (!machinegunAimingUp) torsoAnim = TorsoAnim::MACHINEGUN_IDLE;
        }

        // ========== MACHINEGUN SHOOTING ==========
        if (machinegunShooting) {
            machinegunShootTimer += dt;
            if (machinegunShootTimer >= machinegunShootDelay) {
                machinegunShootTimer = 0.0f;
                machinegunShootFrame++;
                if (machinegunShootFrame >= machinegunShootFrameCount) {
                    machinegunShootFrame = machinegunShootFrameCount - 1;
                }
            }
            torsoAnim = TorsoAnim::MACHINEGUN_SHOOTING;

            machinegunShootCooldown += dt;
            if (machinegunShootCooldown >= machinegunShootCooldownMax) {
                machinegunShooting = false;
                machinegunIdle = true;
                machinegunShootCooldown = 0.0f;
                machinegunShootFrame = 0;
                torsoAnim = TorsoAnim::MACHINEGUN_IDLE;
            }
        }

        // ========== MACHINEGUN THROWING ==========
        if (machinegunThrowing) {
            machinegunThrowTimer += dt;
            if (machinegunThrowTimer >= machinegunThrowDelay) {
                machinegunThrowTimer = 0.0f;
                machinegunThrowFrame++;
                if (machinegunThrowFrame >= machinegunThrowFrameCount) {
                    machinegunThrowFrame = machinegunThrowFrameCount - 1;
                }
            }
            torsoAnim = TorsoAnim::MACHINEGUN_THROWING;

            machinegunThrowCooldown += dt;
            if (machinegunThrowCooldown >= machinegunThrowCooldownMax) {
                machinegunThrowing = false;
                machinegunIdle = true;
                machinegunThrowCooldown = 0.0f;
                machinegunThrowFrame = 0;
                torsoAnim = TorsoAnim::MACHINEGUN_IDLE;
            }
        }

        // ========== MACHINEGUN AIMING UP ==========
        if (hasMachinegun) {
            if (aimingUpInput && !machinegunAimingUp) {
                machinegunAimingUp = true;
                machinegunAimingTransition = true;
                machinegunAimingFrame = 0;
                machinegunAimingTimer = 0.0f;
            }
            else if (!aimingUpInput && machinegunAimingUp) {
                machinegunAimingUp = false;
                machinegunAimingTransition = true;
                machinegunAimingFrame = 0;
                machinegunAimingTimer = 0.0f;
            }

            if (machinegunAimingUp && !machinegunShooting) {
                machinegunAimingTimer += dt;
                if (machinegunAimingTimer >= machinegunAimingDelay) {
                    machinegunAimingTimer = 0.0f;
                    machinegunAimingFrame++;

                    if (machinegunAimingTransition) {
                        if (machinegunAimingFrame >= 2) {
                            machinegunAimingTransition = false;
                            machinegunAimingFrame = 0;
                        }
                    }
                    else {
                        if (machinegunAimingFrame >= 4) {
                            machinegunAimingFrame = 0;
                        }
                    }
                }
                torsoAnim = TorsoAnim::MACHINEGUN_AIMING_TRANSITION;
            }
        }
        if (machinegunShootingUp) {
            machinegunShootUpTimer += dt;
            if (machinegunShootUpTimer >= machinegunShootUpDelay) {
                machinegunShootUpTimer = 0.0f;
                machinegunShootUpFrame++;
                if (machinegunShootUpFrame >= machinegunShootUpFrameCount) {
                    machinegunShootUpFrame = 0;
                    machinegunShootingUp = false;
                }
            }
        }


        // ========== AIMING UP (pistola) ==========
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

    if (machinegunCrouching) {
        // Disparo agachado
        if (machinegunCrouchShooting) {
            machinegunCrouchShootTimer += dt;
            if (machinegunCrouchShootTimer >= machinegunCrouchShootDelay) {
                machinegunCrouchShootTimer = 0.0f;
                machinegunCrouchShootFrame++;
                if (machinegunCrouchShootFrame >= machinegunCrouchShootFrameCount) {
                    machinegunCrouchShootFrame = 0;
                    machinegunCrouchShooting = false;
                }
            }
        }

        // Granada agachado
        if (machinegunCrouchThrowing) {
            machinegunCrouchThrowTimer += dt;
            float currentDelay = (machinegunCrouchThrowFrame >= 4) ? machinegunCrouchThrowEndDelay : machinegunCrouchThrowDelay;
            if (machinegunCrouchThrowTimer >= currentDelay) {
                machinegunCrouchThrowTimer = 0.0f;
                machinegunCrouchThrowFrame++;
                if (machinegunCrouchThrowFrame >= machinegunCrouchThrowFrameCount) {
                    machinegunCrouchThrowFrame = 0;
                    machinegunCrouchThrowing = false;
                }
            }
        }

        // Caminar agachado
        if (velX != 0 && !machinegunCrouchTransition && !machinegunCrouchShooting && !machinegunCrouchThrowing) {
            machinegunCrouchWalking = true;
            machinegunCrouchWalkTimer += dt;
            if (machinegunCrouchWalkTimer >= machinegunCrouchWalkDelay) {
                machinegunCrouchWalkTimer = 0.0f;
                machinegunCrouchWalkFrame++;
                if (machinegunCrouchWalkFrame >= machinegunCrouchWalkFrameCount) {
                    machinegunCrouchWalkFrame = 0;
                }
            }
        }
        else {
            machinegunCrouchWalking = false;
            machinegunCrouchWalkFrame = 0;
            machinegunCrouchWalkTimer = 0.0f;
        }

        // Transición e idle
        if (!machinegunCrouchShooting && !machinegunCrouchThrowing) {
            machinegunCrouchTimer += dt;
            float currentDelay = machinegunCrouchTransition ? machinegunCrouchTransitionDelay : machinegunCrouchIdleDelay;
            if (machinegunCrouchTimer >= currentDelay) {
                machinegunCrouchTimer = 0.0f;
                machinegunCrouchFrame++;

                if (machinegunCrouchTransition) {
                    if (machinegunCrouchFrame >= machinegunCrouchTransitionFrameCount) {
                        machinegunCrouchTransition = false;
                        machinegunCrouchFrame = 0;
                    }
                }
                else {
                    if (!machinegunCrouchWalking && machinegunCrouchFrame >= machinegunCrouchIdleFrameCount) {
                        machinegunCrouchFrame = 0;
                    }
                }
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
        crouchThrowFrame = 0;  
        crouchThrowing = false;
    }

    if (crouching) {
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

        if (crouchThrowing) {
            crouchThrowTimer += dt;
            float currentDelay = (crouchThrowFrame >= 4) ? crouchThrowEndDelay : crouchThrowDelay;
            if (crouchThrowTimer >= currentDelay) {
                crouchThrowTimer = 0.0f;
                crouchThrowFrame++;
                if (crouchThrowFrame >= crouchThrowFrameCount) {
                    crouchThrowFrame = 0;
                    crouchThrowing = false;
                }
            }
        }

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
        if (meleeAttacking) {
            meleeTimer += dt;
            if (meleeTimer >= meleeFrameDelay) {
                meleeTimer = 0.0f;
                meleeFrame++;
                if (meleeFrame >= MELEE_FRAMES) {
                    meleeFrame = MELEE_FRAMES - 1;
                    meleeAttacking = false;
                }
            }
        }
        return;
    }


    // ========== ANIMACIONES NORMALES ==========
    if (!grounded) {
        legsAnim = LegsAnim::JUMPING;
    }
    else if (velX != 0 && !crouchingInput) {
        legsAnim = LegsAnim::WALKING;
    }
    else {
        legsAnim = LegsAnim::IDLE;
    }

    if (!machinegunIdle && !machinegunShooting && !machinegunAimingUp) {
        if (!grounded) {
            torsoAnim = TorsoAnim::JUMPING;
        }
        else if (velX != 0 && !crouchingInput) {
            torsoAnim = TorsoAnim::WALKING;
        }
        else {
            torsoAnim = TorsoAnim::IDLE;
        }
    }

    if (torsoAnim == TorsoAnim::IDLE) {
        idleTimer += dt;
        if (idleTimer >= 0.15f) {
            idleTimer = 0.0f;
            idleFrame = (idleFrame + 1) % 4;
        }
    }

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
    // Melee
if (meleeAttacking) {
    meleeTimer += dt;
    if (meleeTimer >= meleeFrameDelay) {
        meleeTimer = 0.0f;
        meleeFrame++;
        if (meleeFrame >= MELEE_FRAMES) {
            meleeFrame = MELEE_FRAMES - 1;
            meleeAttacking = false;
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

void PlayerAnim::StartMachinegunIdle() {
    machinegunShooting = false;
    machinegunIdle = true;
    machinegunIdleFrame = 0;
    machinegunIdleTimer = 0.0f;
    machinegunShootCooldown = 0.0f;
    torsoAnim = TorsoAnim::MACHINEGUN_IDLE;
}

void PlayerAnim::StartMachinegunShoot() {
    machinegunIdle = false;
    machinegunShooting = true;
    machinegunShootFrame = 0;
    machinegunShootTimer = 0.0f;
    machinegunShootCooldown = 0.0f;
    torsoAnim = TorsoAnim::MACHINEGUN_SHOOTING;
}

void PlayerAnim::StopMachinegun() {
    machinegunIdle = false;
    machinegunShooting = false;
    machinegunIdleFrame = 0;
    machinegunShootFrame = 0;
    machinegunShootCooldown = 0.0f;
    torsoAnim = TorsoAnim::IDLE;
}

void PlayerAnim::StartMachinegunThrow() {
    machinegunIdle = false;
    machinegunShooting = false;
    machinegunThrowing = true;
    machinegunThrowFrame = 0;
    machinegunThrowTimer = 0.0f;
    machinegunThrowCooldown = 0.0f;
    torsoAnim = TorsoAnim::MACHINEGUN_THROWING;
}

void PlayerAnim::StartMachinegunAiming() {
    machinegunAimingUp = true;
    machinegunAimingTransition = true;
    machinegunAimingFrame = 0;
    machinegunAimingTimer = 0.0f;
}

void PlayerAnim::StopMachinegunAiming() {
    machinegunAimingUp = false;
    machinegunAimingTransition = true;
    machinegunAimingFrame = 0;
    machinegunAimingTimer = 0.0f;
    torsoAnim = TorsoAnim::IDLE;  // <- añadir esto
}

void PlayerAnim::StartMachinegunShootUp() {
    machinegunShootingUp = true;
    machinegunShootUpFrame = 0;
    machinegunShootUpTimer = 0.0f;
    machinegunAimingUp = true;
}

void PlayerAnim::StartMachinegunCrouch() {
    machinegunCrouching = true;
    machinegunCrouchTransition = true;
    machinegunCrouchFrame = 0;
    machinegunCrouchTimer = 0.0f;
}

void PlayerAnim::StopMachinegunCrouch() {
    machinegunCrouching = false;
    machinegunCrouchTransition = false;
    machinegunCrouchFrame = 0;
    machinegunCrouchTimer = 0.0f;
    machinegunCrouchWalking = false;
    machinegunCrouchWalkFrame = 0;
    machinegunCrouchShooting = false;
    machinegunCrouchShootFrame = 0;
    machinegunCrouchThrowing = false;
    machinegunCrouchThrowFrame = 0;
    machinegunCrouchThrowCooldown = 0.0f;
}

void PlayerAnim::StartMachinegunCrouchThrow() {
    machinegunCrouchThrowing = true;
    machinegunCrouchThrowFrame = 0;
    machinegunCrouchThrowTimer = 0.0f;
    machinegunCrouchThrowCooldown = 0.0f;
}

void PlayerAnim::StartMachinegunCrouchShoot() {
    machinegunCrouchShooting = true;
    machinegunCrouchShootFrame = 0;
    machinegunCrouchShootTimer = 0.0f;
}

void PlayerAnim::StartCrouchThrow() {
    if (crouchThrowing) return;
    crouchThrowing = true;
    crouchThrowFrame = 0;
    crouchThrowTimer = 0.0f;
}

void PlayerAnim::LoadParachute() {
    Image img = LoadImage("Graphics/parachute1FORPOL.png");
    parachuteSheet = LoadTextureFromImage(img);
    UnloadImage(img);
    SetTextureFilter(parachuteSheet, TEXTURE_FILTER_POINT);
}

void PlayerAnim::UnloadParachute() {
    UnloadTexture(parachuteSheet);
}

void PlayerAnim::UpdateParachute(float dt) {
    if (!parachuteActive) return;
    parachuteTimer += dt;
    if (parachuteTimer >= parachuteDelay) {
        parachuteTimer = 0.0f;
        // cicla entre 0 y 4, el último frame se queda fijo (abierto)
        if (parachuteFrame < parachuteFrameCount - 1)
            parachuteFrame++;
    }
}

void PlayerAnim::DrawParachute(Vector2 playerPos, float scale, bool facingLeft) {
    if (!parachuteActive) return;

    Rectangle src = {
        parachuteFrame * PARACHUTE_W,
        0.0f,
        facingLeft ? -PARACHUTE_W : PARACHUTE_W,
        PARACHUTE_H
    };

    // Centrar el paracaídas horizontalmente sobre el player y ponerlo encima
    float destX = playerPos.x + ( 15.0f * scale) - (PARACHUTE_W * scale / 2.0f);
    float destY = playerPos.y - PARACHUTE_H * scale + 30.0f;

    DrawTexturePro(parachuteSheet, src,
        { destX, destY, PARACHUTE_W * scale, PARACHUTE_H * scale },
        { 0, 0 }, 0, WHITE);
}

void PlayerAnim::LoadParachute2() {
    Image img = LoadImage("Graphics/pARACHUTE2POOOOOOOL.png");
    parachuteSheet2 = LoadTextureFromImage(img);
    UnloadImage(img);
    SetTextureFilter(parachuteSheet2, TEXTURE_FILTER_POINT);
}

void PlayerAnim::UnloadParachute2() {
    UnloadTexture(parachuteSheet2);
}

void PlayerAnim::UpdateParachuteLanding(float dt) {
    if (!parachuteLanding) return;
    parachuteLandingTimer += dt;
    if (parachuteLandingTimer >= parachuteLandingDelay) {
        parachuteLandingTimer = 0.0f;
        if (parachuteLandingFrame < parachuteLandingFrameCount - 1)
            parachuteLandingFrame++;
        else
            parachuteLanding = false;  // termina sola al llegar al último frame
    }
}

void PlayerAnim::DrawParachuteLanding(Vector2 playerPos, float scale, bool facingLeft) {
    if (!parachuteLanding) return;

    // ← usa playerPos directamente, no landingPosition
    float destX = playerPos.x + (15.0f * scale) - (PARACHUTE2_W * scale / 2.0f) - 150.0f;  // ← más a la derecha
    float destY = playerPos.y - PARACHUTE2_H * scale / 2.0f + 70.0f;  // ← más abajo

    Rectangle src = {
        parachuteLandingFrame * PARACHUTE2_W,
        0.0f,
        !facingLeft ? -PARACHUTE2_W : PARACHUTE2_W,  // ← invertido
        PARACHUTE2_H
    };
    DrawTexturePro(parachuteSheet2, src,
        { destX, destY, PARACHUTE2_W * scale, PARACHUTE2_H * scale },
        { 0, 0 }, 0, WHITE);
}

void PlayerAnim::StartParachuteLanding() {
    parachuteLanding = true;
    parachuteLandingFrame = 0;
    parachuteLandingTimer = 0.0f;
    hasLandingPosition = false;  // ← Resetear al empezar
}
void PlayerAnim::StopParachuteLanding() {
    parachuteLanding = false;
    parachuteLandingFrame = 0;
    hasLandingPosition = false;  // ← Resetear
}

void PlayerAnim::StartMelee() {
    meleeAttacking = true;
    meleeFrame = 0;
    meleeTimer = 0.0f;
}

void PlayerAnim::StartP1Anim(int loops) {
    p1AnimActive = true;
    p1AnimFrame = 0;
    p1AnimTimer = 0.0f;
    p1AnimLoopCount = 0;
    p1AnimMaxLoops = loops;  // ← guarda el valor
}

void PlayerAnim::UpdateP1Anim(float dt) {
    if (!p1AnimActive) return;
    p1AnimTimer += dt;

    float currentDelay = (p1AnimFrame == 3) ? p1AnimDelay * 4.0f : p1AnimDelay;  // ← frame 3 va más lento

    if (p1AnimTimer >= currentDelay) {
        p1AnimTimer = 0.0f;
        p1AnimFrame++;
        if (p1AnimFrame >= P1_ANIM_FRAMES) {
            p1AnimFrame = 0;
            p1AnimLoopCount++;
            if (p1AnimLoopCount >= p1AnimMaxLoops)
                p1AnimActive = false;
        }
    }
}

void PlayerAnim::DrawP1Anim(Vector2 playerPos, float scale, bool facingLeft) const {
    if (!p1AnimActive) return;
    Rectangle src = { p1AnimFrame * P1_ANIM_W, 0, P1_ANIM_W, P1_ANIM_H - 1.0f }; 
    float destX = playerPos.x + (17.0f * scale) - (P1_ANIM_W * scale / 2.0f);
    float destY = playerPos.y - P1_ANIM_H * scale - 2.0f * scale;  // ← encima de la cabeza
    DrawTexturePro(texP1Anim, src,
        { destX, destY, P1_ANIM_W * scale, P1_ANIM_H * scale },
        { 0,0 }, 0, WHITE);
}

void PlayerAnim::StartJetFire() {
    if (!jetFireLoaded) {
        jetFireSheet = LoadTexture("Graphics/jetpack/flyfireanim16x64.png");
        SetTextureFilter(jetFireSheet, TEXTURE_FILTER_POINT);
        jetFireLoaded = true;
    }
    jetFireFrame = 0;
    jetFireTimer = 0.0f;
    jetFireActive = true;
    jetFireLooping = false;
    jetFireEnding = false;
}

void PlayerAnim::StopJetFire() {
    if (!jetFireActive) return;
    if (jetFireLooping) {
        jetFireEnding = true;
        jetFireLooping = false;
        jetFireFrame = 10;
    }
}

void PlayerAnim::UpdateJetFire(float dt, bool thrusting) {
    if (!jetFireActive) return;

    jetFireTimer += dt;
    if (jetFireTimer < JET_FIRE_DELAY) return;
    jetFireTimer = 0.0f;

    if (!jetFireLooping && !jetFireEnding) {
        // Fase inicial 0-4
        jetFireFrame++;
        if (jetFireFrame >= 5) {
            if (thrusting) {
                jetFireLooping = true;
                jetFireFrame = 5;
            }
            else {
                jetFireEnding = true;
                jetFireFrame = 10;
            }
        }
    }
    else if (jetFireLooping) {
        // Loop 5-9
        jetFireFrame++;
        if (jetFireFrame > 9) jetFireFrame = 5;
        if (!thrusting) {
            jetFireEnding = true;
            jetFireLooping = false;
            jetFireFrame = 10;
        }
    }
    else if (jetFireEnding) {
        // Final 10-14
        jetFireFrame++;
        if (jetFireFrame >= 15) {
            jetFireActive = false;
            jetFireFrame = 0;
        }
    }
}

void PlayerAnim::DrawJetFire(Vector2 playerPos, float scale, bool facingLeft, bool hasMachinegun) const {
    if (!jetFireActive || !jetFireLoaded) return;
    Rectangle src = { jetFireFrame * JET_FIRE_W, 0, JET_FIRE_W, JET_FIRE_H };

    float x = facingLeft
        ? playerPos.x + (hasMachinegun ? 22.0f : 15.0f) * scale  
        : playerPos.x + (hasMachinegun ? -5.0f : 3.0f) * scale;   
    float y = playerPos.y + (hasMachinegun ? 20.0f : 20.0f) * scale; 

    Rectangle dst = { x, y, JET_FIRE_W * scale, JET_FIRE_H * scale };
    DrawTexturePro(jetFireSheet, src, dst, { 0,0 }, 0, WHITE);
}

void PlayerAnim::StartJetLanding() {
    if (!jetLandLoaded) {
        jetLandSheet = LoadTexture("Graphics/jetpack/landinganim140x40.png");
        SetTextureFilter(jetLandSheet, TEXTURE_FILTER_POINT);
        jetLandLoaded = true;
    }
    jetLandFrame = 0;
    jetLandTimer = 0.0f;
    jetLandActive = true;
}

void PlayerAnim::UpdateJetLanding(float dt) {
    if (!jetLandActive) return;
    jetLandTimer += dt;
    if (jetLandTimer < JET_LAND_DELAY) return;
    jetLandTimer = 0.0f;
    jetLandFrame++;
    if (jetLandFrame >= 8)
        jetLandActive = false;
}

void PlayerAnim::DrawJetLanding(Vector2 playerPos, float scale) const {
    if (!jetLandActive || !jetLandLoaded) return;
    Rectangle src = { jetLandFrame * JET_LAND_W, 0, JET_LAND_W, JET_LAND_H };
    float x = playerPos.x + (34.0f * scale / 2.0f) - (JET_LAND_W * scale / 2.0f);
    float y = playerPos.y + 40.0f * scale - JET_LAND_H * scale;
    Rectangle dst = { x, y, JET_LAND_W * scale, JET_LAND_H * scale };
    DrawTexturePro(jetLandSheet, src, dst, { 0,0 }, 0, WHITE);
}

void PlayerAnim::StartJetSmoke() {
    if (!jetSmokeLoaded) {
        jetSmokeSheet = LoadTexture("Graphics/jetpack/smokefromjetpack48x90.png");
        SetTextureFilter(jetSmokeSheet, TEXTURE_FILTER_POINT);
        jetSmokeLoaded = true;
    }
}

void PlayerAnim::UpdateJetSmoke(float dt) {
    jetSmokeTimer += dt;
    if (jetSmokeTimer < JET_SMOKE_DELAY) return;
    jetSmokeTimer = 0.0f;
    jetSmokeFrame = (jetSmokeFrame + 1) % 10;
}

void PlayerAnim::DrawJetSmoke(Vector2 playerPos, float scale, bool facingLeft) const {
    if (!jetSmokeLoaded) return;
    Rectangle src = { jetSmokeFrame * JET_SMOKE_W, 0, JET_SMOKE_W, JET_SMOKE_H };
    float x = facingLeft
        ? playerPos.x - 0.0f * scale
        : playerPos.x - 14.0f * scale;
    float y = playerPos.y - 54.0f * scale;
    Rectangle dst = { x, y, JET_SMOKE_W * scale, JET_SMOKE_H * scale };
    DrawTexturePro(jetSmokeSheet, src, dst, { 0,0 }, 0, WHITE);
}