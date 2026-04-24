#pragma once
#include <raylib.h>
#include <algorithm>
#define _CRT_SECURE_NO_WARNINGS
#include "CreationManager.hpp"
#include "Player.hpp"
#include "CameraManager.hpp"
#include "SceneManager.hpp"
#include "UiManager.hpp"
class Debug
{
public:
	Debug();
	~Debug();
	void LoadBlocksFromFile(const char* filename);
	void MergeBlocks();
	void SaveBlocksToFile(const char* filename);
	bool GetEditorMode() { return editorMode; };
	void SetEditorMode();
	void EditorModeInput();
	void GeneralDebugInput();
private:
	FILE* file;
	CreationManager creationManager;
	Player player;
	CameraManager cameraManager;
	SceneManager sceneManager;
	UiManager uiManager;
	bool editorMode ;
	float gridSize = 50.0f;
	Vector2 gridOffset = { 0.0f, 0.0f };

};

