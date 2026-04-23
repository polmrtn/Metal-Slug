#include "CameraManager.hpp"

#include <raymath.h>

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
}

void CameraManager::Update(Vector2 playerPos, float bgWidth, float bgHeight, bool isGrounded) {
    float halfScreenWidth = camera.offset.x;
    float halfScreenHeight = camera.offset.y;

    // --- INICIALIZACIÓN INSTANTÁNEA (Evita el Lerp al empezar) ---
    static bool isFirstFrame = true;
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

    // 1. Seguimiento horizontal clásico
    if (playerPos.x > maxScrollX) maxScrollX = playerPos.x;

    camera.target.x = Lerp(camera.target.x, maxScrollX, 0.1f);
    camera.target.x = Clamp(camera.target.x, halfScreenWidth, bgWidth - halfScreenWidth);

    // 2. Límites de seguridad (Clamps del fondo)
    float minY = halfScreenHeight + 100.0f;
    float maxY = bgHeight - halfScreenHeight - 165.0f;

    if (playerPos.x > 14000.0f) {
        minY -= 400.0f;
        maxY -= 400.0f;
    }
    if (minY > maxY) maxY = minY;

    // 3. LÓGICA DE ZONAS (Tu código corregido)
    if (playerPos.x < 14000.0f) {
        // ... (Tu lógica de < 300 y > 600 se mantiene igual)
      
        if (playerPos.y > 600) {
            float targetY = Clamp(playerPos.y, minY, maxY);
            if (abs(targetY - lockedYValue) > 50.0f) {
                lockedYValue = targetY;
            }
        }
    }
    else if (playerPos.x >= 14000.0f && playerPos.x < 15000.0f) {
        // LIBERTAD: Aquí el clamp sigue al jugador sin umbral de 50px
        lockedYValue = Clamp(playerPos.y, minY, maxY);
    }
    else if (playerPos.x >= 14000.0f) {
        static bool bossZoneLocked = false;
        if (!bossZoneLocked) {
            lockedYValue = Clamp(playerPos.y, minY, maxY);
            bossZoneLocked = true;
        }
    }

    // 4. Aplicación suave (Solo si no es el primer frame)
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
