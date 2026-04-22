#include "BackgroundManager.hpp"
#include <math.h>

BackgroundManager::BackgroundManager() {
    AddSprite("Graphics/background sprites/bg1 1.png", { 8000, 155 }, 4.0f, 0.05f, 1.0f, false, 0);
    AddSprite("Graphics/background sprites/bg1 2 3.png", { 10300, 0 }, 4.0f, 0.05f, 1.5f, false, 1);
    AddSprite("Graphics/background sprites/bg4.png", { 0, -165 }, 4.0f, 0.0f, 1.0f, false, 1);
    AddSprite("Graphics/background sprites/more shit and huts/rockboss2.png", { 2200, 670 }, 4.0f, 0.0f, 1.0f, false, 1);
    AddSprite("Graphics/background sprites/more shit and huts/rockboss2.png", { 7700, 800 }, 4.0f, 0.0f, 1.0f, false, 2);
    AddSprite("Graphics/background sprites/more shit and huts/metalboss4.png", { 5400, 690 }, 4.0f, 0.0f, 1.0f, true, 1);
    AddSprite("Graphics/background sprites/more shit and huts/metalboss4.png", { 5400, 690 }, 4.0f, 0.0f, 1.0f, true, 1);
    AddSprite("Graphics/background sprites/spw.png", { 14070, -450 }, 4.0f, 0.0f, 1.0f, true, 2);

    AddAnimation("Graphics/background sprites/bg3.png", { 12000, 0 }, 8, 25.0f, 4.0f, 0.0f, 0.7f, 1.2f, false, 0, 1);
    AddAnimation("Graphics/background sprites/bg2 2.png", { 14500, -350 }, 5, 25.0f, 4.0f, 2.0f, 0.17f, 1.0f, false, 1, 1);
    AddAnimation("Graphics/background sprites/bg floor water.png", { 7440, 860 }, 8, 15.0f, 4.0f, 2.0f, 0.0f, 1.0f, false, 1, 1);
    AddAnimation("Graphics/background sprites/bgwaterfallTOFI.png", { 13350, -165 }, 8, 15.0f, 4.0f, 2.0f, 0.0f, 1.0f, false, 2, 1);
    AddAnimation("Graphics/background sprites/bfwaterfall2.png", { 13340, -165 }, 8, 15.0f, 4.0f, 2.0f, 0.0f, 1.0f, false, 2, 2);
    AddAnimation("Graphics/background sprites/water.png", { 7540, 930 }, 8, 15.0f, 4.0f, 2.0f, 0.0f, 1.0f, true, 2, 6);
    AddAnimation("Graphics/background sprites/wab1.png", { 15000, 600 }, 8, 15.0f, 4.0f, 2.0f, 0.0f, 1.0f, true, 2);

    AddEventSprite("Graphics/background sprites/more shit and huts/huts2.png", { 11050, 250 }, 6, 4.0f, 0.0f, 0.0f, false, 1);
    AddEventSprite("Graphics/background sprites/more shit and huts/hutss3big.png", { 11800, 150 }, 3, 4.0f, 0.0f, 0.0f, false, 1);
    AddEventSprite("Graphics/background sprites/more shit and huts/hutss3big.png", { 11800, 150 }, 3, 4.0f, 0.0f, 0.0f, false, 1);
    AddEventSprite("Graphics/background sprites/more shit and huts/woodboss1.png", { 8080, 800 }, 1, 4.0f, 0.0f, 0.0f, false, 1);
 /* AddEventSprite("Graphics/background sprites/more shit and huts/somehutsandwood.png", { 10200, 150 }, 1, 4.0f, 0.0f, 0.0f, false, 2);*/
    AddEventSprite("Graphics/boss1.png", { 15100, -400 }, 5, 4.0f, 0.0f, 0.0f, false, 2);
    scale = 4.0f;
    origin = { 0, 0 };
}

void BackgroundManager::AddSprite(const char* path, Vector2 pos, float scale, float parallaxFactor, float anchoExtra, bool isFrontground, int layer)
{
    Texture2D tex = LoadTexture(path);
    SetTextureFilter(tex, TEXTURE_FILTER_POINT);

    BackgroundSprite sprite;
    sprite.texture = tex;
    sprite.source = { 0, 0, (float)tex.width, (float)tex.height };
    sprite.dest = {
        pos.x, pos.y,
        tex.width * scale * anchoExtra,
        (float)tex.height * scale
    };
    sprite.parallaxFactor = parallaxFactor;
    sprite.isFrontground = isFrontground;
    sprite.layer = layer;

    if (pos.x + tex.width * scale > totalWidth) totalWidth = pos.x + tex.width * scale;
    if (tex.height * scale > totalHeight) totalHeight = tex.height * scale;

    sprites.push_back(sprite);
}

void BackgroundManager::AddAnimation(const char* path, Vector2 pos, int frames, float fps, float scale, float spacing, float pFactor, float anchoExtra, bool isFrontground, int layer, int repeatCount)
{
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
    newAnim.isFrontground = isFrontground;
    newAnim.layer = layer;
    newAnim.repeatCount = repeatCount;
    newAnim.source = { 0, 0, fWidth, (float)tex.height };
    newAnim.dest = {
        pos.x, pos.y,
        fWidth * scale * anchoExtra,
        (float)tex.height * scale
    };

    animations.push_back(newAnim);
}

