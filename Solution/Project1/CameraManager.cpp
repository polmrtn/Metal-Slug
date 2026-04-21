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

void CameraManager::Update(Vector2 playerPos, float bgWidth, float bgHeight, bool isGrounded) {
    // 1. Seguimiento horizontal clásico
    if (playerPos.x > maxScrollX) maxScrollX = playerPos.x;

    float halfScreenWidth = camera.offset.x;
    float halfScreenHeight = camera.offset.y;

    camera.target.x = Lerp(camera.target.x, maxScrollX, 0.1f);
    camera.target.x = Clamp(camera.target.x, halfScreenWidth, bgWidth - halfScreenWidth);

    // 2. Límites de seguridad (Clamps del fondo)
    float minY = halfScreenHeight + 100.0f;
    float maxY = bgHeight - halfScreenHeight - 165.0f;

    // Ajuste zona Boss (Esto ya lo tenías, lo mantenemos como base)
    if (playerPos.x > 14000.0f) {
        minY -= 400.0f;
        maxY -= 400.0f;
    }
    if (minY > maxY) maxY = minY;

    // 3. LÓGICA DE ZONAS (Bloqueo -> Libertad -> Re-bloqueo)

    // ZONA A: Antes de 13,000 (Comportamiento normal de suelo)
    if (playerPos.x < 12000.0f) {
        if (playerPos.y > 600) {
            float targetY = Clamp(playerPos.y, minY, maxY);
            if (abs(targetY - lockedYValue) > 50.0f) {
                lockedYValue = targetY;
            }
        }
    }
    // ZONA B: Entre 13,000 y 14,000 (Cámara libre en Y para seguir el salto/subida)
    else if (playerPos.x >= 12000.0f && playerPos.x < 14000.0f) {
        // Actualizamos lockedYValue constantemente para seguir al jugador suavemente
        lockedYValue = Clamp(playerPos.y, minY, maxY);
    }
    // ZONA C: A partir de 14,000 (Se vuelve a clavar en la altura actual)
    else if (playerPos.x >= 15000.0f) {
        static bool bossZoneLocked = false;
        if (!bossZoneLocked) {
            // En el momento que cruza los 14k, capturamos la posición actual como el nuevo bloqueo
            lockedYValue = Clamp(playerPos.y, minY, maxY);
            bossZoneLocked = true;
        }
        // Aquí lockedYValue ya no cambia, la cámara se queda fija en esta altura para el Boss
    }

    // 4. Aplicación suave
    camera.target.y = Lerp(camera.target.y, lockedYValue, 0.1f);
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
