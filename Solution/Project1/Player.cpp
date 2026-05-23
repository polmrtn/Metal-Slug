#include "Player.hpp"
#include "GlobalManagers.hpp"

// ========== CONSTRUCTOR / DESTRUCTOR ==========
Player::Player() {
    SetNormalHitbox();
}

void Player::Init() {
    anim.LoadTextures();
    anim.StartParachute();
}

Player::~Player() {
    anim.UnloadTextures();
}

void Player::ResetToStart() {
    pos = { 300.0f, -50.0f };
    vel = { 0.0f, 0.0f };
    fallingTimer = 0.0f;
    isAlive = true;
    isDisappeared = false;
    invincibilityTimer = 0.0f;
    deathPosition = { 0.0f, 0.0f };
    currentWeapon = WeaponType::PISTOL;
    machinegunAmmo = 0;
    isFalling = true;        
    grounded = false;        
    dyingInAir = false;
    deathTimer = 0.0f;
    meleeAttacking = false;
    hasJetpack = false;
    jetpackFuel = 0.0f;
    anim.StopMachinegun();
    anim.StopMachinegunAiming();
    anim.StopMachinegunCrouch();
    anim.StopParachute();
    anim.StartParachute();   
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
void Player::Update(float CameraLeftLimit, float CameraTop) {
    if (isFalling) {
        blinkTimer += GetFrameTime();
        if (blinkTimer >= blinkDelay) {
            blinkTimer = 0.0f;
            blinkVisible = !blinkVisible;
        }

        fallingTimer += GetFrameTime();

        if (fallingTimer >= PARACHUTE_RELEASE_TIME && anim.IsParachuteActive()) {
            // Soltar paracaídas y caer con gravedad
            anim.StopParachute();
            anim.StartParachuteLanding();
            vel.y = 0.0f;  // empieza desde 0 la gravedad
        }

        if (anim.IsParachuteActive()) {
            pos.y += fallSpeed;  // cae lento con paracaídas
        }
        else {
            // Cae con gravedad normal
            vel.y += PARACHUTE_FALL_GRAVITY;
            pos.y += vel.y;
        }

        anim.UpdateParachute(GetFrameTime());
        anim.UpdateParachuteLanding(GetFrameTime());
        anim.UpdateP1Anim(GetFrameTime());

        if (anim.IsParachuteActive() && !anim.IsP1AnimActive())
            anim.StartP1Anim();

        if (grounded) {
            isFalling = false;
            blinkVisible = true;
            fallingTimer = 0.0f;
            vel.y = 0.0f;
            anim.StopParachute();
        }
        return;
    }
    anim.UpdateParachuteLanding(GetFrameTime());
    anim.UpdateP1Anim(GetFrameTime());

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

        // ── CAÍDA EN MUERTE EN EL AIRE ──────────────────────────────
        if (dyingInAir) {
            previousY = pos.y;          // necesario para que PlayerBlockCollision use prevFeetY correcto
            vel.y += deathFallGravity;
            pos.y += vel.y;

            if (grounded) {             // grounded lo setea SystemCollision justo antes de Update
                vel.y = 0.0f;
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
    if (pendingStopAimingUp && !anim.IsMachinegunShootingUp()) {
        pendingStopAimingUp = false;
        aimingUp = false;
        anim.StopMachinegunAiming();
    }

    anim.Update(grounded, inputVelX, crouching, aimingUp, currentWeapon == WeaponType::MACHINEGUN, GetFrameTime());

    // Limpiar el pendingStopAimingUp cuando la animación de disparo hacia arriba termina
    if (pendingStopAimingUp && !anim.IsMachinegunShootingUp()) {
        pendingStopAimingUp = false;
        aimingUp = false;
        anim.StopMachinegunAiming();
    }

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
 
    }

    // Melee timer
    if (meleeAttacking) {
        meleeTimer += GetFrameTime();
        if (meleeTimer >= MELEE_DURATION)
            meleeAttacking = false;
    }

    // Física y movimiento
    if (rampGroundedFrames > 0) {
        rampGroundedFrames--;
        grounded = true;
    }
    if (!grounded) {
        vel.y += GRAVITY;
        if (vel.y > 40.0f) vel.y = 40.0f;
        pos.y += vel.y;

        float camTop = CameraTop;
        if (pos.y + hitboxHeight * 0.8f < camTop) {  // ← solo el 20% superior (casi los pies)
            pos.y = camTop - hitboxHeight * 0.8f;
            vel.y = 0.0f;
        }
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
    if (isFalling) return;
    if (mode != Mode::FULL_BODY && !crouching) {
        if (currentWeapon == WeaponType::MACHINEGUN && anim.IsMachinegunCrouchShooting()) return;
        inputVelX = -MOVE_SPEED;
        if (!leftCollision) vel.x = -MOVE_SPEED;
        else vel.x = 0;
        dir = PlayerDirection::LEFT;
    }
    else if (crouching && !anim.IsCrouchShooting()) {
            if (meleeAttacking) {
        vel.x = 0;
        inputVelX = 0;
        return;
    }
        if (currentWeapon == WeaponType::MACHINEGUN &&
            (anim.IsMachinegunCrouchShooting() || anim.IsMachinegunCrouchThrowing())) return;
        inputVelX = -CROUCH_SPEED;
        if (!leftCollision) vel.x = -CROUCH_SPEED;
        else vel.x = 0;
        dir = PlayerDirection::LEFT;
    }
}

void Player::MoveRight() {
    if (isFalling) return;
    if (mode != Mode::FULL_BODY && !crouching) {
        if (currentWeapon == WeaponType::MACHINEGUN && anim.IsMachinegunCrouchShooting()) return;
        inputVelX = MOVE_SPEED;
        if (!rightCollision) vel.x = MOVE_SPEED;
        else vel.x = 0;
        dir = PlayerDirection::RIGHT;
    }
    else if (crouching && !anim.IsCrouchShooting()) {
        if (meleeAttacking) {
            vel.x = 0;
            inputVelX = 0;
            return;
        }
        if (currentWeapon == WeaponType::MACHINEGUN &&
            (anim.IsMachinegunCrouchShooting() || anim.IsMachinegunCrouchThrowing())) return;
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
        rampGroundedFrames = 0;  
        anim.ResetJump();
    }
}

void Player::StartAimingUp() {
    if (!crouching && mode != Mode::FULL_BODY) {
        aimingUp = true;
        if (currentWeapon == WeaponType::MACHINEGUN) {
            anim.StartMachinegunAiming();
        }
    }
}

void Player::StopAimingUp() {
    // Si estamos disparando hacia arriba, no detener la orientación hasta que termine
    if (currentWeapon == WeaponType::MACHINEGUN && anim.IsMachinegunShootingUp()) {
        pendingStopAimingUp = true;
        return;
    }
    // Si estamos en la animación de disparo hacia arriba pero ya terminó
    if (currentWeapon == WeaponType::MACHINEGUN && !anim.IsMachinegunShootingUp() && pendingStopAimingUp) {
        pendingStopAimingUp = false;
    }
    pendingStopAimingUp = false;
    aimingUp = false;
    anim.StopMachinegunAiming();
}

void Player::StartCrouching() {
    if (grounded && !crouching && mode != Mode::FULL_BODY) {
        float feetY = pos.y + GetHeight();
        crouching = true;
        aimingUp = false;
        SetCrouchHitbox();
        if (currentWeapon == WeaponType::MACHINEGUN) {
            anim.StartMachinegunCrouch();
        }
        else {
            anim.ForceCrouch();
        }
        pos.y = feetY - GetHeight();
    }
}

void Player::StopCrouching() {
    if (crouching) {
        float currentY = pos.y;
        crouching = false;
        SetNormalHitbox();
        pos.y = currentY - 60.0f;
        if (currentWeapon == WeaponType::MACHINEGUN) {
            anim.StopMachinegunCrouch();
        }
    }
}

void Player::Shoot() {
    if (mode != Mode::FULL_BODY) {
        if (crouching) {
            vel.x = 0;
            if (currentWeapon == WeaponType::MACHINEGUN) {
                anim.StartMachinegunCrouchShoot();
            }
            else {
                anim.StartCrouchShoot();
            }
        }
        else if (aimingUp) {
            if (currentWeapon == WeaponType::MACHINEGUN) {
                anim.StartMachinegunShootUp();
            }
            else {
                if (anim.IsShooting()) anim.ForceStopShoot();
                anim.StartShootUp();
            }
        }
        else {
            if (currentWeapon == WeaponType::MACHINEGUN) {
                anim.StartMachinegunShoot();
            }
            else {
                anim.StartShoot();
            }
        }
    }
}

// ========== GETTERS ==========
PlayerDirection Player::GetAimDirection() const {
    if (aimingUp || (currentWeapon == WeaponType::MACHINEGUN && anim.IsMachinegunShootingUp())) {
        return PlayerDirection::UP;
    }
    return dir;
}

// ========== GRANADA ==========
GrenadeThrowData Player::ThrowGrenade() {
    if (crouching) {
        vel.x = 0;
        inputVelX = 0;
        if (currentWeapon == WeaponType::MACHINEGUN) {
            anim.StartMachinegunCrouchThrow();
        }
        else {
            anim.StartCrouchThrow();
        }
    }
    else if (currentWeapon == WeaponType::MACHINEGUN) {
        anim.StartMachinegunThrow();
    }
    else {
        anim.StartThrow();
    }

    GrenadeThrowData data;
    data.startPos = { pos.x + hitboxWidth / 2.0f, pos.y + hitboxHeight * 0.3f };
    data.valid = true;

    switch (dir) {
    case PlayerDirection::LEFT:
        data.startPos = { pos.x - 20.0f, pos.y + hitboxHeight * 0.3f };  // sale por la izquierda
        data.initialVelocity = { -400.0f, -350.0f };
        break;
    case PlayerDirection::RIGHT:
        data.startPos = { pos.x + hitboxWidth + 20.0f, pos.y + hitboxHeight * 0.3f };  // sale por la derecha
        data.initialVelocity = { 400.0f, -350.0f };
        break;
    case PlayerDirection::UP:
        data.startPos = { pos.x + hitboxWidth / 2.0f, pos.y - 20.0f };
        data.initialVelocity = { 0.0f, -600.0f };
        break;
    default:
        data.startPos = { pos.x + hitboxWidth + 20.0f, pos.y + hitboxHeight * 0.3f };
        data.initialVelocity = { 400.0f, -350.0f };
        break;
    }
    return data;
}

// ========== ARMAS ==========
void Player::EquipMachinegun() {
    currentWeapon = WeaponType::MACHINEGUN;
    machinegunAmmo += MACHINEGUN_MAX_AMMO;
    anim.StartMachinegunIdle();
}

void Player::UseAmmo() {
    if (currentWeapon == WeaponType::MACHINEGUN) {
        machinegunAmmo--;
        if (machinegunAmmo <= 0) {
            currentWeapon = WeaponType::PISTOL;
            anim.StopMachinegun();
            anim.StopMachinegunAiming();
            anim.StopMachinegunCrouch();
        }
    }
}

// ========== MUERTE Y RESPAWN ==========
void Player::TakeDamage() {
    if (!isAlive) return;
if (IsInvincible()) return;

    isAlive = false;
    deathTimer = 0.0f;
    isDisappeared = false;

    mode = Mode::FULL_BODY;
    special = SpecialAnim::DEATH;
    specialTimer = 0.0f;
    specialDuration = 1.0f;

    inputVelX = 0;
    vel.x = 0.0f;

    if (grounded) {
        vel.y = 0.0f;
        dyingInAir = false;
        // Si estaba agachado, ajusta pos.y para que los pies queden en el mismo sitio
        if (crouching) {
            float feetY = pos.y + hitboxHeight;  // pies actuales (agachado)
            pos.y = feetY - GetNormalHeight();    // reposiciona como si estuviera de pie
        }
        deathPosition = pos;
    }
    else {
        dyingInAir = true;
        // vel.y mantiene la velocidad actual
    }
}

void Player::Respawn() {
    pos = deathPosition;
    isAlive = true;
    isDisappeared = false;
    deathTimer = 0.0f;

    mode = Mode::FULL_BODY;
    special = SpecialAnim::RESPAWN;
    specialTimer = 0.0f;
    specialDuration = 0.6f;

    vel = { 0.0f, 0.0f };
    invincibilityTimer = invincibilityDuration;

    // Random machinegun al respawnear cerca del boss
    if (GetRandomValue(0, 1) == 1) {
        EquipMachinegun();
    }
    anim.StartP1Anim(3);
}

// ========== HITBOXES ==========
Rectangle Player::GetHitBox() {
    float spriteTotalWidth = 34.0f * SCALE;
    float hitboxX = pos.x + (spriteTotalWidth - hitboxWidth) / 2.0f;
    float hitboxY = pos.y + hitboxOffsetY + 15.0f;  // ← sube el inicio
    return Rectangle{ hitboxX, hitboxY, hitboxWidth, hitboxHeight - 15.0f };  // ← reduce altura
}

Rectangle Player::GetLeftHitBox() {
    Rectangle mainHitBox = GetHitBox();
    float reducedHeight = mainHitBox.height * 0.4f;
    float offsetY = (mainHitBox.height - reducedHeight) / 2.0f - 35.0f;
    float hitboxX = mainHitBox.x - (mainHitBox.width * 0.3f);
    return Rectangle{ hitboxX, mainHitBox.y + offsetY, mainHitBox.width * 0.4f, reducedHeight };
}

Rectangle Player::GetRightHitBox() {
    Rectangle mainHitBox = GetHitBox();
    float reducedHeight = mainHitBox.height * 0.4f;
    float offsetY = (mainHitBox.height - reducedHeight) / 2.0f - 35.0f;
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
    // ========== NUEVO: Animación de caída inicial (fila 30) ==========
    if (isFalling) {
        // Dibujar paracaídas
        anim.DrawParachute(pos, SCALE, dir == PlayerDirection::LEFT);
        anim.DrawParachuteLanding(pos, SCALE, dir == PlayerDirection::LEFT);
        anim.DrawP1Anim(pos, SCALE, dir == PlayerDirection::LEFT);

        if (!blinkVisible) return;

        // Sprite de la fila 30 (30 * 34 = 1020), ancho 34, alto 68
        Rectangle sourceRect = { 0, 29 * 34.0f, 34.0f, 68.0f };

        if (dir == PlayerDirection::LEFT) {
            sourceRect.width = -34.0f;
        }

        // Ajuste vertical para que los pies toquen el hitbox
        float offsetY = -68.0f * SCALE + GetHeight();

        // Prioridad melee
        if (anim.IsMeleeAttacking()) {
            bool mg = (currentWeapon == WeaponType::MACHINEGUN);
            float rowY = anim.GetMeleeRowY(mg, false);
            float drawX = pos.x;
            Rectangle sourceRect = { anim.GetMeleeFrame() * 64.0f, rowY, 64.0f, 64.0f };
            if (dir == PlayerDirection::LEFT) {
                sourceRect.width = -64.0f;
                drawX = pos.x - (64.0f - 34.0f) * SCALE;
            }
            Rectangle destRect = { drawX, pos.y, 64.0f * SCALE, 64.0f * SCALE };
            DrawTexturePro(anim.GetSheet(), sourceRect, destRect, { 0,0 }, 0, WHITE);
            return;
        }

        Rectangle destRect = {
            pos.x,
            pos.y + offsetY,
            34.0f * SCALE,
            68.0f * SCALE
        };

        DrawTexturePro(anim.GetSheet(), sourceRect, destRect, { 0, 0 }, 0, WHITE);
        DrawHitBox();
        return;
    }

    // Resto del código original de Draw()...
    if (!isAlive && isDisappeared) return;

    if (anim.IsP1AnimActive())
        anim.DrawP1Anim(pos, SCALE, dir == PlayerDirection::LEFT);

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
    anim.DrawParachute(pos, SCALE, dir == PlayerDirection::LEFT);
    DrawSeparated();
    anim.DrawParachuteLanding(pos, SCALE, dir == PlayerDirection::LEFT);
    DrawHitBox();
}

void Player::DrawHitBox() {
    DrawRectangleLinesEx(GetHitBox(), 2, WHITE);
    DrawRectangleLinesEx(GetLeftHitBox(), 2, RED);
    DrawRectangleLinesEx(GetRightHitBox(), 2, BLUE);
    DrawRectangleLinesEx(GetMeleeHitBox(), 2, YELLOW);
}

// ========== DIBUJO AGACHADO ==========
void Player::DrawCrouch() {
    Rectangle sourceRect;
    float currentHeight;
    float rowY;
    float yOffset;
    float drawX = pos.x;

    if (anim.IsMeleeAttacking()) {
        bool mg = (currentWeapon == WeaponType::MACHINEGUN);
        float rowY = anim.GetMeleeRowY(mg, true);
        float meleeDrawX = pos.x - (68.0f - 34.0f) / 2.0f * SCALE + (dir == PlayerDirection::RIGHT ? 30.0f : -30.0f);
        Rectangle meleeSrc = { anim.GetMeleeFrame() * 68.0f, rowY, 68.0f, 68.0f };
        if (dir == PlayerDirection::LEFT) meleeSrc.width = -68.0f;
        float destY = pos.y + hitboxHeight - 68.0f * SCALE;  // ← nuevo
        Rectangle destRect = { meleeDrawX, destY, 68.0f * SCALE, 68.0f * SCALE };  // ← nuevo
        DrawTexturePro(anim.GetSheet(), meleeSrc, destRect, { 0,0 }, 0, WHITE);
        return;
    }

    if (anim.IsMachinegunCrouching()) {

        // Prioridad 1: Disparo agachado machinegun
        if (anim.IsMachinegunCrouchShooting()) {
            rowY = anim.GetRowMachinegunCrouchShoot();
            currentHeight = 34.0f;
            yOffset = -35.0f;  // ajustar a ojo
            sourceRect = { anim.GetMachinegunCrouchShootFrame() * 68.0f, rowY, 68.0f, currentHeight };
            if (dir == PlayerDirection::LEFT) {
                sourceRect.width = -68.0f;
                drawX = pos.x - (68.0f - 34.0f) * SCALE;
            }
            Rectangle destRect = { drawX, pos.y + yOffset, 68.0f * SCALE, currentHeight * SCALE };
            DrawTexturePro(anim.GetSheet(), sourceRect, destRect, { 0,0 }, 0, WHITE);
            return;
        }

        // Prioridad 2: Granada agachado machinegun
        if (anim.IsMachinegunCrouchThrowing()) {
            rowY = anim.GetRowMachinegunCrouchThrow();
            currentHeight = anim.GetMachinegunCrouchThrowH();
            yOffset = -165.0f;  // ajustar a ojo
            sourceRect = { anim.GetMachinegunCrouchThrowFrame() * 68.0f, rowY, 68.0f, currentHeight };
            if (dir == PlayerDirection::LEFT) {
                sourceRect.width = -68.0f;
                drawX = pos.x - (68.0f - 34.0f) * SCALE;
            }
            Rectangle destRect = { drawX, pos.y + yOffset, 68.0f * SCALE, currentHeight * SCALE };
            DrawTexturePro(anim.GetSheet(), sourceRect, destRect, { 0,0 }, 0, WHITE);
            return;
        }

        // Prioridad 3: Caminar agachado machinegun
        if (anim.IsMachinegunCrouchWalking()) {
            rowY = anim.GetRowMachinegunCrouchWalk();
            currentHeight = 34.0f;
            yOffset = -35.0f;  // ajustar a ojo
            sourceRect = { anim.GetMachinegunCrouchWalkFrame() * 68.0f, rowY, 68.0f, currentHeight };
            if (dir == PlayerDirection::LEFT) {
                sourceRect.width = -68.0f;
                drawX = pos.x - (68.0f - 34.0f) * SCALE;
            }
            Rectangle destRect = { drawX, pos.y + yOffset, 68.0f * SCALE, currentHeight * SCALE };
            DrawTexturePro(anim.GetSheet(), sourceRect, destRect, { 0,0 }, 0, WHITE);
            return;
        }

        // Prioridad 4: Transición e idle
        if (anim.IsMachinegunCrouchTransition()) {
            rowY = anim.GetRowMachinegunCrouchTransition();
            currentHeight = anim.GetMachinegunCrouchTransitionH();
            yOffset = -165.0f;
            sourceRect = { anim.GetMachinegunCrouchFrame() * 68.0f, rowY, 68.0f, currentHeight };
            if(dir != PlayerDirection::LEFT) drawX = pos.x + 10.0f;
        }
        else {
            rowY = anim.GetRowMachinegunCrouchIdle();
            currentHeight = anim.GetMachinegunCrouchIdleH();
            yOffset = -35.0f;
            sourceRect = { anim.GetMachinegunCrouchFrame() * 68.0f, rowY, 68.0f, currentHeight };
        }

        if (dir == PlayerDirection::LEFT) {
            sourceRect.width = -68.0f;
            drawX = pos.x - (68.0f - 34.0f) * SCALE;
        }
        Rectangle destRect = { drawX, pos.y + yOffset, 68.0f * SCALE, currentHeight * SCALE };
        DrawTexturePro(anim.GetSheet(), sourceRect, destRect, { 0,0 }, 0, WHITE);
        return;
    }

    // ========== PISTOLA CROUCH ==========

    if (anim.IsCrouchThrowing()) {
        rowY = anim.GetCrouchThrowRowY();
        currentHeight = 34.0f;
        yOffset = -25.0f;  // ajustar a ojo
        sourceRect = {
            (float)(anim.GetCrouchThrowFrame() * 68.0f),
            rowY,
            anim.GetCrouchThrowW(),
            currentHeight
        };
        if (dir == PlayerDirection::LEFT) {
            sourceRect.width = -anim.GetCrouchThrowW();
            drawX = pos.x - (anim.GetCrouchThrowW() - 34.0f) * SCALE;
        }
        Rectangle destRect = { drawX, pos.y + yOffset, anim.GetCrouchThrowW() * SCALE, currentHeight * SCALE };
        DrawTexturePro(anim.GetSheet(), sourceRect, destRect, { 0,0 }, 0, WHITE);
        return;
    }

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

    float legsDrawX = pos.x + legsOffsetX * SCALE;
    if (anim.GetLegsAnim() == LegsAnim::JUMPING &&
        (anim.IsMachinegunIdle() || anim.IsMachinegunShooting() || anim.IsMachinegunAiming())) {
        legsDrawX += (dir == PlayerDirection::RIGHT ? -30.0f : 30.0f);
    }

    DrawTexturePro(anim.GetSheet(), legSrc,
        { legsDrawX, baseY + off.legsY * SCALE, w * SCALE, h * SCALE },
        { 0,0 }, 0, WHITE);

    // Torso
    float torsoDrawX = pos.x + torsoOffsetX * SCALE;
    float torsoDrawY = baseY + off.torsoY * SCALE;
    Rectangle torsoSrc;
    Color tint = WHITE;

    // Prioridad: Melee
    if (anim.IsMeleeAttacking()) {
        bool mg = (currentWeapon == WeaponType::MACHINEGUN);
        float rowY = anim.GetMeleeRowY(mg, false);
        // Centrar el 68px respecto a la hitbox de 34px
        float meleeDrawX = pos.x - (68.0f - 34.0f) / 2.0f * SCALE + (dir == PlayerDirection::RIGHT ? 32.0f : -32.0f);
        Rectangle meleeSrc = { anim.GetMeleeFrame() * 68.0f, rowY, 68.0f, 68.0f };
        if (dir == PlayerDirection::LEFT) {
            meleeSrc.width = -68.0f;
        }
        DrawTexturePro(anim.GetSheet(), meleeSrc,
            { meleeDrawX, torsoDrawY - (30.0f * SCALE), 68.0f * SCALE, 68.0f * SCALE },
            { 0,0 }, 0, WHITE);
        return;
    }

    // Prioridad: Lanzar granada
    if (anim.IsThrowing()) {
        torsoSrc = { anim.GetThrowFrame() * w, anim.GetThrowRowY(), w, h };
        if (dir == PlayerDirection::LEFT) torsoSrc.width = -w;
        DrawTexturePro(anim.GetSheet(), torsoSrc,
            { torsoDrawX, torsoDrawY, w * SCALE, h * SCALE },
            { 0,0 }, 0, tint);
        return;
    }

    // Prioridad: Machinegun Idle
    if (anim.IsMachinegunIdle() && !anim.IsMachinegunAiming()) {
        torsoSrc = { anim.GetMachinegunIdleFrame() * 68.0f, anim.GetRowMachinegunIdle(), 68.0f, h };
        float idleDrawX = torsoDrawX;
        if (dir == PlayerDirection::LEFT) {
            torsoSrc.width = -68.0f;
            idleDrawX = torsoDrawX - 140.0f;
        }
        DrawTexturePro(anim.GetSheet(), torsoSrc,
            { idleDrawX, torsoDrawY, 68.0f * SCALE, h * SCALE },
            { 0,0 }, 0, tint);
        return;
    }

    // Prioridad: Machinegun Shooting
    if (anim.IsMachinegunShooting()) {
        torsoSrc = { anim.GetMachinegunShootFrame() * 68.0f, anim.GetRowMachinegunShoot(), 68.0f, h };
        float shootDrawX = torsoDrawX;
        if (dir == PlayerDirection::LEFT) {
            torsoSrc.width = -68.0f;
            shootDrawX = torsoDrawX - 140.0f;
        }
        DrawTexturePro(anim.GetSheet(), torsoSrc,
            { shootDrawX, torsoDrawY, 68.0f * SCALE, h * SCALE },
            { 0,0 }, 0, tint);
        return;
    }

    // Prioridad: Machinegun Throwing
    if (anim.IsMachinegunThrowing()) {
        torsoSrc = { anim.GetMachinegunThrowFrame() * 68.0f, anim.GetRowMachinegunThrow(), 68.0f, h };
        float throwDrawX = torsoDrawX;
        if (dir == PlayerDirection::LEFT) {
            torsoSrc.width = -68.0f;
            throwDrawX = torsoDrawX - 140.0f;
        }
        DrawTexturePro(anim.GetSheet(), torsoSrc,
            { throwDrawX, torsoDrawY, 68.0f * SCALE, h * SCALE },
            { 0,0 }, 0, tint);
        return;
    }

    // Prioridad: Machinegun Shooting Up
    if (anim.IsMachinegunAiming() && anim.IsMachinegunShootingUp()) {
        torsoSrc = { (float)(anim.GetMachinegunShootUpFrame() * 34.0f), anim.GetMachinegunShootUpRowY(), 34.0f, anim.GetMachinegunShootUpH() };
        float shootUpBaseY = torsoDrawY - (68.0f * SCALE) - 20.0f;
        float shootUpBaseX = torsoDrawX - 20.0f;
        if (dir == PlayerDirection::LEFT) {
            torsoSrc.width = -34.0f;
            shootUpBaseX = torsoDrawX + 20.0f;  // ajusta este valor hasta que cuadre
        }
        DrawTexturePro(anim.GetSheet(), torsoSrc,
            { shootUpBaseX, shootUpBaseY, 34.0f * SCALE, anim.GetMachinegunShootUpH() * SCALE },
            { 0,0 }, 0, tint);
        return;
    }

    // Prioridad: Machinegun Aiming
    if (anim.IsMachinegunAiming()) {
        if (anim.IsMachinegunAimingTransition()) {
            // Transición: fila 39, 68 ancho x 34 alto
            torsoSrc = { anim.GetMachinegunAimingFrame() * 68.0f, anim.GetRowMachinegunAiming(), 68.0f, h };
            float aimDrawX = torsoDrawX;
            if (dir == PlayerDirection::LEFT) {
                torsoSrc.width = -68.0f;
                aimDrawX = torsoDrawX - 140.0f;
            }
            DrawTexturePro(anim.GetSheet(), torsoSrc,
                { aimDrawX, torsoDrawY, 68.0f * SCALE, h * SCALE },
                { 0,0 }, 0, tint);
        }
        else {
            // Idle: fila 40-41, 34 ancho x 68 alto
            torsoSrc = { anim.GetMachinegunAimingFrame() * 34.0f, anim.GetRowMachinegunAimingIdle(), 34.0f, 68.0f };
            float aimDrawX = torsoDrawX;
            float aimDrawY = torsoDrawY - (34.0f * SCALE); // ajustar Y porque el sprite es más alto
            if (dir == PlayerDirection::LEFT) {
                torsoSrc.width = -34.0f;
            }
            DrawTexturePro(anim.GetSheet(), torsoSrc,
                { aimDrawX, aimDrawY, 34.0f * SCALE, 68.0f * SCALE },
                { 0,0 }, 0, tint);
        }
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
        float aimOffsetX = 0.0f;
        if (anim.GetLegsAnim() == LegsAnim::JUMPING)
            aimOffsetX = (dir == PlayerDirection::RIGHT) ? 20.0f : -20.0f;  // ← ajusta el valor

        if (anim.IsAimingTransition()) {
            torsoSrc = { (float)(anim.GetAimingFrame() * w), anim.GetAimingTransitionRowY(), w, h };
        }
        else {
            torsoSrc = { (float)(anim.GetAimingFrame() * w), anim.GetAimingIdleRowY(), w, h };
        }
        if (dir == PlayerDirection::LEFT) torsoSrc.width = -w;
        DrawTexturePro(anim.GetSheet(), torsoSrc,
            { torsoDrawX + aimOffsetX, torsoDrawY, w * SCALE, h * SCALE },
            { 0,0 }, 0, tint);
        return;
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

void Player::StartMelee() {
    if (meleeAttacking) return;
    meleeAttacking = true;
    meleeTimer = 0.0f;
    anim.StartMelee();
}

Rectangle Player::GetMeleeHitBox() const {
    Rectangle hb = const_cast<Player*>(this)->GetHitBox();
    float hbW = 60.0f;
    float hbH = 80.0f;
    float hbY = hb.y + hb.height * 0.2f;
    if (dir == PlayerDirection::RIGHT)
        return { hb.x + hb.width, hbY, hbW, hbH };
    else
        return { hb.x - hbW, hbY, hbW, hbH };
}

void Player::EquipJetpack() {
    hasJetpack = true;
    jetpackFuel = JETPACK_MAX_FUEL;
}

void Player::JetpackThrust() {
    if (!hasJetpack || grounded || jetpackFuel <= 0.0f) return;
    vel.y += JETPACK_FORCE;
    if (vel.y < JETPACK_MAX_VEL) vel.y = JETPACK_MAX_VEL;
    jetpackFuel -= JETPACK_FUEL_DRAIN * GetFrameTime();
    if (jetpackFuel <= 0.0f) {
        jetpackFuel = 0.0f;
        hasJetpack = false;  // vuelve al salto normal
    }
}

void Player::FullReset() {
    pos = { 300.0f, -50.0f };
    vel = { 0.0f, 0.0f };
    inputVelX = 0.0f;
    fallingTimer = 0.0f;
    grounded = false;
    aimingUp = false;
    crouching = false;
    dir = PlayerDirection::RIGHT;
    leftCollision = false;
    rightCollision = false;
    mode = Mode::SEPARATED;
    special = SpecialAnim::NONE;
    specialTimer = 0.0f;
    specialDuration = 0.0f;
    isAlive = true;
    deathPosition = { 0.0f, 0.0f };
    invincibilityTimer = 0.0f;
    deathTimer = 0.0f;
    isDisappeared = false;
    currentWeapon = WeaponType::PISTOL;
    machinegunAmmo = 0;
    isFalling = true;
    blinkTimer = 0.0f;
    blinkVisible = true;
    wasOnRamp = false;
    rampGroundedFrames = 0;
    dyingInAir = false;
    meleeAttacking = false;
    meleeTimer = 0.0f;
    hasJetpack = false;
    jetpackFuel = 0.0f;
    SetNormalHitbox();
    anim.StopMachinegun();
    anim.StopMachinegunAiming();
    anim.StopMachinegunCrouch();
    anim.StopParachute();
    anim.StartParachute();
}

void Player::ResetWeapon() {
    currentWeapon = WeaponType::PISTOL;
    machinegunAmmo = 0;
    anim.StopMachinegun();
    anim.StopMachinegunAiming();
    anim.StopMachinegunCrouch();
}

void Player::EquipMachinegunFresh() {
    currentWeapon = WeaponType::MACHINEGUN;
    machinegunAmmo = MACHINEGUN_MAX_AMMO;  // siempre 200 fijos
    anim.StartMachinegunIdle();
}