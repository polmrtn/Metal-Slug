#pragma once
#include "SoldierState.hpp"  // enum aquí
#include "SoldierAnim.hpp" 
#include <raylib.h>


class Soldier {
public:
    Soldier(int type, Vector2 position);
    Soldier(const Soldier& other);
    ~Soldier();

    void Update();
    void UpdateAI(class Player& player);
    void Draw();
    void DrawHitBox();
    void Attack(Player& player);
    Rectangle GetHurtBox();
    bool IsVisionRay(Player& player);
    Rectangle GetHitBox();
    void SetSoliderState(SoldierState newState);
    int GetType();
    bool GetWantsToShoot() { return wantsToShoot; }
    void ResetShootFlag() { wantsToShoot = false; }


    float GetWidth() { return 34.f * scale; }
    float GetHeight() { return 68.f * scale; }
    bool GetIsGrounded() { return isGrounded; }
    bool IsDeadAnimFinished() const { return soldierAnim.IsAnimationFinished(); }
    float GetY() const { return position.y; }
    float GetX() const { return position.x; }
    Vector2 GetPosition() const { return position; }
    bool GetisAlive() { return isAlive; }
    void TriggerDeath();
   
    void SetY(float newY) { position.y = newY; }
    void SetVelocityY(float newVelY) { velocity.y = newVelY; }
    void SetVelocityX(float newVelX) { velocity.x = newVelX; }
    void SetGrounded(bool grounded) { isGrounded = grounded; }
    void SetisAlive(bool isalive) { isAlive = isalive; }
    float GetVelocityY() const { return velocity.y; }
    bool WantsToShoot() const { return wantsToShoot; }
    void ResetShootWants() { wantsToShoot = false; }
    bool IsFacingRight() const { return facingRight; }
    Rectangle GetLeftHitBox();
    Rectangle GetRightHitBox();
    void SetLeftCollision(bool val) { leftCollision = val; }
    void SetRightCollision(bool val) { rightCollision = val; }
    bool GetLeftCollision() const { return leftCollision; }
    bool GetRightCollision() const { return rightCollision; }
private:
    //reference soldier animation
    SoldierAnim soldierAnim;
    Texture2D image;
    Vector2 position;
    Vector2 velocity;
    int type;
    bool wantsToShoot = false;
    float scale;
    float gravity;
    bool leftCollision = false;
    bool rightCollision = false;
    bool isGrounded;
    bool isAlive;
    bool facingRight ;
    bool hasShot = false;
    
    
    // Variables de IA (cada soldado tiene las suyas)
    SoldierState currentState;
    float stateTimer;
    float attackTimer = 0.0f;

    // Prevent retrigger while player stays in vision: only trigger once per entry
    bool attackTriggered = false;
};



