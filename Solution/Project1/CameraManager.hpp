#pragma once
#include <raylib.h>
#include <vector>
#include <string>

struct CameraZone {
    std::string id;
    float triggerX;
    float minY;
    float maxY;
    float minX;   // offset relative to triggerX
    float maxX;   // offset relative to triggerX
    bool  active;
    bool  clampX = true;
    bool  clampY = true;
};

class CameraManager {
public:
    CameraManager();
    CameraManager(Vector2 screenCenter);
    ~CameraManager();

    void Update(Vector2 playerPos, float bgWidth, float bgHeight, bool playerIsGrounded);
    void Begin();
    void End();
    void Reset();

    float    GetLeftLimit();
    Camera2D GetCamera() { return camera; }
    Vector2  GetScreenToWorld(Vector2 screenPos);

    void AddZone(const std::string& id, float triggerX,
        float minY, float maxY,
        float minX, float maxX,
        bool clampX, bool clampY);
    void RemoveZone(const std::string& id);
    void ActivateZone(const std::string& id);
    void DeactivateZone(const std::string& id);

    // Toggle clamp flags for an existing zone (used by debug/editor)
    void ToggleZoneClampX(const std::string& id);
    void ToggleZoneClampY(const std::string& id);

    const std::vector<CameraZone>& GetZones() const { return zones; }

private:
    Camera2D camera;
    bool  yLocked = false;
    float lockedYValue = 0.0f;
    float maxScrollX = 0.0f;
    bool  isFirstFrame = true;
    bool  bossZoneLocked = false;
    float currentMinY = 0.f;
    float currentMaxY = 0.f;
    std::vector<CameraZone> zones;
};