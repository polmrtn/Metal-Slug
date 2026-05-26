#include "CameraManager.hpp"
#include <raymath.h>

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

    if (isFirstFrame) {
        maxScrollX = playerPos.x;
        camera.target.x = Clamp(maxScrollX, halfScreenWidth, bgWidth - halfScreenWidth);
        float startMinY = halfScreenHeight + 100.0f;
        float startMaxY = bgHeight - halfScreenHeight - 165.0f;
        lockedYValue = Clamp(playerPos.y, startMinY, startMaxY);
        camera.target.y = lockedYValue;
        isFirstFrame = false;
        return;
    }

    // ?? Seguimiento horizontal ????????????????????????????????
    // Detectar entrada en zona boss
    if (playerPos.x >= 15520.0f && !transitionZone) {
        transitionZone = true;
        transitionTimer = 0.0f;
    }

    if (!transitionZone) {
        if (playerPos.x > maxScrollX) maxScrollX = playerPos.x;
        camera.target.x = Lerp(camera.target.x, maxScrollX, 0.1f);
        camera.target.x = Clamp(camera.target.x, halfScreenWidth, bgWidth - halfScreenWidth);
    }
    else {
        transitionTimer += 0.016f;  // ~1 frame
        // Espera 0.5s antes de cambiar la lógica para que la cámara esté quieta
        float blend = Clamp(transitionTimer / 0.5f, 0.0f, 1.0f);
        float targetNormal = maxScrollX;
        float targetBoss = Clamp(playerPos.x, 15520.0f, bgWidth - halfScreenWidth);
        float targetX = targetNormal + (targetBoss - targetNormal) * blend;
        camera.target.x = Lerp(camera.target.x, targetX, 0.05f);
        camera.target.x = Clamp(camera.target.x, 15520.0f - (1.0f - blend) * 600.0f, bgWidth - halfScreenWidth);
    }

    // ?? Límites verticales ????????????????????????????????????
    float minY = halfScreenHeight + 100.0f;
    float maxY = bgHeight - halfScreenHeight - 165.0f;
    if (playerPos.x > 14000.0f) {
        minY -= 400.0f;
        maxY -= 400.0f;
    }
    if (minY > maxY) maxY = minY;

    // ?? Lógica de zonas Y ?????????????????????????????????????
    if (playerPos.x < 14000.0f) {
        if (playerPos.y > 600) {
            float targetY = Clamp(playerPos.y, minY, maxY);
            if (abs(targetY - lockedYValue) > 50.0f) {
                lockedYValue = targetY;
            }
        }
    }
    else if (playerPos.x >= 14000.0f && playerPos.x < 15000.0f) {
        lockedYValue = Clamp(playerPos.y, minY, maxY);
    }
    else if (playerPos.x >= 15000.0f) {
        if (!bossZoneLocked) {
            lockedYValue = Clamp(playerPos.y, minY, maxY);
            bossZoneLocked = true;
        }
    }

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