#include "BulletAnim.hpp"
BulletAnim::BulletAnim() {
    SetAnimation(BulletState::GRENADESOLDIER);
    frame = 0;
    timer = 0;
    animForward = true;
    animCompleted = false;
}

BulletAnim::~BulletAnim()
{

}// En BulletAnim.cpp
bool BulletAnim::IsAnimationFinished() {
    // Si la animación actual es la explosión
    int index = (int)currentAnim;

    // Suponiendo que CLIPS tiene un tamaño definido (ej. 5)
    // Cambia '5' por el número real de animaciones que tengas
    if (index < 0 || index >= (sizeof(CLIPS) / sizeof(CLIPS[0]))) {
        return false;
    }

    if (currentAnim == BulletState::EXPLOSIONSOLDIER) {
        const AnimClipBullet& clip = CLIPS[index];
        return (frame >= clip.frames - 1);
    }
    return false;

}
void BulletAnim::LoadTexture(int type) {
    if (type == 1 )//Marco type grenade
    {
        Image img = LoadImage("Graphics/bullet.png");
        bulletPlayerImg = LoadTextureFromImage(img);
    }
    else if (type == 2) {
        Image img = LoadImage("Graphics/grenade_enemy.png");
        spriteSheet = LoadTextureFromImage(img);
        SetTextureFilter(spriteSheet, TEXTURE_FILTER_POINT);
    }
}

void BulletAnim::Update()
{
    float dt = GetFrameTime();
    const AnimClipBullet& clip = CLIPS[(int)currentAnim];
    timer += dt;

    if (timer >= 1.f / clip.fps) {
        timer = 0.f;
    
        if (clip.loop) {
            frame = (frame + 1) % clip.frames;
        }
        else {
            if (frame < clip.frames - 1) frame++;
            else {
                // Non-looping (non-pingpong) finished when reaching last frame
                animCompleted = true;

            }
        }
    }
}

void BulletAnim::SetAnimation(BulletState animation)
{
    if (currentAnim == animation) return;
    currentAnim = animation;
    frame = 0;
    timer = 0.f;
    animForward = true;
    // reset flags
    animCompleted = false;
}
Rectangle BulletAnim::GetSourceRect() {
    const AnimClipBullet& clip = CLIPS[(int)currentAnim];
    return { frame * clip.cellW, clip.rowY, clip.cellW, clip.cellH };

}

