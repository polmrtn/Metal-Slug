#pragma once
#include "SoldierState.hpp"  // enum aquí
#include "SoldierAnim.hpp" 


class Soldier {
public:
    Soldier(int type, Vector2 position);
    Soldier(const Soldier& other);
    ~Soldier();

    void Update();
    void UpdateAI(class Player& player);
    void Draw();
    void DrawHitBox();

    Rectangle GetHitBox();
    int GetType();

    float GetWidth() { return 34.f * scale; }
    float GetHeight() { return 68.f * scale; }
    bool GetIsGrounded() { return isGrounded; }
    float GetY() const { return position.y; }
    float GetX() const { return position.x; }
    Vector2 GetPosition() const { return position; }
    bool GetisAlive() { return isAlive; }

    void SetY(float newY) { position.y = newY; }
    void SetVelocityY(float newVelY) { velocity.y = newVelY; }
    void SetVelocityX(float newVelX) { velocity.x = newVelX; }
    void SetGrounded(bool grounded) { isGrounded = grounded; }
    void SetisAlive(bool isalive) { isAlive = isalive; }
    float GetVelocityY() const { return velocity.y; }

private:
    //reference soldier animation
    SoldierAnim soldierAnim;
    Texture2D image;
    Vector2 position;
    Vector2 velocity;
    int type;

    float scale;
    float gravity;
    
    bool isGrounded;
    bool isAlive;

    // Variables de IA (cada soldado tiene las suyas)
    SoldierState currentState;
    float stateTimer;
};



