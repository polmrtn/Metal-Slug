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

    // Boss zone parameters (hard limit en 15000, allowance atrás 100px)
    const float BOSS_ZONE_HARD_LIMIT = 15000.0f;
    const float BACK_ALLOWANCE = 100.0f; // permite mover la cámara 100px hacia atrás
    const float LEFT_ALLOW = BOSS_ZONE_HARD_LIMIT - BACK_ALLOWANCE;
    const float SLACK_END = BOSS_ZONE_HARD_LIMIT + 1000.0f; // franja intermedia para Y si es necesario

    // 1. Seguimiento horizontal con limitación doble (verifica dos veces)
    if (playerPos.x >= LEFT_ALLOW) {
        if (playerPos.x > maxScrollX) {
            // Si avanza, no permitimos superar el hard limit
            maxScrollX = std::min(playerPos.x, BOSS_ZONE_HARD_LIMIT);
        }
        else {
            // Permitir retroceso limitado dentro del allowance
            if (playerPos.x >= LEFT_ALLOW && playerPos.x < maxScrollX) {
                maxScrollX = playerPos.x;
            }
            // si retrocede más allá del allowance, no reducimos maxScrollX
        }
    }
    else {
        // Comportamiento normal fuera de la zona del boss: solo crecer
        if (playerPos.x > maxScrollX) maxScrollX = playerPos.x;
    }

    camera.target.x = Lerp(camera.target.x, maxScrollX, 0.1f);

    // Primera verificación/clamp: límites generales del fondo
    camera.target.x = Clamp(camera.target.x, halfScreenWidth, bgWidth - halfScreenWidth);

    // Segunda verificación: aplicar hard limit y permitir retroceso hasta LEFT_ALLOW
    if (playerPos.x >= LEFT_ALLOW) {
        if (camera.target.x > BOSS_ZONE_HARD_LIMIT) camera.target.x = BOSS_ZONE_HARD_LIMIT;
        if (camera.target.x < LEFT_ALLOW) camera.target.x = LEFT_ALLOW;
        camera.target.x = Clamp(camera.target.x, std::max(halfScreenWidth, LEFT_ALLOW), bgWidth - halfScreenWidth);
    }

    // 2. Límites de seguridad (Clamps del fondo) para Y
    float minY = halfScreenHeight + 100.0f;
    float maxY = bgHeight - halfScreenHeight - 165.0f;

    if (playerPos.x > BOSS_ZONE_HARD_LIMIT) {
        minY -= 400.0f;
        maxY -= 400.0f;
    }
    if (minY > maxY) maxY = minY;

    // 3. Lógica de zonas (comportamiento Y) usando el umbral en 14000
    if (playerPos.x < BOSS_ZONE_HARD_LIMIT) {
        if (playerPos.y > 600) {
            float targetY = Clamp(playerPos.y, minY, maxY);
            if (abs(targetY - lockedYValue) > 50.0f) {
                lockedYValue = targetY;
            }
        }
    }
    else if (playerPos.x >= BOSS_ZONE_HARD_LIMIT && playerPos.x < SLACK_END) {
        // Libertad vertical en la franja intermedia
        lockedYValue = Clamp(playerPos.y, minY, maxY);
    }
    else {
        if (!bossZoneLocked) {
            lockedYValue = Clamp(playerPos.y, minY, maxY);
            bossZoneLocked = true;
        }
    }

    // 4. Aplicación suave vertical con transición más lenta al subir
    // En raylib Y aumenta hacia abajo, por lo que "subir" corresponde a lockedYValue < camera.target.y
    const float LERP_UP = 0.02f;   // más lento al subir
    const float LERP_DOWN = 0.06f; // más rápido al bajar
    float lerpFactor = (lockedYValue < camera.target.y) ? LERP_UP : LERP_DOWN;

    if (playerPos.y > 100) {
        camera.target.y = Lerp(camera.target.y, lockedYValue, lerpFactor);
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
