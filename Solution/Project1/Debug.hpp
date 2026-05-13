#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <raylib.h>
#include <algorithm>
#include "GlobalManagers.hpp"

class Debug
{
public:
    Debug();
    ~Debug();

    void SetEditorMode(Camera2D cam);       // dibuja el editor (llamar dentro de camera.Begin())
    void EditorModeInput(Camera2D cam);     // procesa input del editor (llamar cada frame)
    void GeneralDebugInput();   // cheats (L=nivel, J=score, C=crédito)
    void EnableHitboxes();
    void SaveToFile(const char* filename) const;
    void DrawEditorGrid(Camera2D cam);
    bool GetEditorMode() const { return editorMode; }
    void SetGridOffset(Vector2 offset) { gridOffset = offset; }

private:
    bool      editorMode = false;
    float     gridSize = TILE_SIZE;   // sincronizado con el tamaño de tile
    TileType  activeTileType = TileType::SOLID;
    Vector2 gridOffset = { 0.0f, 0.0f };
};
