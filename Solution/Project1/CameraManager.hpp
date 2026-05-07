#pragma once
#include <raylib.h>
#include <vector>
#include <string>

class CameraManager {
public:
    CameraManager();
    CameraManager(Vector2 screenCenter);
    ~CameraManager();

    void Update(Vector2 playerPos, float bgWidth, float bgHeight, bool playerIsGrounded);
    void Begin();
    void End();
    void Reset();

    float   GetLeftLimit();
    Camera2D GetCamera() { return camera; }
    Vector2  GetScreenToWorld(Vector2 screenPos);

    // ?? Zone system ???????????????????????????????????????????????????????????
    void AddZone(const std::string& id, float triggerX, float minY, float maxY);
    void RemoveZone(const std::string& id);
    void ActivateZone(const std::string& id);
    void DeactivateZone(const std::string& id);

private:
    Camera2D camera;

    bool  yLocked = false;
    float lockedYValue = 0.0f;
    float maxScrollX = 0.0f;
    bool  isFirstFrame = true;
    bool  bossZoneLocked = false;

    float currentMinY = 0.f;
    float currentMaxY = 0.f;

    // ?? Camera zones ??????????????????????????????????????????????????????????
    struct CameraZone {
        std::string id;
        float triggerX;
        float minY;
        float maxY;
        bool  active;
    };
    std::vector<CameraZone> zones;
};