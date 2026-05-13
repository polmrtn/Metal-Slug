#define _CRT_SECURE_NO_WARNINGS
#include "Debug.hpp"

Debug::Debug() : editorMode(false) {}
Debug::~Debug() {}

void Debug::SetEditorMode(Camera2D cam)
{
    if (!editorMode) return;

<<<<<<< HEAD
    DrawText("EDITOR  F1:Salir | 1:Solid | 2:Platform | C:Ceiling | R:Ramp | Del:Borrar | F5:Guardar",
        8, 8, 11, RED);
    DrawText("ITEMS  B:Box | M:Machinegun | P:Plushy | K:Fish",
        8, 20, 11, GREEN);
    DrawText("ZONA CAM  Z:Crear | Ctrl+1/2:RangoX | Ctrl+3/4:RangoY | T/G:Toggle zone clamp X/Y",
        8, 32, 11, SKYBLUE);

    DrawText(TextFormat("Zona config: clampX=%d clampY=%d",
        pendingClampX, pendingClampY),
        8, 44, 11, SKYBLUE);

    // Show whether X/Y ranges were modified and current numeric ranges
    DrawText(TextFormat("RangoX active: %d  Xmin=%.0f Xmax=%.0f (step=%.0f)",
        pendingRangeActiveX ? 1 : 0, pendingZoneMinX, pendingZoneMaxX, Debug::PENDING_STEP_X),
        8, 56, 11, YELLOW);
    DrawText(TextFormat("RangoY active: %d  Ymin=%.0f Ymax=%.0f (step=%.0f)",
        pendingRangeActiveY ? 1 : 0, pendingZoneMinY, pendingZoneMaxY, Debug::PENDING_STEP_Y),
        8, 68, 11, YELLOW);

    Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), cam);
    DrawText(TextFormat("World (%.0f, %.0f)  GridOffset (%.0f, %.0f)",
        mouseWorld.x, mouseWorld.y, gridOffset.x, gridOffset.y),
        8, 80, 12, YELLOW);
=======
    // Sombra para efecto negrita
    DrawText("EDITOR | F1:Salir | Click:Solid | RClick:Platform | C:Ceiling | R:Ramp | Del:Borrar | S:Soldier1 | D:Soldier2 | B:Box | J:Jetpack | F5:Guardar",
        9, 9, 13, BLACK);
    DrawText("EDITOR | F1:Salir | Click:Solid | RClick:Platform | C:Ceiling | R:Ramp | Del:Borrar | S:Soldier1 | D:Soldier2 | B:Box | J:Jetpack | F5:Guardar",
        8, 8, 13, RED);

    Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), cam);

    DrawText(TextFormat("World (%.0f, %.0f)  GridOffset (%.0f, %.0f)",
        worldPos.x, worldPos.y, gridOffset.x, gridOffset.y),
        9, 25, 13, BLACK);
    DrawText(TextFormat("World (%.0f, %.0f)  GridOffset (%.0f, %.0f)",
        worldPos.x, worldPos.y, gridOffset.x, gridOffset.y),
        8, 24, 13, YELLOW);
>>>>>>> main

    const char* typeName = "";
    switch (activeTileType) {
    case TileType::SOLID:    typeName = "SOLID";    break;
    case TileType::PLATFORM: typeName = "PLATFORM"; break;
    case TileType::CEILING:  typeName = "CEILING";  break;
    case TileType::RAMP_UP:  typeName = "RAMP_UP";  break;
    }
