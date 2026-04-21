#include "Grenade.hpp"
#include <cmath>
#include<vector>
#include "Soldier.hpp" 
#include "raylib.h"

// Definir las variables estáticas
Texture2D Grenade::texture = { 0 };
bool Grenade::textureLoaded = false;

Grenade::Grenade(Vector2 startPos, Vector2 targetPos, float power) {
    this->startPos = startPos;
    this->targetPos = targetPos;
    this->position = startPos;

    // Cargar textura solo una vez
    if (!textureLoaded) {
        texture = LoadTexture("Graphics/marcogrenade.png");
        SetTextureFilter(texture, TEXTURE_FILTER_POINT);
        textureLoaded = true;
        TraceLog(LOG_INFO, "Grenade texture loaded (ID: %d)", texture.id);
    }

    CalculateTrajectory(power);
}

Grenade::~Grenade() {}

void Grenade::CalculateTrajectory(float power) {
    float dx = targetPos.x - startPos.x;
    float dy = targetPos.y - startPos.y;

    // Aumentar el tiempo de vuelo (más tiempo = más lejos)
    float time = power / 1500.0f;
    if (time < 0.08f) time = 0.08f;

    velocity.x = dx / time;
    velocity.y = (dy - 0.5f * gravity * time * time) / time;
}

void Grenade::Update() {
    if (!isActive) return;

    if (hasExploded) {
        explosionAnimTimer += GetFrameTime();
        if (explosionAnimTimer >= explosionFrameDelay) {
            explosionAnimTimer = 0.0f;
            explosionFrame++;
            if (explosionFrame >= explosionTotalFrames) {
                explosionFrame = explosionTotalFrames - 1;
                isExploding = false;
            }
        }
        explosionTimer += GetFrameTime();
        if (explosionTimer >= explosionDuration && explosionFrame >= explosionTotalFrames - 1) {
            isActive = false;
        }
        return;
    }

    // Guardar posición anterior para detección continua
    Vector2 prevPos = position;

    // Actualizar animación
    animationTimer += GetFrameTime();
    if (animationTimer >= frameDelay) {
        animationTimer = 0.0f;
        currentFrame = (currentFrame + 1) % totalFrames;
    }

    // Físicas
    velocity.y += gravity * GetFrameTime();
    position.x += velocity.x * GetFrameTime();
    position.y += velocity.y * GetFrameTime();

    // Detección de colisión con el suelo (continua)
    if (!hasExploded) {
        bool crossedGround = (prevPos.y < targetPos.y && position.y >= targetPos.y);

        if (crossedGround || position.y >= targetPos.y) {
            if (!hasBounced) {
                // PRIMER REBOTE - no explota
                position.y = targetPos.y;
                velocity.y = -velocity.y * bounceDamping;
                hasBounced = true;
            }
            else if (hasBounced) {
                // SEGUNDO CONTACTO - EXPLOTA
                position.y = targetPos.y;
                Explode();
            }
        }
    }
}

void Grenade::CheckCollisionWithSoldiers(std::vector<Soldier>& soldiers) {
    if (!isActive || hasExploded) return;

    for (auto& soldier : soldiers) {
        if (soldier.GetisAlive()) {
            if (CheckCollisionRecs(soldier.GetHurtBox(), GetHitBox())) {
                Explode();  // Explota inmediatamente al tocar al soldado
                return;
            }
        }
    }
}
void Grenade::Explode() {
    hasExploded = true;
    explosionTimer = 0.0f;
    StartExplosion();
}

void Grenade::StartExplosion() {
    isExploding = true;
    explosionAnimTimer = 0.0f;
    explosionFrame = 0;
}

Rectangle Grenade::GetHitBox() const {
    if (hasExploded) {
        return Rectangle{ position.x - explosionRadius, position.y - explosionRadius,
                          explosionRadius * 2, explosionRadius * 2 };
    }
    return Rectangle{ position.x - 11, position.y - 11, 22, 22 };
}

Rectangle Grenade::GetExplosionHitBox() const {
    // Tamaño del sprite de explosión: 52x112
    float width = 52.0f;
    float height = 112.0f;
    float scale = 2.0f;  // Misma escala que usas en Draw()
    float offsetX = -40.0f;  // Mismo offset que usas en Draw()
    float offsetY = -height * scale;  // Base en el suelo

    return Rectangle{
        position.x + offsetX,
        targetPos.y + offsetY,
        width * scale,
        height * scale
    };
}

void Grenade::Draw() {
    if (!isActive) {
        return;
    }

    if (hasExploded && isExploding) {
        // Debug: dibujar hitbox de explosión
        Rectangle explosionBox = GetExplosionHitBox();
        DrawRectangleLinesEx(explosionBox, 2, YELLOW);
        if (!textureLoaded) return;

        float frameWidth = 52.0f;
        float frameHeight = 112.0f;
        float startRowY = 0.0f;

        Rectangle sourceRect = {
            explosionFrame * frameWidth,
            startRowY,
            frameWidth,
            frameHeight  // ← Alto completo, sin recortar
        };

        float scale = 2.0f;
        float offsetX = -40.0f;
        float offsetY = -frameHeight * scale;

        Rectangle destRect = {
            position.x + offsetX,
            position.y + offsetY,
            frameWidth * scale,
            frameHeight * scale
        };

        DrawTexturePro(texture, sourceRect, destRect, { 0, 0 }, 0, WHITE);
        return;
    }

    // Dibujar granada normal (sin explotar)
    if (!textureLoaded) return;

    float frameWidth = 22.0f;
    float frameHeight = 22.0f;
    float startRowY = 6.0f * 22.0f;

    Rectangle sourceRect = {
        currentFrame * frameWidth,
        startRowY,
        frameWidth,
        frameHeight
    };

    float scale = 3.0f;
    Rectangle destRect = {
        position.x - (frameWidth * scale) / 2 + 90.0f,
        position.y - (frameHeight * scale) / 2,
        frameWidth * scale,
        frameHeight * scale
    };

    DrawTexturePro(texture, sourceRect, destRect, { 0, 0 }, 0, WHITE);
}