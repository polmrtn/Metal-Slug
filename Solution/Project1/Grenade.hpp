#pragma once
#include<vector>
#include <raylib.h>
#include "Soldier.hpp" 

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
    void CheckCollisionWithSoldiers(std::vector<Soldier>& soldiers);

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
    bool hasBounced = false;  // ← Si ya ha rebotado
    float bounceDamping = 0.5f;  // ← Pérdida de velocidad al rebotar (0.6 = 60% de velocidad)

    // Animación de la granada
    static Texture2D texture;  
    static bool textureLoaded;
    float animationTimer = 0.0f;
    int currentFrame = 0;
    int totalFrames = 16;
    float frameDelay = 0.05f;  // Velocidad de animación

    void CalculateTrajectory(float power);
    void Explode();

    // Animación de explosión
    float explosionAnimTimer = 0.0f;
    int explosionFrame = 0;
    int explosionTotalFrames = 27;
    float explosionFrameDelay = 0.03f;  // Velocidad de la explosión
    bool isExploding = false;

    void StartExplosion();
};