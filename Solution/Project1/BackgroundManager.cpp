#include "BackgroundManager.hpp"
#include <math.h>

BackgroundManager::BackgroundManager() {
    background = LoadTexture("Graphics/background sprites/bg4.png");
    parallax1 = LoadTexture("Graphics/background sprites/bg1 1.png");
    parallax2 = LoadTexture("Graphics/background sprites/bg1 2.png");

    // Añadimos el barco con un factor de 0.1f (se mueve lento)
    AddAnimation("Graphics/background sprites/bg3.png", { 11500, 0 }, 8, 25.0f, 4.0f, 2.0f, 0.7f);
    AddAnimation("Graphics/background sprites/bg2 2.png", { 15000, -350 }, 5, 25.0f, 4.0f, 2.0f, 0.0f);

    scale = 4.0f;
    float yOffset = -165.0f;
    origin = { 0, 0 };

    source = { 0, 0, (float)background.width, (float)background.height };
    source1 = { 0, 0, (float)parallax1.width, (float)parallax1.height };
    source2 = { 0, 0, (float)parallax2.width, (float)parallax2.height };

    destination = { 0, yOffset, (float)background.width * scale, (float)background.height * scale };
    destination1 = { 0, 155, (float)parallax1.width * scale, (float)parallax1.height * scale };
    destination2 = { 9500, -20, (float)parallax2.width * scale, 100 + (float)parallax2.height * scale };
}

void BackgroundManager::AddAnimation(const char* path, Vector2 pos, int frames, float fps, float scale, float spacing, float pFactor) {
    Texture2D tex = LoadTexture(path);
    SetTextureFilter(tex, TEXTURE_FILTER_POINT);

    float fWidth = (float)(tex.width - (spacing * (frames - 1))) / frames;

    BackgroundAnim newAnim;
    newAnim.texture = tex;
    newAnim.totalFrames = frames;
    newAnim.fps = fps;
    newAnim.currentFrame = 0;
    newAnim.timer = 0.0f;
    newAnim.spacing = spacing;
    newAnim.active = true;
    newAnim.parallaxFactor = pFactor;
    newAnim.source = { 0, 0, fWidth, (float)tex.height };

    // ESCALA ASIMÉTRICA: Aquí aplicamos el ancho extra y usamos pos.y
    float anchoExtra = 1.5f;
    newAnim.dest = {
        pos.x,
        pos.y, // <--- AQUÍ se usa el -300 que le pases
        fWidth * scale * anchoExtra,
        (float)tex.height * scale
    };

    animations.push_back(newAnim);
}

void BackgroundManager::FollowPlayer(Vector2 cameraPos) {
    this->camX = cameraPos.x; // Guardamos la posición X real
    parallaxOffset1 = cameraPos.x * 0.05f;
}

void BackgroundManager::Update(float dt) {
    for (auto& anim : animations) {
        anim.Update(dt);
    }
}

void BackgroundManager::Draw() {
    // 1. Dibujar Animaciones con Parallax
    for (auto& anim : animations) {
        Rectangle renderDest = anim.dest;

        if (this->camX > 12500.0f) {
            float relativeTravel = this->camX - 12500.0f;

            // CAMBIO: Usamos + para que el fondo se mueva a la derecha con el player
            // Si 0.1f es mucho, baja el valor a 0.05f
            renderDest.x = anim.dest.x + (relativeTravel * anim.parallaxFactor);
        }
        else {
            renderDest.x = anim.dest.x;
        }

        DrawTexturePro(anim.texture, anim.source, renderDest, origin, 0, WHITE);
    }

    // 2. Dibujar Parallax infinito (Capa 1)
    float imgWidth1 = destination1.width;
    for (int i = 0; i < 3; i++) {
        Rectangle d1 = destination1;
        d1.x = (imgWidth1 * i) - fmod(parallaxOffset1, imgWidth1) + 8000;
        DrawTexturePro(parallax1, source1, d1, origin, 0, WHITE);
    }

    // 3. Resto de capas
    Rectangle d2 = destination2;
    d2.x = destination2.x - (this->camX * 0.05f); // Ejemplo de otro parallax directo
    DrawTexturePro(parallax2, source2, d2, origin, 0, WHITE);

    DrawTexturePro(background, source, destination, origin, 0, WHITE);
}

BackgroundManager::~BackgroundManager() {
    UnloadTexture(background);
    UnloadTexture(parallax1);
    UnloadTexture(parallax2);
    for (auto& anim : animations) {
        UnloadTexture(anim.texture);
    }
}