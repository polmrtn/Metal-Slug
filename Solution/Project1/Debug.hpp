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

    void SetEditorMode();       // dibuja el editor (llamar dentro de camera.Begin())
    void EditorModeInput();     // procesa input del editor (llamar cada frame)
    void GeneralDebugInput();   // cheats (L=nivel, J=score, C=crédito)
    void EnableHitboxes();
    void SaveToFile(const char* filename) const;

    bool GetEditorMode() const { return editorMode; }

private:
    bool      editorMode = false;
    float     gridSize = TILE_SIZE;   // sincronizado con el tamaño de tile
    TileType  activeTileType = TileType::SOLID;
};
