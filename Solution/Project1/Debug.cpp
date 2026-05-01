#define _CRT_SECURE_NO_WARNINGS
#include "Debug.hpp"

Debug::Debug() : editorMode(false) {}
Debug::~Debug() {}

// ─────────────────────────────────────────────────────────────
//  Editor visual (toggle con F1)
// ─────────────────────────────────────────────────────────────
void Debug::SetEditorMode()
{
    if (!editorMode) return;

    // Cuadrícula
    for (float x = fmodf(0, gridSize); x < (float)GetScreenWidth(); x += gridSize)
        DrawLineV({ x, 0 }, { x, (float)GetScreenHeight() }, ColorAlpha(GRAY, 0.4f));
    for (float y = fmodf(0, gridSize); y < (float)GetScreenHeight(); y += gridSize)
        DrawLineV({ 0, y }, { (float)GetScreenWidth(), y }, ColorAlpha(GRAY, 0.4f));

    // Tiles y colliders fusionados
    creationManager.GetTileMap().DrawTiles();
    creationManager.GetTileMap().DrawColliders();

    // HUD del editor
    DrawText("EDITOR  F1:Salir | LClick:Solid | RClick:Platform | C:Ceiling | R:Ramp | Del:Borrar | 1:Soldado | 2:Soldado2 | B:Caja | F5:Guardar",
        8, 8, 11, RED);

    Vector2 worldPos = cameraManager.GetScreenToWorld(GetMousePosition());
    DrawText(TextFormat("World (%.0f, %.0f)  Player (%.0f, %.0f)",
        worldPos.x, worldPos.y,
        player.GetPosition().x, player.GetPosition().y),
        8, 24, 12, YELLOW);

    // Tipo activo
    const char* typeName = "";
    switch (activeTileType) {
    case TileType::SOLID:    typeName = "SOLID";    break;
    case TileType::PLATFORM: typeName = "PLATFORM"; break;
    case TileType::CEILING:  typeName = "CEILING";  break;
    case TileType::RAMP_UP:  typeName = "RAMP_UP";  break;
    }
    DrawText(TextFormat("Tipo activo: %s", typeName), 8, 40, 12, WHITE);
}

void Debug::EditorModeInput()
{
    // Toggle editor
    static float f1Cooldown = 0.0f;
    if (f1Cooldown > 0.0f) f1Cooldown -= GetFrameTime();
    if (IsKeyPressed(KEY_F1) && f1Cooldown <= 0.0f) {
        editorMode = !editorMode;
        f1Cooldown = 0.2f;
        // Al salir del editor, rehacer el merge
        if (!editorMode) {
            creationManager.GetTileMap().Bake();
            TraceLog(LOG_INFO, "Editor cerrado — Bake ejecutado");
        }
    }

    if (!editorMode) return;

    // Selección de tipo con teclado
    if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) activeTileType = TileType::SOLID;
    if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) activeTileType = TileType::PLATFORM;
    if (IsKeyPressed(KEY_C))                              activeTileType = TileType::CEILING;
    if (IsKeyPressed(KEY_R))                              activeTileType = TileType::RAMP_UP;

    Vector2 mousePos = GetMousePosition();
    Vector2 worldPos = cameraManager.GetScreenToWorld(mousePos);

    // Añadir tile
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        creationManager.GetTileMap().AddTileWorld(worldPos.x, worldPos.y, activeTileType);

    // Atajo rápido: click derecho = PLATFORM
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        creationManager.GetTileMap().AddTileWorld(worldPos.x, worldPos.y, TileType::PLATFORM);

    // Borrar tile
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) || IsKeyDown(KEY_DELETE))
        creationManager.GetTileMap().RemoveTileWorld(worldPos.x, worldPos.y);

    // Soldados e items (en coordenadas de mundo)
    static float spawnCooldown = 0.0f;
    if (spawnCooldown > 0.0f) spawnCooldown -= GetFrameTime();

    if (IsKeyPressed(KEY_S) && spawnCooldown <= 0.0f) {
        creationManager.GetSoldiers().emplace_back(1, worldPos);
        spawnCooldown = 0.3f;
        TraceLog(LOG_INFO, "Soldado tipo 1 en (%.0f, %.0f)", worldPos.x, worldPos.y);
    }
    if (IsKeyPressed(KEY_D) && spawnCooldown <= 0.0f) {
        creationManager.GetSoldiers().emplace_back(2, worldPos);
        spawnCooldown = 0.3f;
        TraceLog(LOG_INFO, "Soldado tipo 2 en (%.0f, %.0f)", worldPos.x, worldPos.y);
    }
    if (IsKeyPressed(KEY_B) && spawnCooldown <= 0.0f) {
        creationManager.GetItems().emplace_back(worldPos, ItemType::BOX);
        spawnCooldown = 0.3f;
        TraceLog(LOG_INFO, "Caja en (%.0f, %.0f)", worldPos.x, worldPos.y);
    }

    // Guardar
    if (IsKeyPressed(KEY_F5)) {
        SaveToFile("level.txt");
        TraceLog(LOG_INFO, "Nivel guardado");
    }
}

// ─────────────────────────────────────────────────────────────
//  Guardar / Cargar
// ─────────────────────────────────────────────────────────────
void Debug::SaveToFile(const char* filename) const
{
    FILE* f = fopen(filename, "w");
    if (!f) return;

    // El TileMap guarda sus propios tiles internamente
    // Aquí guardamos soldados e items además
    // Primero guardamos el tilemap en el mismo archivo
    // Tiles: "col row type"
    // Soldados: "S x y type"
    // Items: "I x y type"

    // Necesitamos acceso a los tiles — los guardamos via TileMap::SaveToFile
    // pero como queremos un solo archivo, reescribimos aquí:
    creationManager.GetTileMap().SaveToFile(filename);  // guarda tiles

    // Ahora añadimos soldados e items al mismo archivo (append)
    f = fopen(filename, "a");
    if (!f) return;

    for (const auto& s : creationManager.GetSoldiers())
        fprintf(f, "S %.0f %.0f %d\n", s.GetX(), s.GetY(), const_cast<Soldier&>(s).GetType());

    for (const auto& item : creationManager.GetItems()) {
        int t = (item.GetType() == ItemType::BOX) ? 1 : 0;
        fprintf(f, "I %.0f %.0f %d\n", item.GetPosition().x, item.GetPosition().y, t);
    }

    fclose(f);
}

// ─────────────────────────────────────────────────────────────
//  Debug general (cheats)
// ─────────────────────────────────────────────────────────────
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