#include "CameraManager.hpp"
#include <raymath.h>
#include <algorithm>

CameraManager::CameraManager() {}

CameraManager::CameraManager(Vector2 screenCenter) {
    camera.target = { 0, 0 };
    camera.offset = screenCenter;
    camera.rotation = 0.0f;
    camera.zoom = 1.00f;
    yLocked = false;
    lockedYValue = 0.0f;
    isFirstFrame = true;
    bossZoneLocked = false;
}

CameraManager::~CameraManager() {}

float CameraManager::GetLeftLimit() {
    return camera.target.x - camera.offset.x;
}

Vector2 CameraManager::GetScreenToWorld(Vector2 screenPos) {
    return GetScreenToWorld2D(screenPos, camera);
}

void CameraManager::Reset() {
    camera.target = { 0, 0 };
    camera.offset = { 640.0f, 448.0f };
    maxScrollX = 0.0f;
    yLocked = false;
    lockedYValue = 0.0f;
    isFirstFrame = true;
    bossZoneLocked = false;
    currentMinY = 0.f;
    currentMaxY = 0.f;
    zones.clear();
}

void CameraManager::AddZone(const std::string& id, float triggerX,
    float minY, float maxY,
    float minX, float maxX,
    bool clampX, bool clampY)
{
    for (auto& z : zones) {
        if (z.id == id) {
            z = { id, triggerX, minY, maxY, minX, maxX, true, clampX, clampY };
            return;
        }
    }
    zones.push_back({ id, triggerX, minY, maxY, minX, maxX, true, clampX, clampY });
}

void CameraManager::RemoveZone(const std::string& id) {
    zones.erase(std::remove_if(zones.begin(), zones.end(),
        [&id](const CameraZone& z) { return z.id == id; }), zones.end());
}

void CameraManager::ActivateZone(const std::string& id) {
    for (auto& z : zones) {
        if (z.id == id) { z.active = true; return; }
    }
}

void CameraManager::DeactivateZone(const std::string& id) {
    for (auto& z : zones) {
        if (z.id == id) { z.active = false; return; }
    }
}

void CameraManager::ToggleZoneClampX(const std::string& id) {
    for (auto& z : zones) {
        if (z.id == id) { z.clampX = !z.clampX; return; }
    }
}

void CameraManager::ToggleZoneClampY(const std::string& id) {
    for (auto& z : zones) {
        if (z.id == id) { z.clampY = !z.clampY; return; }
    }
}

void CameraManager::Update(Vector2 playerPos, float bgWidth, float bgHeight, bool isGrounded) {
    float halfW = camera.offset.x;
    float halfH = camera.offset.y;

    if (isFirstFrame) {
        maxScrollX = playerPos.x;
        camera.target.x = Clamp(maxScrollX, halfW, bgWidth - halfW);
        float defMinY = halfH + 100.0f;
        float defMaxY = bgHeight - halfH - 165.0f;
        lockedYValue = Clamp(playerPos.y, defMinY, defMaxY);
        camera.target.y = lockedYValue;
        currentMinY = defMinY;
        currentMaxY = defMaxY;
        isFirstFrame = false;
        return;
    }

    // Update the "maxScrollX" (furthest right player has gone)
    if (playerPos.x > maxScrollX) maxScrollX = playerPos.x;

    // Defaults (world absolute limits)
    float minX = halfW;
    float maxX = bgWidth - halfW;
    float minY = halfH + 100.0f;
    float maxY = bgHeight - halfH - 165.0f;
    bool  doClampX = false; // default: don't force clamps unless zone requests it
    bool  doClampY = false;
    float bestTrigger = -1.f;
    const CameraZone* bestZone = nullptr;

    // Find the active zone that applies (largest triggerX <= player.x)
    for (const auto& zone : zones) {
        if (!zone.active) continue;
        if (playerPos.x >= zone.triggerX && zone.triggerX > bestTrigger) {
            bestTrigger = zone.triggerX;
            bestZone = &zone;
        }
    }

    if (bestZone) {
        // Use zone limits as absolute world coordinates.
        // The editor now creates zones storing absolute X range (minX/maxX),
        // and absolute Y limits (minY/maxY). When a zone is active we base
        // all clamping decisions solely on that zone (no fallback to defaults).
        minY = bestZone->minY;
        maxY = bestZone->maxY;
        minX = bestZone->minX;
        maxX = bestZone->maxX;
        doClampX = bestZone->clampX;
        doClampY = bestZone->clampY;
    } else {
        // No zone applies: fall back to default clamping behaviour
        doClampX = true;
        doClampY = true;
    }

    // Sanity: ensure ranges are valid
    if (minY > maxY) maxY = minY;
    if (minX > maxX) maxX = minX;

    // Horizontal: compute desired scroll target (based on maxScrollX)
    float desiredX = maxScrollX;

    if (bestZone) {
        // If the zone requests X clamping, lock desiredX to zone's X-limits.
        if (doClampX) {
            desiredX = Clamp(desiredX, minX, maxX);
            // Also clamp maxScrollX so future updates don't push outside the zone
            maxScrollX = Clamp(maxScrollX, minX, maxX);
        }
        // Smoothly follow desiredX but ensure final value stays within limits
        camera.target.x = Lerp(camera.target.x, desiredX, 0.12f);
        if (doClampX) camera.target.x = Clamp(camera.target.x, minX, maxX);
    } else {
        // No zone -> standard behavior
        camera.target.x = Lerp(camera.target.x, desiredX, 0.1f);
        camera.target.x = Clamp(camera.target.x, halfW, bgWidth - halfW);
    }

    // Vertical: if zone active and requests clamp, clamp strictly to zone limits
    if (bestZone && doClampY) {
        // Immediate clamping to prevent camera moving outside during jumps.
        camera.target.y = Clamp(playerPos.y, minY, maxY);
    } else {
        // Standard soft follow with defaults
        float defMinY = halfH + 100.0f;
        float defMaxY = bgHeight - halfH - 165.0f;
        float targetY = Clamp(playerPos.y, defMinY, defMaxY);
        camera.target.y = Lerp(camera.target.y, targetY, 0.06f);
    }
}

void CameraManager::Begin() { BeginMode2D(camera); }
void CameraManager::End() { EndMode2D(); }