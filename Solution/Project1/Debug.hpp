#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <raylib.h>
#include <algorithm>
#include "GlobalManagers.hpp"

class Debug {
public:
    Debug();
    ~Debug();
    void SetEditorMode(Camera2D cam);
    void EditorModeInput(Camera2D cam);
    void GeneralDebugInput();
    void EnableHitboxes();
    void SaveToFile(const char* filename) const;
    void DrawEditorGrid(Camera2D cam);
    bool GetEditorMode() const { return editorMode; }
    void SetGridOffset(Vector2 offset) { gridOffset = offset; }

private:
    bool      editorMode = false;
    float     gridSize = TILE_SIZE;
    TileType  activeTileType = TileType::SOLID;
    Vector2   gridOffset = { 0.0f, 0.0f };

    // Para crear zonas desde el editor
    int zoneCount = 0;

    // Pending zone defaults: medio-rango X = 100, medio-rango Y = 100
    // X ajusta en pasos de 100, Y en pasos de 50
    float pendingZoneMinX = -100.f;
    float pendingZoneMaxX =  100.f;
    float pendingZoneMinY = -100.f;
    float pendingZoneMaxY =  100.f;
    bool  pendingClampX = true;
    bool  pendingClampY = true;

    // Flags to show if ranges were modified/added interactively
    bool pendingRangeActiveX = false;
    bool pendingRangeActiveY = false;

    // Step sizes
    static constexpr float PENDING_STEP_X = 100.0f;
    static constexpr float PENDING_STEP_Y = 50.0f;
};