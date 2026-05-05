#pragma once
#include "GlobalManagers.hpp"
#include "CameraManager.hpp"
#include "SystemCollision.hpp"
#include "Bullet.hpp"
#include "Grenade.hpp"
#include "InputManager.hpp"
#include "Debug.hpp"


class Game {
public:
    Game();
    ~Game();

    void Update();
    void Draw();
    void HandleInput();
    void Reset();

    void Shoot(int bulletType, Vector2 startPos, bool faceRight);
    void ShootMachinegun(float yOffset);
    void ThrowGrenade();
    void CheckBulletsOutOfCamera();
    void StartMachinegunBurst();
    bool IsMachinegunBurst() const { return machinegunBurst; }
    PlayerDirection GetMachinegunBurstDir() const { return machinegunBurstDir; }

    bool ShouldRestart() const { return shouldRestart; }
    SceneManager& GetSceneManager() { return sceneManager; }
    UiManager& GetUiManager() { return uiManager; }

private:
    // CameraManager local porque necesita el offset de pantalla en el constructor
    CameraManager camera;
    SystemCollision systemCollision;
    InputManager inputManager{this};
    Debug debug;

    // Estado de la ráfaga de machinegun
    bool machinegunBurst = false;
    static constexpr int MACHINEGUN_BURST_SIZE = 6;
    const float burstOffsets[MACHINEGUN_BURST_SIZE] = { -10.0f, 0.0f, 10.0f, -10.0f, 0.0f, 10.0f };
    int machinegunBurstCount = 0;
    PlayerDirection machinegunBurstDir = PlayerDirection::RIGHT;

    // Estado de sonido machinegun
    bool machinegunSoundActive = false;

    bool  shouldRestart = false;
    float gameOverTimer = 0.0f;
};