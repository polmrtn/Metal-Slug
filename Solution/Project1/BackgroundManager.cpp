#include "BackgroundManager.hpp"
#include "raymath.h"

BackgroundManager::BackgroundManager()
{
    yposSprite = 0;
    xposSprite = 0;

    background = LoadTexture("Graphics/NEW/bg4.png");

    // ========== PARÁMETROS AJUSTABLES ==========
    float scale = 4.0f;  // Escala única (ancho y alto se multiplican por esto)
    float yOffset = -165.0f;  // Desplazamiento vertical (ajusta si es necesario)
    // ===========================================

    float scaledWidth = background.width * scale;
    float scaledHeight = background.height * scale;  // 224 * escala

    source = { 0, 0, (float)background.width, (float)background.height };
    destination = { 0, yposSprite + yOffset, scaledWidth, scaledHeight };
    origin = { 0, 0 };

    this->scale = scale;
}

BackgroundManager::~BackgroundManager()
{
    UnloadTexture(background);
}

void BackgroundManager::FollowPlayer(Vector2 cameraPos)
{
    float maxX = background.width - (destination.width / scale);
    source.x = cameraPos.x;

    if (source.x < 0) source.x = 0;
    if (source.x > maxX) source.x = maxX;
}

void BackgroundManager::Draw()
{
    DrawTexturePro(background, source, destination, origin, 0, WHITE);
}