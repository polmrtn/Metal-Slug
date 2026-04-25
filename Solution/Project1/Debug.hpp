#pragma once
#include <raylib.h>
#include <algorithm>
#define _CRT_SECURE_NO_WARNINGS
#include "GlobalManagers.hpp"

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
	void EnableHitboxes();
private:
	FILE* file;
	bool editorMode ;
	float gridSize = 50.0f;
	Vector2 gridOffset = { 0.0f, 0.0f };

};

