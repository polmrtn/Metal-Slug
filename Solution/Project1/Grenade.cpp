#include "Grenade.hpp"
#include <cmath>
#include<vector>
#include "Soldier.hpp" 
#include "raylib.h"

// Definir las variables estáticas
Texture2D Grenade::texture = { 0 };
bool Grenade::textureLoaded = false;

Grenade::Grenade(Vector2 startPos, Vector2 initialVelocity) {
    this->position = startPos;
    this->velocity = initialVelocity;

    if (!textureLoaded) {
        texture = LoadTexture("Graphics/marcogrenade.png");
        SetTextureFilter(texture, TEXTURE_FILTER_POINT);
        textureLoaded = true;
    }
}

Grenade::~Grenade() {}

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
    explosionPosition = position;
    explosionFacingRight = (velocity.x >= 0);
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
    float width = 52.0f;
    float height = 112.0f;
    float scale = 2.0f;
    float offsetX = explosionFacingRight ? 10.0f : -65.0f;
    float offsetY = -height * scale + 10.0f;  // sube desde donde está la granada

    return Rectangle{
        explosionPosition.x + offsetX,
        explosionPosition.y + offsetY,
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
        /*Rectangle explosionBox = GetExplosionHitBox();
        DrawRectangleLinesEx(explosionBox, 2, YELLOW);*/
        if (!textureLoaded) return;

        float frameWidth = 52.0f;
        float frameHeight = 112.0f;
        float startRowY = 0.0f;

        Rectangle sourceRect = {
            explosionFrame * frameWidth,
            startRowY,
            frameWidth,
            frameHeight  
        };

        float scale = 2.0f;
        float offsetX = explosionFacingRight ? 10.0f : -65.0f;
        float offsetY = -frameHeight * scale + 20.0f;

        Rectangle destRect = {
            explosionPosition.x + offsetX,
            explosionPosition.y + offsetY,
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

void Grenade::CheckCollisionWithBlocks(const std::vector<Block>& blocks) {
    if (!isActive || hasExploded) return;

    Rectangle grenadeBox = GetHitBox();

    for (const auto& block : blocks) {
        // Ignorar rampas
        if (block.IsRamp()) continue;

        Rectangle blockRect = block.GetRect();
        if (!CheckCollisionRecs(grenadeBox, blockRect)) continue;

        // ===== TECHO: solo bloquea si la granada va hacia ARRIBA =====
        if (block.GetType() == BlockType::CEILING) {
            if (velocity.y < 0) {
                position.y = blockRect.y + blockRect.height + 11.0f;
                velocity.y = -velocity.y * bounceDamping;
            }
            return;
        }

        // ===== SUELO NORMAL: viene de arriba (cayendo) =====
        if (velocity.y > 0 &&
            position.y - velocity.y * GetFrameTime() < blockRect.y) {

            position.y = blockRect.y - 11.0f;

            if (!hasBounced) {
                velocity.y = -velocity.y * bounceDamping;
                velocity.x *= 0.7f;
                hasBounced = true;
            }
            else {
                Explode();
            }
            return;
        }

        // ===== LATERAL: explota =====
        Explode();
        return;
    }
}