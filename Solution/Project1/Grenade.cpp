#include "Grenade.hpp"
#include <cmath>
#include "raylib.h"

Grenade::Grenade(Vector2 startPos, Vector2 targetPos, float power) {
    this->startPos = startPos;
    this->targetPos = targetPos;
    this->position = startPos;
    LoadGrenadeTexture();
    CalculateTrajectory(power);
}

Grenade::~Grenade() { UnloadGrenadeTexture(); }

void Grenade::LoadGrenadeTexture() {
    texture = LoadTexture("Graphics/marcogrenade.png");
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);
}

void Grenade::UnloadGrenadeTexture() {
    UnloadTexture(texture);
}

void Grenade::CalculateTrajectory(float power) {
    float dx = targetPos.x - startPos.x;
    float dy = targetPos.y - startPos.y;

    // Aumentar el tiempo de vuelo (más tiempo = más lejos)
    float time = power / 500.0f;
    if (time < 0.2f) time = 0.2f;

    velocity.x = dx / time;
    velocity.y = (dy - 0.5f * gravity * time * time) / time;
}

void Grenade::Update() {
    if (!isActive) return;

    if (hasExploded) {
        explosionTimer += GetFrameTime();
        if (explosionTimer >= explosionDuration) {
            isActive = false;
        }
        return;
    }

    // Actualizar animación
    animationTimer += GetFrameTime();
    if (animationTimer >= frameDelay) {
        animationTimer = 0.0f;
        currentFrame++;
        if (currentFrame >= totalFrames) {
            currentFrame = 0;
        }
    }

    velocity.y += gravity * GetFrameTime();
    position.x += velocity.x * GetFrameTime();
    position.y += velocity.y * GetFrameTime();

    if (position.y >= targetPos.y || position.x >= targetPos.x + 100) {
        Explode();
    }
}

void Grenade::Explode() {
    hasExploded = true;
    explosionTimer = 0.0f;
}

Rectangle Grenade::GetHitBox() const {
    if (hasExploded) {
        return Rectangle{ position.x - explosionRadius, position.y - explosionRadius,
                          explosionRadius * 2, explosionRadius * 2 };
    }
    return Rectangle{ position.x - 11, position.y - 11, 22, 22 };
}

Rectangle Grenade::GetExplosionHitBox() const {
    return Rectangle{ position.x - explosionRadius, position.y - explosionRadius,
                      explosionRadius * 2, explosionRadius * 2 };
}

void Grenade::Draw() {
    if (!isActive) return;

    if (hasExploded) {
        float alpha = 1.0f - (explosionTimer / explosionDuration);
        DrawCircle(position.x, position.y, explosionRadius, ColorAlpha(RED, alpha));
        DrawCircleLines(position.x, position.y, explosionRadius, ColorAlpha(ORANGE, alpha));
    }
    else {
        // Dibujar granada con animación
         // Grid 22x22, fila 6
        float frameWidth = 22.0f;
        float frameHeight = 22.0f;
        float startRowY = 6.0f * 22.0f;  // Fila 6 * 22 = 132

        Rectangle sourceRect = {
            currentFrame * frameWidth,
            startRowY,
            frameWidth,
            frameHeight
        };

        float scale = 3.0f;  // Escala para que se vea bien
        Rectangle destRect = {
            position.x - (frameWidth * scale) / 2,
            position.y - (frameHeight * scale) / 2,
            frameWidth * scale,
            frameHeight * scale
        };

        DrawTexturePro(texture, sourceRect, destRect, { 0, 0 }, 0, WHITE);
    }
}