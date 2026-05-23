#include "AudioManager.hpp"
#include "raylib.h"

AudioManager::AudioManager()
{
   
}

void AudioManager::Init() {
    introMusic = LoadMusicStream("OST/FX AUDIO/INTRO AUDIO.ogg");
    introMusic.looping = false;   // gra tylko raz, nie zapętla
    SetMusicVolume(introMusic, 4.0f);
    howtoplayMusic = LoadMusicStream("OST/FX AUDIO/HOWTOPLAY AUDIO.ogg");
    howtoplayMusic.looping = false;
    titleMusic = LoadMusicStream("OST/04. Steel Beast 5Beats (Boss Stage).ogg");
    gameMusic = LoadMusicStream("OST/03. Main Theme from Metal Slug (Stage 1).ogg");
    gameSound = LoadSound("OST/FX AUDIO/file002 mission 1 start.ogg");
    deathSound  = LoadSound("OST/Non_music/npc_death.ogg");
    deathSound2 = LoadSound("OST/Non_music/npc_death2.mp3");
    SetSoundVolume(deathSound2, 6.5f);
    shootSound = LoadSound("OST/Non_music/pistol_shot.wav");
    grenadeSound = LoadSound("OST/Non_music/grenade.wav");
    machinegunEquipSound = LoadSound("OST/FX AUDIO/file011 heavy machinegun.ogg");
    machinegunShootSound = LoadSound("OST/Non_music/machinegun_shot.mp3");
    creditSound          = LoadSound("OST/Non_music/credit_sound.mp3");
    SetSoundVolume(creditSound, 0.6f);
}

Sound& AudioManager::GetDeathSound()  { return deathSound; }
Sound& AudioManager::GetDeathSound2() { return deathSound2; }

void AudioManager::PlayRandomDeathSound()
{
    // Jesli deathSound2 nie zaladowal sie poprawnie, zawsze graj deathSound
    if (deathSound2.stream.buffer == nullptr) {
        ::PlaySound(deathSound);
        return;
    }
    if (GetRandomValue(0, 1) == 0)
        ::PlaySound(deathSound);
    else
        ::PlaySound(deathSound2);
}
Sound& AudioManager::GetShootSound() { return shootSound; }
Sound& AudioManager::GetGrenadeSound() { return grenadeSound; }
Sound& AudioManager::GetMachinegunEquipSound() { return machinegunEquipSound; }
Sound& AudioManager::GetMachinegunShootSound() { return machinegunShootSound; }
Sound& AudioManager::GetCreditSound()          { return creditSound; }

Music& AudioManager::GetIntroMusic()      { return introMusic; }
Music& AudioManager::GetTitleMusic()      { return titleMusic; }
Music& AudioManager::GetGameMusic()       { return gameMusic; }
Music& AudioManager::GetHowtoplayMusic()  { return howtoplayMusic; }
Sound& AudioManager::GetGameSound() { return gameSound; }

void AudioManager::PlayMusic(Music music)   { PlayMusicStream(music); }
void AudioManager::UpdateMusic(Music music)  { UpdateMusicStream(music); }
void AudioManager::StopMusic(Music music)    { StopMusicStream(music); }
void AudioManager::StopIntroMusic()          { SetMusicVolume(introMusic, 0.0f); StopMusicStream(introMusic); }
void AudioManager::PlaySound(Sound sound) { ::PlaySound(sound); }
void AudioManager::StopSound(Sound sound) { ::StopSound(sound); }

AudioManager::~AudioManager()
{
    UnloadSound(gameSound);
    UnloadMusicStream(introMusic);
    UnloadMusicStream(howtoplayMusic);
    UnloadMusicStream(titleMusic);
    UnloadMusicStream(gameMusic);
    UnloadSound(deathSound);
    UnloadSound(deathSound2);
    UnloadSound(shootSound);
    UnloadSound(grenadeSound);
    UnloadSound(machinegunEquipSound);
    UnloadSound(machinegunShootSound);
    UnloadSound(creditSound);
}