<<<<<<< HEAD
    DrawText(TextFormat("Tipo activo: %s", typeName), 8, 94, 12, WHITE);

    // Visualizar zonas existentes: trigger line, label y rectángulo de la zona (usa minX/maxX como ABSOLUTAS)
    float worldTopY = cam.target.y - cam.offset.y;
    float worldBottomY = cam.target.y + cam.offset.y;
    for (const auto& zone : cameraManager.GetZones()) {
        DrawLineV({ zone.triggerX, worldTopY }, { zone.triggerX, worldBottomY }, ColorAlpha(SKYBLUE, 0.6f));

        Vector2 labelScreen = GetWorldToScreen2D({ zone.triggerX, worldTopY + 12.0f }, cam);
        DrawText(TextFormat("%s cX=%d cY=%d", zone.id.c_str(), (int)zone.clampX, (int)zone.clampY),
            (int)(labelScreen.x + 5), (int)labelScreen.y, 11, SKYBLUE);

        // zone.minX/maxX are absolute world coords now
        Vector2 tl = GetWorldToScreen2D({ zone.minX, zone.maxY }, cam);
        Vector2 br = GetWorldToScreen2D({ zone.maxX, zone.minY }, cam);
        Rectangle rz = { tl.x, tl.y, br.x - tl.x, br.y - tl.y };
        if (rz.width < 0) { rz.x += rz.width; rz.width = -rz.width; }
        if (rz.height < 0) { rz.y += rz.height; rz.height = -rz.height; }

        DrawRectangleRec(rz, ColorAlpha(SKYBLUE, 0.08f));
        DrawRectangleLinesEx(rz, 2.0f, ColorAlpha(SKYBLUE, 0.7f));
        DrawText(TextFormat("Y:%.0f/%.0f X:%.0f/%.0f", zone.minY, zone.maxY, zone.minX, zone.maxX),
            (int)(rz.x + 4), (int)(rz.y + 4), 10, SKYBLUE);
    }

    // Preview for pending zone at mouse X — pendingZoneMinX/MaxX are offsets, convert to absolute for preview
    {
        float tx = mouseWorld.x;
        float absMinX = tx + pendingZoneMinX;
        float absMaxX = tx + pendingZoneMaxX;
        float minY = mouseWorld.y + pendingZoneMinY;
        float maxY = mouseWorld.y + pendingZoneMaxY;

        DrawLineV({ tx, worldTopY }, { tx, worldBottomY }, ColorAlpha(GOLD, 0.6f));

        Color yColor = pendingRangeActiveY ? ColorAlpha(LIME, 0.95f) : ColorAlpha(LIME, 0.5f);
        DrawLineEx(GetWorldToScreen2D({ tx - 2000.0f, minY }, cam), GetWorldToScreen2D({ tx + 2000.0f, minY }, cam), 3.0f, yColor);
        DrawLineEx(GetWorldToScreen2D({ tx - 2000.0f, maxY }, cam), GetWorldToScreen2D({ tx + 2000.0f, maxY }, cam), 3.0f, yColor);
        DrawText(TextFormat("Ymin=%.0f  Ymax=%.0f", minY, maxY),
            (int)(GetWorldToScreen2D({ tx + 8.0f, minY }, cam).x),
            (int)(GetWorldToScreen2D({ tx + 8.0f, minY }, cam).y), 10, yColor);

        Vector2 p_tl = GetWorldToScreen2D({ absMinX, maxY }, cam);
        Vector2 p_br = GetWorldToScreen2D({ absMaxX, minY }, cam);
        Rectangle preview = { p_tl.x, p_tl.y, p_br.x - p_tl.x, p_br.y - p_tl.y };
        if (preview.width < 0) { preview.x += preview.width; preview.width = -preview.width; }
        if (preview.height < 0) { preview.y += preview.height; preview.height = -preview.height; }
        Color xColor = pendingRangeActiveX ? ColorAlpha(SKYBLUE, 0.8f) : ColorAlpha(GRAY, 0.35f);
        DrawRectangleRec(preview, ColorAlpha(xColor, 0.12f));
        DrawRectangleLinesEx(preview, 2.0f, xColor);
        DrawText(TextFormat("Xmin=%.0f Xmax=%.0f", absMinX, absMaxX),
            (int)(preview.x + 4), (int)(preview.y - 14), 10, xColor);
    }
=======
    DrawText(TextFormat("Tipo activo: %s", typeName), 9, 41, 13, BLACK);
    DrawText(TextFormat("Tipo activo: %s", typeName), 8, 40, 13, WHITE);
