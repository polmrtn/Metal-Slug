#pragma once
#include "Player.hpp"
#include "Soldier.hpp"
#include "AudioManager.hpp"
#include "SceneManager.hpp"
#include "Bullet.hpp"
#include "BackgroundManager.hpp"
#include "CameraManager.hpp"
#include "LevelMap.hpp"
#include "UiManager.hpp"
#include "SoldierAnim.hpp"
#include "Grenade.hpp" 
#include "Item.hpp"
#include "BulletAnim.hpp"
#include <vector>

class Game {
public:
    Game();
    ~Game();
    // Ciclo principal
    void Update();
    void Draw();
    // Input
    void HandleInput();
    // Disparo y granadas
    void Shoot();
    void Shoot(int BulletType, Vector2 startPos, bool faceRight);
    void ThrowGrenade();
    // Colisiones
    void ResolveCollisions();
    void BlockCollisions();
    void BulletsCollision();
    void GrenadesCollision();
    void SoldierBlockCollision();
    // Guardar/Cargar nivel
    void SaveBlocksToFile(const char* filename);
    void LoadBlocksFromFile(const char* filename);
    void ShootMachinegun(float yOffset);
    void CheckBulletsOutOfCamera();
    void MergeBlocks();
    void Reset();
    bool ShouldRestart() const { return shouldRestart; }
    SceneManager& GetSceneManager() { return sceneManager; }
    UiManager& GetUiManager() { return UiManager; }

private:
    // ========== TEMPORIZADORES ==========
  
    // ========== GESTORES ==========
    CameraManager camera;
    BackgroundManager backgroundManager;
    AudioManager audioManager;
    SceneManager sceneManager;
    UiManager UiManager;
    BulletAnim bulletAnim;
    // ========== JUGADOR ==========
    Player player;
    // ========== ENTIDADES ==========
 
    // ========== ANIMACIONES ==========
    SoldierAnim soldierAnim;
    // ========== METODOS DE CREACION ==========
   
    // ========== MODO EDITOR ==========
   
    bool machinegunBurst = false;
    
   
    static constexpr int MACHINEGUN_BURST_SIZE = 6;
    const float burstOffsets[6] = { -10.0f, 0.0f, 10.0f, -10.0f, 0.0f, 10.0f };
    PlayerDirection machinegunBurstDir = PlayerDirection::RIGHT;
    bool shouldRestart = false;

    // ========== CRÉDITOS ==========
   // Delay entre inserciones de crédito (segundos)

    // ========== DZWIEK MACHINEGUN (osobny timer, 4 dzwieki co 0.25s) ==========
    bool machinegunSoundActive = false;
   
};