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
	backgroundManager.FollowPlayer({ camera.target.x, camera.target.y });
    float halfScreenWidth = camera.offset.x;

    camera.target.x = Lerp(camera.target.x, maxScrollX, 0.1f);
    camera.target.x = Clamp(camera.target.x, halfScreenWidth, bgWidth - halfScreenWidth);

    float halfScreenHeight = camera.offset.y;

    float minY = halfScreenHeight + 124.0f;
    float maxY = bgHeight - halfScreenHeight - 165.0f;

    if (minY > maxY) maxY = minY;

    if (!yLocked && isGrounded) {
        yLocked = true;
        lockedYValue = Clamp(playerPos.y, minY, maxY);
    }

    if (yLocked) {
        camera.target.y = Lerp(camera.target.y, lockedYValue, 0.1f);
    }
    else {
        camera.target.y = Clamp(playerPos.y, minY, maxY);
    }
}

void CameraManager::Begin() {
    BeginMode2D(camera);
}

void CameraManager::End() {
    EndMode2D();
}
