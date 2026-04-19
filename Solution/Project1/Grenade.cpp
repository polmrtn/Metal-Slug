#include "Grenade.hpp"
#include <cmath>

Grenade::Grenade(Vector2 startPos, Vector2 targetPos, float power) {
    this->startPos = startPos;
    this->targetPos = targetPos;
    this->position = startPos;
    CalculateTrajectory(power);
}

Grenade::~Grenade() {}

void Grenade::CalculateTrajectory(float power) {
    float dx = targetPos.x - startPos.x;
    float dy = targetPos.y - startPos.y;

    // Aumentar el tiempo de vuelo (más tiempo = más lejos)
    float time = power / 500.0f;
    if (time < 0.8f) time = 0.8f;

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
    return Rectangle{ position.x - 5, position.y - 5, 10, 10 };
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
        DrawCircle(position.x, position.y, 5, DARKGREEN);
        DrawCircle(position.x, position.y, 3, GREEN);
    }
}