#include "CameraManager.hpp"

#include <raymath.h>

CameraManager::CameraManager(Vector2 screenCenter) {
    camera.target = { 0, 0 };     
    camera.offset = screenCenter; 
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    yLocked = false;              
    lockedYValue = 0.0f;
}

CameraManager::~CameraManager() {}

float CameraManager::GetLeftLimit() {
    return camera.target.x - camera.offset.x;
}

void CameraManager::Update(Vector2 playerPos, float bgWidth, float bgHeight, bool isGrounded) {
    if (playerPos.x > maxScrollX) {
        maxScrollX = playerPos.x;
    }

    backgroundManager.FollowPlayer(camera.target);
    float halfScreenWidth = camera.offset.x;
    float halfScreenHeight = camera.offset.y;

    camera.target.x = Lerp(camera.target.x, maxScrollX, 0.1f);
    camera.target.x = Clamp(camera.target.x, halfScreenWidth, bgWidth - halfScreenWidth);

    // ========== AJUSTE DINÁMICO DE LÍMITES EN Y ==========
    float minY = halfScreenHeight + 124.0f;
    float maxY = bgHeight - halfScreenHeight - 165.0f;

    
    // desplazamos los límites hacia arriba para permitir que la cámara suba
    if (playerPos.x > 14000.0f) {
        // Ajusta este valor (ej: -400.0f) según qué tan arriba esté tu jefe final
        float elevacionBoss = -400.0f;
        minY += elevacionBoss;
        maxY += elevacionBoss;
    }

    if (minY > maxY) maxY = minY;

    // ========== LÓGICA DE TRANSICIÓN ==========
    bool inTransitionZone = (playerPos.x >= 14000.0f && playerPos.x <= 15500.0f);

    if (inTransitionZone) {
        yLocked = false;
    }
    else if (playerPos.x > 15500.0f && !yLocked && isGrounded) {
        // Bloqueamos en la nueva zona alta
        yLocked = true;
        lockedYValue = Clamp(playerPos.y, minY, maxY);
    }
    else if (playerPos.x < 14000.0f && !yLocked && isGrounded) {
        yLocked = true;
        lockedYValue = Clamp(playerPos.y, minY, maxY);
    }

    // APLICACIÓN DE LA CÁMARA
    if (yLocked) {
        camera.target.y = Lerp(camera.target.y, lockedYValue, 0.1f);
    }
    else {
        // Durante la transición, usamos los nuevos límites permitidos
        camera.target.y = Lerp(camera.target.y, Clamp(playerPos.y, minY, maxY), 0.01f);
    }
}

void CameraManager::Begin() {
    BeginMode2D(camera);
}

void CameraManager::End() {
    EndMode2D();
}

Vector2 CameraManager::GetScreenToWorld(Vector2 screenPos) {
    return GetScreenToWorld2D(screenPos, camera);
}
