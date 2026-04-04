#pragma once
#include <raylib.h>

class CameraManager {
public:
    CameraManager(Vector2 screenCenter);
    ~CameraManager();

    // Actualiza la posición de la cámara siguiendo al jugador
    void Update(Vector2 playerPos);

    // Métodos para entrar y salir del modo 2D en el Game::Draw()
    void Begin();
    void End();

    // Getters por si necesitas saber dónde está la cámara
    Camera2D GetCamera() { return camera; }

private:
    Camera2D camera;
};