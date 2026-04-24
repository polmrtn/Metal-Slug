#include "Debug.hpp"

Debug::Debug()
{
	editorMode = false;
	file = fopen("level_creationManager.GetBlocks().txt", "r");
	if (file) {
		fclose(file);
		LoadBlocksFromFile("level_creationManager.GetBlocks().txt");
	}
}
void Debug::LoadBlocksFromFile(const char* filename) {
creationManager.GetBlocks().clear();
creationManager.GetSoldiers().clear();
creationManager.GetItems().clear();

	FILE* file;
	fopen_s(&file, filename, "r");
	if (!file) {
		TraceLog(LOG_WARNING, "No se pudo cargar el archivo %s", filename);
		return;
	}

	char line[256];
	while (fgets(line, sizeof(line), file)) {
		if (line[0] == 'B') {
			float x, y, w, h;
			int typeValue;
			if (sscanf_s(line + 2, "%f,%f,%f,%f,%d", &x, &y, &w, &h, &typeValue) == 5) {
				if (typeValue == 2) creationManager.GetBlocks().emplace_back(x, y, w, h, BlockType::RAMP_UP);
				else if (typeValue == 3) creationManager.GetBlocks().emplace_back(x, y, w, h, BlockType::RAMP_DOWN);
				else if (typeValue == 4) creationManager.GetBlocks().emplace_back(x, y, w, h, BlockType::CEILING);
				else creationManager.GetBlocks().emplace_back(x, y, w, h, typeValue == 1);
			}
		}
		else if (line[0] == 'S') {
			float x, y;
			int type;
			if (sscanf_s(line + 2, "%f,%f,%d", &x, &y, &type) == 3) {
				creationManager.GetSoldiers().emplace_back(type, Vector2{ x, y });
			}
		}
		else if (line[0] == 'I') {
			float x, y;
			int type;
			if (sscanf_s(line + 2, "%f,%f,%d", &x, &y, &type) == 3) {
				creationManager.GetItems().emplace_back(Vector2{ x, y },
					type == 1 ? ItemType::BOX : ItemType::SHOTGUN);
			}
		}
		else {
			float x, y, w, h;
			int typeValue;
			if (sscanf_s(line, "%f,%f,%f,%f,%d", &x, &y, &w, &h, &typeValue) == 5) {
				if (typeValue == 2) creationManager.GetBlocks().emplace_back(x, y, w, h, BlockType::RAMP_UP);
				else if (typeValue == 3) creationManager.GetBlocks().emplace_back(x, y, w, h, BlockType::RAMP_DOWN);
				else if (typeValue == 4) creationManager.GetBlocks().emplace_back(x, y, w, h, BlockType::CEILING);
				else creationManager.GetBlocks().emplace_back(x, y, w, h, typeValue == 1);
			}
		}
	}

	fclose(file);
	MergeBlocks();
	TraceLog(LOG_INFO, "Nivel cargado: %d bloques, %d soldados, %d creationManager.GetItems()",
		(int)creationManager.GetBlocks().size(), (int)creationManager.GetSoldiers().size(), (int)creationManager.GetItems().size());
}
void Debug::SaveBlocksToFile(const char* filename) {
	FILE* file;
	fopen_s(&file, filename, "w");
	if (!file) return;

	for (const auto& block : creationManager.GetBlocks()) {
		Rectangle rect = block.GetRect();
		int typeValue = 0;
		if (block.GetType() == BlockType::RAMP_UP) typeValue = 2;
		else if (block.GetType() == BlockType::RAMP_DOWN) typeValue = 3;
		else if (block.GetType() == BlockType::CEILING) typeValue = 4;
		else typeValue = block.IsGround() ? 1 : 0;
		fprintf(file, "B %.0f,%.0f,%.0f,%.0f,%d\n",
			rect.x, rect.y, rect.width, rect.height, typeValue);
	}

	for (const auto& soldier : creationManager.GetSoldiers()) {
		fprintf(file, "S %.0f,%.0f,%d\n",
			soldier.GetX(), soldier.GetY(), const_cast<Soldier&>(soldier).GetType());
	}

	for (const auto& item : creationManager.GetItems()) {
		int itemType = (item.GetType() == ItemType::BOX) ? 1 : 0;
		fprintf(file, "I %.0f,%.0f,%d\n",
			item.GetPosition().x, item.GetPosition().y, itemType);
	}

	fclose(file);
}
void Debug::SetEditorMode()
{
	if (editorMode) {
			for (float x = fmod(gridOffset.x, gridSize); x < (float)GetScreenWidth(); x += gridSize) {
				DrawLineV({ x, 0 }, { x, (float)GetScreenHeight() }, GRAY);
			}
			for (float y = fmod(gridOffset.y, gridSize); y < (float)GetScreenHeight(); y += gridSize) {
				DrawLineV({ 0, y }, { (float)GetScreenWidth(), y }, GRAY);
			}

			DrawText("EDITOR MODE - F1:Salir | Click:Suelo | Right:Plataforma | Mid:Borrar | R:RampaUP | T:RampaDOWN | Y:Techo | 1:Soldado1 | 2:Soldado2 | B:Caja | G:MachinegunItem | F5:Guardar",
				10, 10, 12, RED);

			Vector2 pPos = player.GetPosition();
			DrawText(TextFormat("Player: (%.0f, %.0f)", pPos.x, pPos.y),
				10, 50, 15, GREEN);

			Vector2 mousePos = GetMousePosition();
			Vector2 worldPos = cameraManager.GetScreenToWorld(mousePos);
			DrawText(TextFormat("World: (%.0f, %.0f)", worldPos.x, worldPos.y), 10, 30, 15, YELLOW);
		}
}
void Debug::EditorModeInput()
{
	static float f1Cooldown = 0.0f;
	if (IsKeyPressed(KEY_F1) && f1Cooldown <= 0.0f) {
		editorMode = !editorMode;
		f1Cooldown = 0.2f;
		TraceLog(LOG_INFO, "Editor mode: %s", editorMode ? "ON" : "OFF");
	}
	if (f1Cooldown > 0.0f) f1Cooldown -= GetFrameTime();

	if (editorMode) {
		if (IsKeyDown(KEY_W)) gridOffset.y -= 5.0f;
		if (IsKeyDown(KEY_S)) gridOffset.y += 5.0f;
		if (IsKeyDown(KEY_A)) gridOffset.x -= 5.0f;
		if (IsKeyDown(KEY_D)) gridOffset.x += 5.0f;

		Vector2 mousePos = GetMousePosition();
		Vector2 worldPos = cameraManager.GetScreenToWorld(mousePos);

		int tileX = (int)floor((worldPos.x - gridOffset.x) / gridSize);
		int tileY = (int)floor((worldPos.y - gridOffset.y) / gridSize);

		float blockX = gridOffset.x + tileX * gridSize;
		float blockY = gridOffset.y + tileY * gridSize;

		if (IsKeyPressed(KEY_Y)) {
			creationManager.GetBlocks().emplace_back(blockX, blockY, gridSize, gridSize, BlockType::CEILING);
			TraceLog(LOG_INFO, "Techo creado en (%.0f, %.0f)", blockX, blockY);
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			creationManager.GetBlocks().emplace_back(blockX, blockY, gridSize, gridSize, true);
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
			creationManager.GetBlocks().emplace_back(blockX, blockY, gridSize, gridSize, false);
		}
		if (IsKeyPressed(KEY_R)) {
			creationManager.GetBlocks().emplace_back(blockX, blockY, gridSize, gridSize, BlockType::RAMP_UP);
			TraceLog(LOG_INFO, "Rampa UP creada en (%.0f, %.0f)", blockX, blockY);
		}
		if (IsKeyPressed(KEY_T)) {
			creationManager.GetBlocks().emplace_back(blockX, blockY, gridSize, gridSize, BlockType::RAMP_DOWN);
			TraceLog(LOG_INFO, "Rampa DOWN creada en (%.0f, %.0f)", blockX, blockY);
		}
		static float soldierSpawnCooldown = 0.0f;
		if (soldierSpawnCooldown > 0.0f) soldierSpawnCooldown -= GetFrameTime();

		if (IsKeyPressed(KEY_ONE) && soldierSpawnCooldown <= 0.0f) {
			creationManager.GetSoldiers().emplace_back(1, worldPos);
			soldierSpawnCooldown = 0.3f;
			TraceLog(LOG_INFO, "Soldado tipo 1 en (%.0f, %.0f)", worldPos.x, worldPos.y);
		}
		else if (IsKeyPressed(KEY_TWO) && soldierSpawnCooldown <= 0.0f) {
			creationManager.GetSoldiers().emplace_back(2, worldPos);
			soldierSpawnCooldown = 0.3f;
			TraceLog(LOG_INFO, "Soldado tipo 2 en (%.0f, %.0f)", worldPos.x, worldPos.y);
		}
		static float itemSpawnCooldown = 0.0f;
		if (itemSpawnCooldown > 0.0f) itemSpawnCooldown -= GetFrameTime();

		if (IsKeyPressed(KEY_B) && itemSpawnCooldown <= 0.0f) {
			creationManager.GetItems().emplace_back(worldPos, ItemType::BOX);
			itemSpawnCooldown = 0.3f;
			TraceLog(LOG_INFO, "Caja en (%.0f, %.0f)", worldPos.x, worldPos.y);
		}
		if (IsKeyPressed(KEY_G) && itemSpawnCooldown <= 0.0f) {
			creationManager.GetItems().emplace_back(worldPos, ItemType::SHOTGUN);
			itemSpawnCooldown = 0.3f;
			TraceLog(LOG_INFO, "Machinegun item en (%.0f, %.0f)", worldPos.x, worldPos.y);
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
			float bX = blockX;
			float bY = blockY;
			auto it = std::remove_if(creationManager.GetBlocks().begin(), creationManager.GetBlocks().end(),
				[bX, bY](const Block& b) {
					return b.GetRect().x == bX && b.GetRect().y == bY;
				});
			creationManager.GetBlocks().erase(it, creationManager.GetBlocks().end());

			auto iIt = std::remove_if(creationManager.GetItems().begin(), creationManager.GetItems().end(),
				[&worldPos](const Item& i) {
					return CheckCollisionPointRec(worldPos, i.GetHitBox());
				});
			creationManager.GetItems().erase(iIt, creationManager.GetItems().end());
		}
		if (IsKeyPressed(KEY_F5)) {
			SaveBlocksToFile("level_creationManager.GetBlocks().txt");
		}
	}
}
void Debug::GeneralDebugInput()
{
	if (IsKeyPressed(KEY_L)) uiManager.NextLevel();
	if (IsKeyPressed(KEY_J)) uiManager.AddScore(100);
	if (IsKeyPressed(KEY_C) && creditCooldown <= 0.0f) {
		if (uiManager.GetCredits() < 99) {
			uiManager.SetCredits(1);
			creditCooldown = creditDelay;
		}
	}
}
void Debug::MergeBlocks() {
	if (creationManager.GetBlocks().empty()) return;

	std::vector<Block> merged;
	const float EPS = 0.5f;  // tolerancia para comparar floats

	std::sort(creationManager.GetBlocks().begin(), creationManager.GetBlocks().end(), [](const Block& a, const Block& b) {
		if (fabsf(a.GetRect().y - b.GetRect().y) > 0.5f) return a.GetRect().y < b.GetRect().y;
		return a.GetRect().x < b.GetRect().x;
		});

	size_t i = 0;
	while (i < creationManager.GetBlocks().size()) {
		Rectangle r = creationManager.GetBlocks()[i].GetRect();
		BlockType t = creationManager.GetBlocks()[i].GetType();
		bool g = creationManager.GetBlocks()[i].IsGround();

		if (t != BlockType::NORMAL) {
			merged.push_back(creationManager.GetBlocks()[i]);
			i++;
			continue;
		}

		size_t j = i + 1;
		while (j < creationManager.GetBlocks().size()) {
			Rectangle rj = creationManager.GetBlocks()[j].GetRect();
			bool sameRow = fabsf(rj.y - r.y) < EPS && fabsf(rj.height - r.height) < EPS;
			bool adjacent = fabsf(rj.x - (r.x + r.width)) < EPS;
			bool sameType = creationManager.GetBlocks()[j].GetType() == t && creationManager.GetBlocks()[j].IsGround() == g;
			if (sameRow && adjacent && sameType) {
				r.width += rj.width;
				j++;
			}
			else break;
		}

		if (g) merged.emplace_back(r.x, r.y, r.width, r.height, true);
		else   merged.emplace_back(r.x, r.y, r.width, r.height, false);
		i = j;
	}

	creationManager.GetBlocks() = merged;
	TraceLog(LOG_INFO, "MergeBlocks: %d bloques tras fusionar", (int)creationManager.GetBlocks().size());
}
Debug::~Debug()
{
}