void BackgroundManager::AddEventSprite(const char* path, Vector2 pos, int frames, float scale, float spacing, float parallaxFactor, bool isFrontground, int layer)
{
    Texture2D tex = LoadTexture(path);
    SetTextureFilter(tex, TEXTURE_FILTER_POINT);

    float fWidth = (float)(tex.width - (spacing * (frames - 1))) / frames;

    BackgroundEventSprite sprite;
    sprite.texture = tex;
    sprite.totalFrames = frames;
    sprite.currentFrame = 0;
    sprite.frameWidth = fWidth;
    sprite.spacing = spacing;
    sprite.parallaxFactor = parallaxFactor;
    sprite.isFrontground = isFrontground;
    sprite.layer = layer;
    sprite.source = { 0, 0, fWidth, (float)tex.height };
    sprite.dest = { pos.x, pos.y, fWidth * scale, (float)tex.height * scale };

    eventSprites.push_back(sprite);
}

void BackgroundManager::SetEventSpriteFrame(int index, int frame)
{
    if (index >= 0 && index < (int)eventSprites.size()) {
        eventSprites[index].SetFrame(frame);
    }
}

void BackgroundManager::FollowPlayer(Vector2 cameraPos) {
    this->camX = cameraPos.x;
}

void BackgroundManager::Update(float dt) {
    for (auto& anim : animations) {
        anim.Update(dt);
    }
}

void BackgroundManager::Draw() {
    for (int currentLayer = 0; currentLayer <= 2; currentLayer++) {

        // Sprites
        for (int i = 0; i < (int)sprites.size(); i++) {
            auto& sprite = sprites[i];
            if (sprite.isFrontground || sprite.layer != currentLayer) continue;

            if (i == 0) { // bg1 1 — repite 9 veces
                float imgWidth = sprite.dest.width;
                for (int j = 0; j < 9; j++) {
                    Rectangle dest = sprite.dest;
                    dest.x = (imgWidth * j) - fmod(this->camX * sprite.parallaxFactor, imgWidth);
                    DrawTexturePro(sprite.texture, sprite.source, dest, origin, 0, WHITE);
                }
            }
            else {
                Rectangle dest = sprite.dest;
                dest.x = sprite.dest.x - (this->camX * sprite.parallaxFactor);
                DrawTexturePro(sprite.texture, sprite.source, dest, origin, 0, WHITE);
            }
        }

        // Animaciones
        for (auto& anim : animations) {
            if (anim.isFrontground || anim.layer != currentLayer) continue;

            for (int r = 0; r < anim.repeatCount; r++) {
                Rectangle renderDest = anim.dest;
                renderDest.x = anim.dest.x + (anim.dest.width * r);

                if (this->camX > 12500.0f) {
                    float relativeTravel = this->camX - 12500.0f;
                    renderDest.x += relativeTravel * anim.parallaxFactor;
                }

                DrawTexturePro(anim.texture, anim.source, renderDest, origin, 0, WHITE);
            }
        }

        // EventSprites
        for (auto& sprite : eventSprites) {
            if (sprite.isFrontground || sprite.layer != currentLayer) continue;
            Rectangle dest = sprite.dest;
            dest.x = sprite.dest.x - (this->camX * sprite.parallaxFactor);
            DrawTexturePro(sprite.texture, sprite.source, dest, origin, 0, WHITE);
        }
    }
}

void BackgroundManager::Drawfrontground()
{
    for (auto& sprite : sprites) {
        if (!sprite.isFrontground) continue;
        Rectangle dest = sprite.dest;
        dest.x = sprite.dest.x - (this->camX * sprite.parallaxFactor);
        DrawTexturePro(sprite.texture, sprite.source, dest, origin, 0, WHITE);
    }

    for (auto& anim : animations) {
        if (!anim.isFrontground) continue;

        for (int r = 0; r < anim.repeatCount; r++) {
            Rectangle renderDest = anim.dest;
            renderDest.x = anim.dest.x + (anim.dest.width * r) - (this->camX * anim.parallaxFactor);
            DrawTexturePro(anim.texture, anim.source, renderDest, origin, 0, WHITE);
        }
    }

    for (auto& sprite : eventSprites) {
        if (!sprite.isFrontground) continue;
        Rectangle dest = sprite.dest;
        dest.x = sprite.dest.x - (this->camX * sprite.parallaxFactor);
        DrawTexturePro(sprite.texture, sprite.source, dest, origin, 0, WHITE);
    }
}

BackgroundManager::~BackgroundManager() {
    for (auto& sprite : sprites) {
        UnloadTexture(sprite.texture);
    }
    for (auto& anim : animations) {
        UnloadTexture(anim.texture);
    }
    for (auto& sprite : eventSprites) {
        UnloadTexture(sprite.texture);
    }
}