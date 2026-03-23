#include "AudioManager.hpp"

AudioManager::AudioManager()
{
	titleMusic = LoadMusicStream("OST/04. Steel Beast 5Beats (Boss Stage).ogg");
	gameMusic = LoadMusicStream("OST/03. Main Theme from Metal Slug (Stage 1).ogg");
	gameSound = LoadSound("OST/FX AUDIO/file002 mission 1 start.ogg");
}
Music& AudioManager::GetTitleMusic() {
	return titleMusic;
}
Music& AudioManager::GetGameMusic() {
	return gameMusic;
}
Sound& AudioManager::GetGameSound() {
	return gameSound;
}
void AudioManager::PlayMusic(Music music)
{
	PlayMusicStream(music);
}
void AudioManager::UpdateMusic(Music music)
{
	UpdateMusicStream(music);
}
void AudioManager::StopMusic(Music music)
{
	StopMusicStream(music);
}
void AudioManager::PlaySound(Sound sound)
{
	::PlaySound(sound);
}
void AudioManager::StopSound(Sound sound)
{
	::StopSound(sound);
}

AudioManager::~AudioManager()
{
	
	UnloadSound(gameSound);
	UnloadMusicStream(titleMusic);
}
