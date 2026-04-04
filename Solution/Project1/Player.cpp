#include "Player.hpp"

Player::Player() {
    // 1. Cargar Textura y Filtro para Pixel Art
    image = LoadTexture("Graphics/Marco_Sprites - Neutral 1.png");
    SetTextureFilter(image, TEXTURE_FILTER_POINT);

    // 2. Configuración de Escala y Dimensiones
    scale = 4.0f; // Ajusta este valor al tamaño que desees
    normalHeight = (float)image.height;
    crouchHeight = normalHeight * 0.6f;

    // 3. Variables de Física
    position = { 100, 100 };
    velocity = { 0, 0 };
    gravity = 0.8f;
    jumpForce = -14.0f;
    moveSpeed = 5.0f;
    crouchSpeed = 2.5f;
    groundLevel = 800.0f;

    // 4. Estados Iniciales
    isGrounded = false;
    aimingUp = false;
    isCrouching = false;
    direction = PlayerDirection::RIGHT;
}

Player::~Player() {
    UnloadTexture(image);
}

void Player::Update() {
    // Aplicar Gravedad
    if (!isGrounded) {
        velocity.y += gravity;
    }
    else if (velocity.y > 0) {
        velocity.y = 0;
    }

    // Aplicar Movimiento
    position.y += velocity.y;
    position.x += velocity.x;

    // Colisión con el Suelo (Considerando Escala)
    float currentHeight = GetHeight();
    if (position.y + currentHeight >= groundLevel) {
        position.y = groundLevel - currentHeight;
        velocity.y = 0;
        isGrounded = true;
    }
    else {
        isGrounded = false;
    }

    // Límites laterales del mapa
    float currentWidth = GetWidth();
    if (position.x < 0) position.x = 0;
    if (position.x + currentWidth > 7300) position.x = 7300 - currentWidth;
}

void Player::Draw() {
    // Recorte de la imagen (Source)
    Rectangle sourceRect = { 0, 0, (float)image.width, (float)image.height };
    if (direction == PlayerDirection::LEFT) {
        sourceRect.width = -(float)image.width;
    }

    // Área en pantalla (Destino escalado)
    Rectangle destRect = {
        position.x,
        position.y,
        (float)image.width * scale,
        (float)image.height * scale
    };

    Vector2 origin = { 0, 0 };

    // Feedback visual de estados (opcional)
    Color tint = WHITE;
    if (isCrouching) tint = SKYBLUE;
    if (aimingUp) tint = YELLOW;

    DrawTexturePro(image, sourceRect, destRect, origin, 0.0f, tint);
}

Rectangle Player::GetHitBox() {
    return Rectangle{ position.x, position.y, GetWidth(), GetHeight() };
}

void Player::StartCrouching() {
    if (isGrounded && !isCrouching) {
        isCrouching = true;
        // Bajamos la posición Y para que los pies no floten al encogerse
        position.y += (normalHeight - crouchHeight) * scale;
        aimingUp = false;
    }
}

void Player::StopCrouching() {
    if (isCrouching) {
        isCrouching = false;
        // Subimos la posición Y para recuperar la altura normal
        position.y -= (normalHeight - crouchHeight) * scale;
    }
}

void Player::Jump() {
    if (isGrounded && !isCrouching) {
        velocity.y = jumpForce;
        isGrounded = false;
    }
}

void Player::MoveLeft() {
    velocity.x = isCrouching ? -crouchSpeed : -moveSpeed;
    if (!aimingUp) direction = PlayerDirection::LEFT;
}

void Player::MoveRight() {
    velocity.x = isCrouching ? crouchSpeed : moveSpeed;
    if (!aimingUp) direction = PlayerDirection::RIGHT;
}

void Player::StopMovingHorizontal() {
    velocity.x = 0;
}

void Player::StartAimingUp() {
    if (!isCrouching) aimingUp = true;
}

void Player::StopAimingUp() {
    aimingUp = false;
}

PlayerDirection Player::GetAimDirection() const {
    if (aimingUp) return PlayerDirection::UP;
    return direction;
}

void Player::DrawHitBox(bool isColliding) {
    DrawRectangleLinesEx(GetHitBox(), 2, isColliding ? RED : GREEN);
}

void Player::Shoot() {
    // Implementar creación de balas aquí
}
