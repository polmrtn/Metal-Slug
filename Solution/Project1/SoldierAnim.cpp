#include "SoldierAnim.hpp"

SoldierAnim::SoldierAnim()
{
	LoadTexture();
	SetAnimation(SoldierState::IDLE);
    isShooting = false;
    frame = 0;
    timer = 0;
    animForward = true;
    attackPeakReached = false;
    animCompleted = false;
}
		
Rectangle SoldierAnim::GetSourceRect() {
	const AnimClip& clip = CLIPS[(int)currentAnim];
	return { frame * clip.cellW, clip.rowY, clip.cellW, clip.cellH };
	
}
void SoldierAnim::UnloadTextures() {
    UnloadTexture(spriteSheet);
}
SoldierAnim::~SoldierAnim()
{

}

void SoldierAnim::LoadTexture()
{
	Image img = LoadImage("Graphics/NEW/Sprite-0002.png");
	spriteSheet = LoadTextureFromImage(img);
	SetTextureFilter(spriteSheet, TEXTURE_FILTER_POINT);
}

void SoldierAnim::Update()
{
    float dt = GetFrameTime();
    const AnimClip& clip = CLIPS[(int)currentAnim];
    timer += dt;

    if (timer >= 1.f / clip.fps) {
        timer = 0.f;
        attackPeakReached = false;
        if (currentAnim == SoldierState::ATTACKING) { // logica para que se devuelva la animacion ya que haya terminado
            if (animForward) {
                frame++;
                if (frame >= clip.frames - 1) {
                    attackPeakReached = true;
                    animForward = false;
                    
                }
            }
            else {
                frame--;
                if (frame <= 0) {
                    frame = 0;
                    animCompleted = true;
                }
            }
        }
        else if (clip.loop) {
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

void SoldierAnim::SetAnimation(SoldierState animation)
{
    if (currentAnim == animation ) return;
    currentAnim = animation;
    frame = 0;
    timer = 0.f;
    animForward = true;
    // reset flags
    attackPeakReached = false;
    animCompleted = false;
}

void SoldierAnim::ForceAnimation(SoldierState animation) {
    currentAnim = animation;
    frame = 0;
    timer = 0.f;
    animForward = true;
    attackPeakReached = false;
    animCompleted = false;
}

bool SoldierAnim::IsAnimationFinished() const {
    return animCompleted;
}

