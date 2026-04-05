#include "CameraManager.hpp"
#include <raymath.h>

CameraManager::CameraManager(Vector2 screenCenter) {
    camera = { 0 };
    camera.target = { 0, 0 };      // Dónde mira la cámara (el jugador)
    camera.offset = screenCenter;  // El centro de la pantalla (ej: 640, 448)
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;            // 1.0f es tamaño normal
}

CameraManager::~CameraManager() {}

void CameraManager::Update(Vector2 playerPos) {
   
    camera.target.x = playerPos.x;
    camera.target.y = 448.0f;
      float lerpFactor = 0.5f;
      camera.target.x = Lerp(camera.target.x, playerPos.x, lerpFactor); 
}
void CameraManager::Begin(){
    BeginMode2D(camera);
}

void CameraManager::End() {
    EndMode2D();
}