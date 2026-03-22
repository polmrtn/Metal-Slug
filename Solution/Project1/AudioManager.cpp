#include "AudioManager.hpp"

AudioManager::AudioManager()
{
	titleMusic = LoadMusicStream("OST/03. Main Theme from Metal Slug (Stage 1).ogg");
	gameMusic = LoadMusicStream("OST/FX AUDIO/file002 mission 1 start.ogg");
}
Music& AudioManager::GetTitleMusic() {
	return titleMusic;
}
Music& AudioManager::GetGameMusic() {
	return gameMusic;
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

AudioManager::~AudioManager()
{
	
	UnloadMusicStream(gameMusic);
	UnloadMusicStream(titleMusic);
}
