#pragma once
#include <raylib.h>
#include "BackgroundManager.hpp"

class CameraManager {
public:
    CameraManager(Vector2 screenCenter);
    ~CameraManager();

    float GetLeftLimit();
    void Update(Vector2 playerPos, float bgWidth, float bgHeight, bool playerIsGrounded);
    void Begin();
    void End();
    Camera2D GetCamera() { return camera; }
    Vector2 GetScreenToWorld(Vector2 screenPos);
private:
	BackgroundManager backgroundManager;
    Camera2D camera;
    bool yLocked = false;       
    float lockedYValue = 0.0f;
    float maxScrollX = 0.0f;
};