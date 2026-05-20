#include "CameraManager.hpp"

#include <raymath.h>
#include <algorithm>

CameraManager::CameraManager() {

}
CameraManager::CameraManager(Vector2 screenCenter) {
    camera.target = { 0, 0 };     
    camera.offset = screenCenter; 
    camera.rotation = 0.0f;
    camera.zoom = 1.00f;

    yLocked = false;              
    lockedYValue = 0.0f;
}

CameraManager::~CameraManager() {}

float CameraManager::GetLeftLimit() {
    return camera.target.x - camera.offset.x;
}

void CameraManager::Reset()
{
    camera.target = { 0, 0 };
    camera.offset = { 600.0f, 448.0f };
    maxScrollX = 0.0f;
    yLocked = false;
    lockedYValue = 0.0f;
    isFirstFrame = true;
    bossZoneLocked = false;
}

void CameraManager::Update(Vector2 playerPos, float bgWidth, float bgHeight, bool isGrounded) {
    float halfScreenWidth = camera.offset.x;
    float halfScreenHeight = camera.offset.y;

    // --- INICIALIZACIÓN INSTANTÁNEA (Evita el Lerp al empezar) ---
    if (isFirstFrame) {
        maxScrollX = playerPos.x;
        camera.target.x = Clamp(maxScrollX, halfScreenWidth, bgWidth - halfScreenWidth);

        // Calculamos un minY/maxY inicial para el primer frame
        float startMinY = halfScreenHeight + 100.0f;
        float startMaxY = bgHeight - halfScreenHeight - 165.0f;
        lockedYValue = Clamp(playerPos.y, startMinY, startMaxY);

        camera.target.y = lockedYValue; // Asignación directa, sin Lerp
        isFirstFrame = false;
        return;
    }
    // -------------------------------------------------------------

    // Boss zone parameters
    const float BOSS_ZONE_START = 14000.0f;
    const float BOSS_ZONE_SLACK = 300.0f; // allow camera to move this far into boss area before fully locking
    const float BOSS_ZONE_HARD_LOCK_X = BOSS_ZONE_START + BOSS_ZONE_SLACK;

    // 1. Seguimiento horizontal cl?sico con tope para zona del boss
    // - first clamp maxScrollX so the camera won't progress indefinitely into boss area
    float desiredScrollX = playerPos.x;
    if (playerPos.x >= BOSS_ZONE_START) {
        // limit how far maxScrollX can grow once player enters boss zone
        desiredScrollX = std::min(playerPos.x, BOSS_ZONE_HARD_LOCK_X);
    }
    if (desiredScrollX > maxScrollX) maxScrollX = desiredScrollX;

    camera.target.x = Lerp(camera.target.x, maxScrollX, 0.1f);

    // Second clamp: enforce camera target won't pass the boss hard limit (verifica dos veces)
    float cameraHardLimitX = Clamp(BOSS_ZONE_HARD_LOCK_X, halfScreenWidth, bgWidth - halfScreenWidth);
    camera.target.x = Clamp(camera.target.x, halfScreenWidth, bgWidth - halfScreenWidth);
    if (playerPos.x >= BOSS_ZONE_START) {
        // ensure we never show area beyond boss hard lock (additional safety)
        if (camera.target.x > cameraHardLimitX) camera.target.x = cameraHardLimitX;
    }

    // 2. Límites de seguridad (Clamps del fondo)
    float minY = halfScreenHeight + 100.0f;
    float maxY = bgHeight - halfScreenHeight - 165.0f;

    if (playerPos.x > BOSS_ZONE_START) {
        minY -= 400.0f;
        maxY -= 400.0f;
    }
    if (minY > maxY) maxY = minY;

    // 3. Lógica de zonas (Y behaviour)
    if (playerPos.x < BOSS_ZONE_START) {
        // ... (mantener lógica previa para tracking normal Y)
        if (playerPos.y > 600) {
            float targetY = Clamp(playerPos.y, minY, maxY);
            if (abs(targetY - lockedYValue) > 50.0f) {
                lockedYValue = targetY;
            }
        }
    }
    else if (playerPos.x >= BOSS_ZONE_START && playerPos.x < BOSS_ZONE_HARD_LOCK_X) {
        // libertad en la franja de slack: seguir al jugador sin el umbral de 50px
        lockedYValue = Clamp(playerPos.y, minY, maxY);
    }
    else { // playerPos.x >= BOSS_ZONE_HARD_LOCK_X
        // al entrar en el lock definitivo fijamos lockedYValue una sola vez
        if (!bossZoneLocked) {
            lockedYValue = Clamp(playerPos.y, minY, maxY);
            bossZoneLocked = true;
        }
    }

    // 4. Aplicaci?n suave (Solo si no es el primer frame)
    if (playerPos.y > 100) {
        camera.target.y = Lerp(camera.target.y, lockedYValue, 0.06f);
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