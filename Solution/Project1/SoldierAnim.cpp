#include "SoldierAnim.hpp"

SoldierAnim::SoldierAnim()
{
	LoadTexture();
	SetAnimation(SoldierState::IDLE);
    isShooting = false;
    frame = 0;
    timer = 0;
}
		
Rectangle SoldierAnim::GetSourceRect() {
	const AnimClip& clip = CLIPS[(int)currentAnim];
	return { frame * clip.cellW, clip.rowY, clip.cellW, clip.cellH };
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
		if (clip.loop) {
			frame = (frame + 1) % clip.frames;
		}
		else {
			if (frame < clip.frames - 1) frame++;
		}
	}
}


