#pragma once
#include <raylib.h>

class Grenade {
public:
    Grenade(Vector2 startPos, Vector2 targetPos, float power);
    ~Grenade();

    void Update();
    void Draw();

    bool IsActive() const { return isActive; }
    Rectangle GetHitBox() const;
    Rectangle GetExplosionHitBox() const;
    bool HasExploded() const { return hasExploded; }

private:
    Vector2 position;
    Vector2 velocity;
    Vector2 startPos;
    Vector2 targetPos;

    float gravity = 600.0f;
    bool isActive = true;
    bool hasExploded = false;
    float explosionTimer = 0.0f;
    float explosionDuration = 0.3f;
    float explosionRadius = 50.0f;

    void CalculateTrajectory(float power);
    void Explode();
};