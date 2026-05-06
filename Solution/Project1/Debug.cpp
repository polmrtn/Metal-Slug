#define _CRT_SECURE_NO_WARNINGS
#include "Debug.hpp"

Debug::Debug() : editorMode(false) {}
Debug::~Debug() {}

void Debug::SetEditorMode(Camera2D cam)
{
    if (!editorMode) return;

    DrawText("EDITOR  F1:Salir | 1:Solid | 2:Platform | C:Ceiling | R:Ramp | Del:Borrar | Flechas:MoverGrid | F5:Guardar",
        8, 8, 11, RED);
    DrawText("ENTIDADES  S:Soldier1 | D:Soldier2 | B:Box | M:Machinegun | P:Plushy | K:Fish",
        8, 20, 11, GREEN);

    Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), cam);
    DrawText(TextFormat("World (%.0f, %.0f)  GridOffset (%.0f, %.0f)",
        worldPos.x, worldPos.y, gridOffset.x, gridOffset.y),
        8, 32, 12, YELLOW);

    const char* typeName = "";
    switch (activeTileType) {
    case TileType::SOLID:    typeName = "SOLID";    break;
    case TileType::PLATFORM: typeName = "PLATFORM"; break;
    case TileType::CEILING:  typeName = "CEILING";  break;
    case TileType::RAMP_UP:  typeName = "RAMP_UP";  break;
    }
    DrawText(TextFormat("Tipo activo: %s", typeName), 8, 48, 12, WHITE);
}

void Debug::EditorModeInput(Camera2D cam)
{
    static float f1Cooldown = 0.0f;
    if (f1Cooldown > 0.0f) f1Cooldown -= GetFrameTime();
    if (IsKeyPressed(KEY_F1) && f1Cooldown <= 0.0f) {
        editorMode = !editorMode;
        f1Cooldown = 0.2f;
        if (!editorMode) {
            creationManager.GetTileMap().Bake();
            TraceLog(LOG_INFO, "Editor cerrado — Bake ejecutado");
        }
    }

    if (!editorMode) return;

    // Selección de tipo de tile
    if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) activeTileType = TileType::SOLID;
    if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) activeTileType = TileType::PLATFORM;
    if (IsKeyPressed(KEY_C)) activeTileType = TileType::CEILING;
    if (IsKeyPressed(KEY_R)) activeTileType = TileType::RAMP_UP;

    // Mover offset del grid
    const float GRID_SPEED = 150.0f;
    if (IsKeyDown(KEY_UP))    gridOffset.y -= GRID_SPEED * GetFrameTime();
    if (IsKeyDown(KEY_DOWN))  gridOffset.y += GRID_SPEED * GetFrameTime();
    if (IsKeyDown(KEY_LEFT))  gridOffset.x -= GRID_SPEED * GetFrameTime();
    if (IsKeyDown(KEY_RIGHT)) gridOffset.x += GRID_SPEED * GetFrameTime();

    creationManager.GetTileMap().SetGridOffset(gridOffset);

    Vector2 mousePos = GetMousePosition();
    Vector2 worldPos = GetScreenToWorld2D(mousePos, cam);

    // Colocar / borrar tiles
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        creationManager.GetTileMap().AddTileWorld(worldPos.x, worldPos.y, activeTileType);
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        creationManager.GetTileMap().AddTileWorld(worldPos.x, worldPos.y, TileType::PLATFORM);
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) || IsKeyDown(KEY_DELETE))
    {
        creationManager.GetTileMap().RemoveTileWorld(worldPos.x, worldPos.y);
        auto& items = creationManager.GetItems();
        items.erase(std::remove_if(items.begin(), items.end(),
            [&worldPos](const Item& i) {
                return CheckCollisionPointRec(worldPos, i.GetHitBox());
            }), items.end());
    }

    // Spawn con cooldown compartido
    static float spawnCooldown = 0.0f;
    if (spawnCooldown > 0.0f) spawnCooldown -= GetFrameTime();

    // Soldados
    if (IsKeyPressed(KEY_S) && spawnCooldown <= 0.0f) {
        creationManager.GetSoldiers().emplace_back(1, worldPos);
        spawnCooldown = 0.3f;
        TraceLog(LOG_INFO, "Soldier 1 en (%.0f, %.0f)", worldPos.x, worldPos.y);
    }
    if (IsKeyPressed(KEY_D) && spawnCooldown <= 0.0f) {
        creationManager.GetSoldiers().emplace_back(2, worldPos);
        spawnCooldown = 0.3f;
        TraceLog(LOG_INFO, "Soldier 2 en (%.0f, %.0f)", worldPos.x, worldPos.y);
    }

    // Items
    if (IsKeyPressed(KEY_B) && spawnCooldown <= 0.0f) {
        creationManager.GetItems().emplace_back(worldPos, ItemType::BOX);
        spawnCooldown = 0.3f;
        TraceLog(LOG_INFO, "Box en (%.0f, %.0f)", worldPos.x, worldPos.y);
    }
    if (IsKeyPressed(KEY_M) && spawnCooldown <= 0.0f) {
        creationManager.GetItems().emplace_back(worldPos, ItemType::SHOTGUN);
        spawnCooldown = 0.3f;
        TraceLog(LOG_INFO, "Machinegun en (%.0f, %.0f)", worldPos.x, worldPos.y);
    }
    if (IsKeyPressed(KEY_P) && spawnCooldown <= 0.0f) {
        creationManager.GetItems().emplace_back(worldPos, ItemType::PLUSHY);
        spawnCooldown = 0.3f;
        TraceLog(LOG_INFO, "Plushy en (%.0f, %.0f)", worldPos.x, worldPos.y);
    }
    if (IsKeyPressed(KEY_K) && spawnCooldown <= 0.0f) {
        creationManager.GetItems().emplace_back(worldPos, ItemType::FISH);
        spawnCooldown = 0.3f;
        TraceLog(LOG_INFO, "Fish en (%.0f, %.0f)", worldPos.x, worldPos.y);
    }
    if (IsKeyPressed(KEY_L) && spawnCooldown <= 0.0f) {
        creationManager.GetItems().emplace_back(worldPos, ItemType::MEDAL);
        spawnCooldown = 0.3f;
        TraceLog(LOG_INFO, "Fish en (%.0f, %.0f)", worldPos.x, worldPos.y);
    }

    if (IsKeyPressed(KEY_F5)) {
        SaveToFile("level.txt");
        TraceLog(LOG_INFO, "Nivel guardado");
    }
}