>>>>>>> main
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

    // Tipo de tile / atajos con CTRL para modificar rangos
    if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
        if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
            // Ctrl+1 -> aumentar rango X (por PENDING_STEP_X)
            pendingZoneMaxX += Debug::PENDING_STEP_X;
            pendingRangeActiveX = true;
            TraceLog(LOG_INFO, "PendingZone X increased: min=%.0f max=%.0f", pendingZoneMinX, pendingZoneMaxX);
        } else {
            activeTileType = TileType::SOLID;
        }
    }
    if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
        if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
            // Ctrl+2 -> disminuir rango X
            pendingZoneMaxX -= Debug::PENDING_STEP_X;
            if (pendingZoneMaxX < pendingZoneMinX) pendingZoneMaxX = pendingZoneMinX + Debug::PENDING_STEP_X;
            pendingRangeActiveX = true;
            TraceLog(LOG_INFO, "PendingZone X decreased: min=%.0f max=%.0f", pendingZoneMinX, pendingZoneMaxX);
        } else {
            activeTileType = TileType::PLATFORM;
        }
    }

    // Ctrl+3 / Ctrl+4 para ajustar rango Y (paso PENDING_STEP_Y)
    if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) {
        if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
            pendingZoneMaxY += Debug::PENDING_STEP_Y;
            pendingZoneMinY -= Debug::PENDING_STEP_Y;
            pendingRangeActiveY = true;
            TraceLog(LOG_INFO, "PendingZone Y expanded: min=%.0f max=%.0f", pendingZoneMinY, pendingZoneMaxY);
        }
    }
    if (IsKeyPressed(KEY_FOUR) || IsKeyPressed(KEY_KP_4)) {
        if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
            pendingZoneMaxY -= Debug::PENDING_STEP_Y;
            pendingZoneMinY += Debug::PENDING_STEP_Y;
            if (pendingZoneMaxY < pendingZoneMinY) {
                pendingZoneMinY = -100.f;
                pendingZoneMaxY = 100.f;
            }
            pendingRangeActiveY = true;
            TraceLog(LOG_INFO, "PendingZone Y shrunk: min=%.0f max=%.0f", pendingZoneMinY, pendingZoneMaxY);
        }
    }

    if (IsKeyPressed(KEY_C))                               activeTileType = TileType::CEILING;
    if (IsKeyPressed(KEY_R))                               activeTileType = TileType::RAMP_UP;

    // Mover grid
    const float GRID_SPEED = 150.0f;
    if (IsKeyDown(KEY_UP))    gridOffset.y -= GRID_SPEED * GetFrameTime();
    if (IsKeyDown(KEY_DOWN))  gridOffset.y += GRID_SPEED * GetFrameTime();
    if (IsKeyDown(KEY_LEFT))  gridOffset.x -= GRID_SPEED * GetFrameTime();
    if (IsKeyDown(KEY_RIGHT)) gridOffset.x += GRID_SPEED * GetFrameTime();
    creationManager.GetTileMap().SetGridOffset(gridOffset);

    Vector2 mousePos = GetMousePosition();
    Vector2 worldPos = GetScreenToWorld2D(mousePos, cam);

    // Tiles
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        creationManager.GetTileMap().AddTileWorld(worldPos.x, worldPos.y, activeTileType);
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        creationManager.GetTileMap().AddTileWorld(worldPos.x, worldPos.y, TileType::PLATFORM);
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) || IsKeyDown(KEY_DELETE)) {
        creationManager.GetTileMap().RemoveTileWorld(worldPos.x, worldPos.y);
        auto& items = creationManager.GetItems();
        items.erase(std::remove_if(items.begin(), items.end(),
            [&worldPos](const Item& i) {
                return CheckCollisionPointRec(worldPos, i.GetHitBox());
            }), items.end());
    }

    // Spawn cooldown
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

    // Config zona camara toggles for pending parameters
    if (IsKeyPressed(KEY_X)) {
        pendingClampX = !pendingClampX;
        TraceLog(LOG_INFO, "Pending clampX=%d", pendingClampX);
    }
    if (IsKeyPressed(KEY_Y)) {
        pendingClampY = !pendingClampY;
            TraceLog(LOG_INFO, "Pending clampY=%d", pendingClampY);
    }

    // Toggle clamp flags on nearest zone under mouse: T -> clampX, G -> clampY
    if (IsKeyPressed(KEY_T)) {
        const auto& zones = cameraManager.GetZones();
        float bestDist = 1e9f; std::string bestId;
        for (const auto& z : zones) {
            float d = fabsf(z.triggerX - worldPos.x);
            if (d < bestDist && d < 100.0f) { bestDist = d; bestId = z.id; }
        }
        if (!bestId.empty()) {
            cameraManager.ToggleZoneClampX(bestId);
            TraceLog(LOG_INFO, "Toggled clampX for zone '%s'", bestId.c_str());
        } else TraceLog(LOG_INFO, "No zone near mouse to toggle clampX");
    }
    if (IsKeyPressed(KEY_G)) {
        const auto& zones = cameraManager.GetZones();
        float bestDist = 1e9f; std::string bestId;
        for (const auto& z : zones) {
            float d = fabsf(z.triggerX - worldPos.x);
            if (d < bestDist && d < 100.0f) { bestDist = d; bestId = z.id; }
        }
        if (!bestId.empty()) {
            cameraManager.ToggleZoneClampY(bestId);
            TraceLog(LOG_INFO, "Toggled clampY for zone '%s'", bestId.c_str());
        } else TraceLog(LOG_INFO, "No zone near mouse to toggle clampY");
    }

    // Crear zona de camara (use absolute X limits computed from mouse X + pending offsets)
    if (IsKeyPressed(KEY_Z) && spawnCooldown <= 0.0f) {
        char id[32];
        sprintf(id, "zone%d", zoneCount++);
        float absMinX = worldPos.x + pendingZoneMinX;
        float absMaxX = worldPos.x + pendingZoneMaxX;
        cameraManager.AddZone(
            id,
            worldPos.x,
            worldPos.y + pendingZoneMinY,
            worldPos.y + pendingZoneMaxY,
            absMinX,
            absMaxX,
            pendingClampX,
            pendingClampY
        );
        pendingRangeActiveX = true;
        pendingRangeActiveY = true;
        spawnCooldown = 0.3f;
        TraceLog(LOG_INFO, "Zona '%s' en X=%.0f absXrange=%.0f..%.0f clampX=%d clampY=%d",
            id, worldPos.x, absMinX, absMaxX, pendingClampX, pendingClampY);
    }
    if (IsKeyPressed(KEY_J) && spawnCooldown <= 0.0f) {
        creationManager.GetItems().emplace_back(worldPos, ItemType::JETPACK);
        spawnCooldown = 0.3f;
    }

    if (IsKeyPressed(KEY_F5)) {
        SaveToFile("level.txt");
        TraceLog(LOG_INFO, "Nivel guardado");
    }
}

void Debug::SaveToFile(const char* filename) const
{
    creationManager.GetTileMap().SaveToFile(filename);

<<<<<<< HEAD
    FILE* f = fopen(filename, "a");
=======
    creationManager.GetTileMap().SaveToFile(filename);  

    f = fopen(filename, "a");
>>>>>>> main
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
        case ItemType::MEDAL:   t = 4; break;
        }
        fprintf(f, "I %.0f %.0f %d\n", item.GetPosition().x, item.GetPosition().y, t);
    }

<<<<<<< HEAD
    for (const auto& zone : cameraManager.GetZones()) {
        fprintf(f, "Z %.0f %.0f %.0f %.0f %.0f %s %d %d\n",
            zone.triggerX,
            zone.minY, zone.maxY,
            zone.minX, zone.maxX,
            zone.id.c_str(),
            (int)zone.clampX, (int)zone.clampY);
=======
    for (const auto& item : creationManager.GetItems()) {
        int t = 0;
        if (item.GetType() == ItemType::BOX)     t = 1;
        if (item.GetType() == ItemType::JETPACK) t = 2;
        fprintf(f, "I %.0f %.0f %d\n", item.GetPosition().x, item.GetPosition().y, t);
>>>>>>> main
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