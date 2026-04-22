#include "AudioManager.hpp"
#include "raylib.h"

AudioManager::AudioManager()
{
    titleMusic = LoadMusicStream("OST/04. Steel Beast 5Beats (Boss Stage).ogg");
    gameMusic = LoadMusicStream("OST/03. Main Theme from Metal Slug (Stage 1).ogg");
    gameSound = LoadSound("OST/FX AUDIO/file002 mission 1 start.ogg");
    deathSound = LoadSound("OST/Non_music/npc_death.wav");
    shootSound = LoadSound("OST/Non_music/pistol_shot.wav");
    grenadeSound = LoadSound("OST/Non_music/grenade.wav");
    machinegunEquipSound = LoadSound("OST/FX AUDIO/file011 heavy machinegun.ogg");
    machinegunShootSound = LoadSound("OST/Non_music/shotgun_shot.wav");
}

Sound& AudioManager::GetDeathSound() { return deathSound; }
Sound& AudioManager::GetShootSound() { return shootSound; }
Sound& AudioManager::GetGrenadeSound() { return grenadeSound; }
Sound& AudioManager::GetMachinegunEquipSound() { return machinegunEquipSound; }
Sound& AudioManager::GetMachinegunShootSound() { return machinegunShootSound; }

Music& AudioManager::GetTitleMusic() { return titleMusic; }
Music& AudioManager::GetGameMusic() { return gameMusic; }
Sound& AudioManager::GetGameSound() { return gameSound; }

void AudioManager::PlayMusic(Music music) { PlayMusicStream(music); }
void AudioManager::UpdateMusic(Music music) { UpdateMusicStream(music); }
void AudioManager::StopMusic(Music music) { StopMusicStream(music); }
void AudioManager::PlaySound(Sound sound) { ::PlaySound(sound); }
void AudioManager::StopSound(Sound sound) { ::StopSound(sound); }

AudioManager::~AudioManager()
{
    UnloadSound(gameSound);
    UnloadMusicStream(titleMusic);
    UnloadMusicStream(gameMusic);
    UnloadSound(deathSound);
    UnloadSound(shootSound);
    UnloadSound(grenadeSound);
    UnloadSound(machinegunEquipSound);
    UnloadSound(machinegunShootSound);
}