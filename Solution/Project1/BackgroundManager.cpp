#include "BackgroundManager.hpp"
#include "raymath.h"

BackgroundManager::BackgroundManager()
{
    yposSprite = 0;
    xposSprite = 0;
    int type = 0;

    background = LoadTexture("Graphics/background sprites/bg4.png");
	parallax1 = LoadTexture("Graphics/background sprites/bg1 1.png");

    // ========== PARÁMETROS AJUSTABLES ==========
    float scale = 4.0f;  // Escala única (ancho y alto se multiplican por esto)
    float yOffset = -165.0f;  // Desplazamiento vertical (ajusta si es necesario)
    // ===========================================

    float scaledWidth = background.width * scale;
    float scaledHeight = background.height * scale;  // 224 * escala
	source2 = { 0, 0, (float)parallax1.width, (float)parallax1.height };
    source = { 0, 0, (float)background.width, (float)background.height };
    destination = { 0, yposSprite + yOffset, scaledWidth, scaledHeight };
    destination2 = { 0, yposSprite ,  (float)parallax1.width * scale , (float)parallax1.height * scale };
    origin = { 0, 0 };

    this->scale = scale;
}

BackgroundManager::~BackgroundManager()
{
    UnloadTexture(background);
}


void BackgroundManager::FollowPlayer(Vector2 cameraPos) //solo parallax
{
    float maxX = parallax1.width - (destination.width / scale);
    source.x = cameraPos.x;

    if (source.x < 0) source.x = 0;
    if (source.x > maxX) source.x = maxX;
}

void BackgroundManager::Draw()
{
	float imgWidth = destination2.width; //logica para loopear imagen parallax

    for (int i = 0; i < 2; i++) {
        Rectangle dest = destination2;
        dest.x = imgWidth * i;
        DrawTexturePro(parallax1, source2, dest, origin, 0, WHITE);
    }
    
    DrawTexturePro(background, source, destination, origin, 0, WHITE);
	
}