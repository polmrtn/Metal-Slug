#include "BulletAnim.hpp"
BulletAnim::BulletAnim() {
    currentAnim = (BulletState)-1;
    
    frame = 0;
    timer = 0;
    animForward = true;
    animCompleted = false;
    SetAnimation(BulletState::GRENADESOLDIER);
}

BulletAnim::~BulletAnim()
{

}
bool BulletAnim::IsAnimationFinished() {
    // Si la animaci�n actual es la explosi�n
    int index = (int)currentAnim;

    // Suponiendo que CLIPS tiene un tama�o definido (ej. 5)
    // Cambia '5' por el n�mero real de animaciones que tengas
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
    else if (type == 3) {
        Image img = LoadImage("Graphics/bulletrifle.png");
        TraceLog(LOG_INFO, "bulletrifle loaded: %d x %d", img.width, img.height);
        bulletRifleImg = LoadTextureFromImage(img);
        SetTextureFilter(bulletRifleImg, TEXTURE_FILTER_POINT);
    }
}

void BulletAnim::Update()
{
    int index = (int)currentAnim;
    if (index < 0 || index >= (int)(sizeof(CLIPS) / sizeof(CLIPS[0]))) return;

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

