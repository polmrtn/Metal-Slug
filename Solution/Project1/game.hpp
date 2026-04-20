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
    void ThrowGrenade();

    // Colisiones
    void ResolveCollisions();
    void BlockCollisions();
    void BulletsCollision();
    void GrenadesCollision();

    // Guardar/Cargar nivel
    void SaveBlocksToFile(const char* filename);
    void LoadBlocksFromFile(const char* filename);

private:
    // ========== TEMPORIZADORES ==========
    float shootTimer = 0.0f;
    float grenadeCooldown = 0.0f;

    // Delays de armas
    float shootDelayPistol = 0.2f;
    float shootDelayMachinegun = 0.1f;
    float grenadeDelay = 1.0f;

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
    std::vector<Bullet> bullets;
    std::vector<Soldier> soldiers;
    std::vector<Grenade> grenades;
    std::vector<Item> items;
    std::vector<Block> blocks;

    // ========== ANIMACIONES ==========
    SoldierAnim soldierAnim;

    // ========== M�TODOS DE CREACI�N ==========
    std::vector<Bullet> CreateBullets();
    std::vector<Soldier> CreateSoldiers();
    std::vector<Block> CreateBlocks();
    std::vector<Item> CreateItems();

    // ========== MODO EDITOR ==========
    bool editorMode = false;
    float gridSize = 50.0f;
    Vector2 gridOffset = { 0.0f, 0.0f };
};