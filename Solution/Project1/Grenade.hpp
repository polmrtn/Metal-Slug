#pragma once
#include<vector>
#include <raylib.h>
#include "Soldier.hpp" 
#include "LevelMap.hpp" 

class Grenade {
public:
    Grenade(Vector2 startPos, Vector2 initialVelocity);
    ~Grenade();

    void Update();
    void Draw();

    bool IsActive() const { return isActive; }
    Rectangle GetHitBox() const;
    Rectangle GetExplosionHitBox() const;
    bool HasExploded() const { return hasExploded; }
    void CheckCollisionWithSoldiers(std::vector<Soldier>& soldiers);
    void Explode();
    void StartExplosion();
    void CheckCollisionWithBlocks(const std::vector<Block>& blocks);
    bool HasPlayedSound() const { return soundPlayed; }
    void SetSoundPlayed(bool val) { soundPlayed = val; }

private:
    Vector2 position;
    Vector2 velocity;


    float gravity = 1000.0f;
    bool isActive = true;
    bool hasExploded = false;
    float explosionTimer = 0.0f;
    float explosionDuration = 0.3f;
    float explosionRadius = 50.0f;
    bool hasBounced = false;  // ← Si ya ha rebotado
    float bounceDamping = 0.3f;  // ← Pérdida de velocidad al rebotar (0.6 = 60% de velocidad)

    // Animación de la granada
    static Texture2D texture;  
    static bool textureLoaded;
    float animationTimer = 0.0f;
    int currentFrame = 0;
    int totalFrames = 16;
    float frameDelay = 0.05f;  // Velocidad de animación
    bool soundPlayed = false;

    // Animación de explosión
    float explosionAnimTimer = 0.0f;
    int explosionFrame = 0;
    int explosionTotalFrames = 27;
    float explosionFrameDelay = 0.03f;  // Velocidad de la explosión
    bool isExploding = false;
    Vector2 explosionPosition = { 0.0f, 0.0f };
    bool explosionFacingRight = true;

};