void Debug::SaveToFile(const char* filename) const
{
    creationManager.GetTileMap().SaveToFile(filename);

    FILE* f = fopen(filename, "a");
    if (!f) return;

    for (const auto& s : creationManager.GetSoldiers())
        fprintf(f, "S %.0f %.0f %d\n", s.GetX(), s.GetY(), const_cast<Soldier&>(s).GetType());

    for (const auto& item : creationManager.GetItems()) {
        int t = 0;
        switch (item.GetType()) {
        case ItemType::SHOTGUN: t = 0; break;
        case ItemType::BOX:     t = 1; break;
        case ItemType::PLUSHY:  t = 2; break;
        case ItemType::FISH:    t = 3; break;
        case ItemType::MEDAL:    t = 4; break;
        }
        fprintf(f, "I %.0f %.0f %d\n", item.GetPosition().x, item.GetPosition().y, t);
    }

    fclose(f);
}

void Debug::GeneralDebugInput()
{
    if (IsKeyPressed(KEY_L)) uiManager.NextLevel();
    if (IsKeyPressed(KEY_J)) uiManager.AddScore(100);
    if (IsKeyPressed(KEY_C) && timerManager.IsReady(TimerType::CREDIT_COOLDOWN)) {
        if (uiManager.GetCredits() < 99) {
            uiManager.SetCredits(1);
            timerManager.StartTimer(TimerType::CREDIT_DELAY);
        }
    }
}

void Debug::EnableHitboxes() {}

void Debug::DrawEditorGrid(Camera2D cam)
{
    if (!editorMode) return;

    float hw = GetScreenWidth() / 2.0f;
    float hh = GetScreenHeight() / 2.0f;
    float worldLeft = cam.target.x - hw;
    float worldTop = cam.target.y - hh;
    float worldRight = cam.target.x + hw;
    float worldBottom = cam.target.y + hh;

    float startX = worldLeft + fmodf(gridOffset.x - worldLeft, TILE_SIZE);
    float startY = worldTop + fmodf(gridOffset.y - worldTop, TILE_SIZE);

    for (float x = startX; x < worldRight; x += TILE_SIZE)
        DrawLineV({ x, worldTop }, { x, worldBottom }, ColorAlpha(GRAY, 0.4f));
    for (float y = startY; y < worldBottom; y += TILE_SIZE)
        DrawLineV({ worldLeft, y }, { worldRight, y }, ColorAlpha(GRAY, 0.4f));
}