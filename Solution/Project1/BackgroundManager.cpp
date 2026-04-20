#include "BackgroundManager.hpp"
#include "raymath.h"

BackgroundManager::BackgroundManager()
{
    yposSprite = 0;
    xposSprite = 0;
    int type = 0;

    background = LoadTexture("Graphics/background sprites/bg4.png");
	parallax1 = LoadTexture("Graphics/background sprites/bg1 1.png");
    parallax2 = LoadTexture("Graphics/background sprites/bg1 2.png");

    // ========== PARÁMETROS AJUSTABLES ==========
    float scale = 4.0f;  // Escala única (ancho y alto se multiplican por esto)
    float yOffset = -165.0f;  // Desplazamiento vertical (ajusta si es necesario)
    // ===========================================

    float scaledWidth = background.width * scale;
    float scaledHeight = background.height * scale;  // 224 * escala
    origin = { 0, 0 };
	
	source1 = { 0, 0, (float)parallax1.width, (float)parallax1.height };
    source2 = { 0, 0, (float)parallax2.width, (float)parallax2.height };
    source = { 0, 0, (float)background.width, (float)background.height };
    
    
    destination = { 0, 0 + yOffset,scaledWidth, scaledHeight };
	destination1 = { 0, yposSprite ,  (float)parallax1.width * scale , (float)parallax1.height * scale };
    destination2 = { 1845, yposSprite ,  (float)parallax2.width * scale , (float)parallax2.height * scale };
   

    this->scale = scale;
}

BackgroundManager::~BackgroundManager()
{
    UnloadTexture(background);
	UnloadTexture(parallax1);
    UnloadTexture(parallax2);
}


void BackgroundManager::FollowPlayer(Vector2 cameraPos)
{
    float maxX = parallax1.width - (destination.width / scale);
    source1.x = cameraPos.x * 0.5f; // ← parallax más lento
    source2.x = cameraPos.x * 0.3f; // ← parallax aún más lento
    if (source1.x < 0) source1.x = 0;
    if (source2.x < 0) source2.x = 0;
    // ← source del background NO se toca aquí
}

void BackgroundManager::Draw()
{
    float imgWidth1 = destination1.width;

    for (int i = 0; i < 2; i++) {
        Rectangle dest1 = destination1;
        dest1.x = imgWidth1 * i;
        DrawTexturePro(parallax1, source1, dest1, origin, 0, WHITE);

        Rectangle dest2 = destination2;
        dest2.x = imgWidth1 * i; // ← o destination2.width * i si tienen distinto tamaño
        DrawTexturePro(parallax2, source2, dest2, origin, 0, WHITE);
    }

    // Background siempre fijo con su source sin modificar
    DrawTexturePro(background, source, destination, origin, 0, WHITE);
}