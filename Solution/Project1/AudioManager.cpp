#include "AudioManager.hpp"
#include "raylib.h"
#include <cstdio>

AudioManager::AudioManager()
{
   
}

void AudioManager::Init() {
    introMusic = LoadMusicStream("OST/FX AUDIO/INTRO AUDIO.ogg");
    introMusic.looping = false;   // gra tylko raz, nie zapętla
    SetMusicVolume(introMusic, 4.0f);
    howtoplayMusic = LoadMusicStream("OST/FX AUDIO/HOWTOPLAY AUDIO.ogg");
    howtoplayMusic.looping = false;
    theEndMusic = LoadMusicStream("OST/Non_music/the-end-song.mp3");
    theEndMusic.looping = false;
    titleMusic = LoadMusicStream("OST/04. Steel Beast 5Beats (Boss Stage).ogg");
    gameMusic = LoadMusicStream("OST/03. Main Theme from Metal Slug (Stage 1).ogg");
    gameSound = LoadSound("OST/FX AUDIO/file002 mission 1 start.ogg");
    for (int i = 0; i < DEATH_SOUND_COUNT; i++) {
        char path[64];
        snprintf(path, sizeof(path), "OST/Non_music/soldier_death_%03d.mp3", i + 1);
        deathSounds[i] = LoadSound(path);
    }
    shootSound = LoadSound("OST/Non_music/pistol_shot.wav");
    grenadeSound = LoadSound("OST/Non_music/grenade.wav");
    machinegunEquipSound = LoadSound("OST/FX AUDIO/file011 heavy machinegun.ogg");
    machinegunShootSound = LoadSound("OST/Non_music/machinegun_shot.mp3");
    creditSound          = LoadSound("OST/Non_music/credit_sound.mp3");
    thankYouSound        = LoadSound("OST/Non_music/thank_you_sound.mp3");
    missionCompleteSound = LoadSound("OST/Non_music/metal-slug-mission-complete.mp3");
    jetpackPickupSound   = LoadSound("OST/Non_music/ok-metal-slug-sound.mp3");
    grenadePickupSound   = LoadSound("OST/Non_music/grenade_pickup_sound.mp3");
    playerDeathSound     = LoadSound("OST/Non_music/player_death.mp3");
    goSignSound          = LoadSound("OST/Non_music/go_sign.mp3");
    jetpackStartSound    = LoadSound("OST/Non_music/jetpack_jet_start.wav");
    jetpackMidSound      = LoadSound("OST/Non_music/jetpack_jet_mid.wav");
    jetpackStopSound     = LoadSound("OST/Non_music/jetpack_jet_stop.wav");
    SetSoundVolume(jetpackStartSound, 1.5f);
    SetSoundVolume(jetpackMidSound,   1.5f);
    SetSoundVolume(jetpackStopSound,  1.5f);
    gameOverSound        = LoadSound("OST/Non_music/game_over.mp3");
    bossBulletSound      = LoadSound("OST/Non_music/boss_bullet.mp3");
    bossOpenSound        = LoadSound("OST/Non_music/boss_sliding_open.mp3");
    bossBigBulletSound   = LoadSound("OST/Non_music/boss_bigbullet.mp3");
    SetSoundVolume(bossBigBulletSound, 3.0f);
    for (int i = 0; i < BOSS_DEATH_SOUND_COUNT; i++) {
        char path[64];
        snprintf(path, sizeof(path), "OST/Non_music/boss_death_%03d.mp3", i + 1);
        bossDeathSounds[i] = LoadSound(path);
    }
    SetSoundVolume(jetpackPickupSound, 1.5f);
    SetSoundVolume(missionCompleteSound, 6.0f);
    SetSoundVolume(thankYouSound, 3.0f);
    SetSoundVolume(creditSound, 0.6f);
}

void AudioManager::PlayRandomDeathSound()
{
    int idx = GetRandomValue(0, DEATH_SOUND_COUNT - 1);
    if (deathSounds[idx].stream.buffer != nullptr)
        ::PlaySound(deathSounds[idx]);
}
Sound& AudioManager::GetShootSound() { return shootSound; }
Sound& AudioManager::GetGrenadeSound() { return grenadeSound; }
Sound& AudioManager::GetMachinegunEquipSound() { return machinegunEquipSound; }
Sound& AudioManager::GetMachinegunShootSound() { return machinegunShootSound; }
Sound& AudioManager::GetCreditSound()          { return creditSound; }
Sound& AudioManager::GetThankYouSound()        { return thankYouSound; }
Sound& AudioManager::GetMissionCompleteSound() { return missionCompleteSound; }
Sound& AudioManager::GetJetpackPickupSound()   { return jetpackPickupSound; }
Sound& AudioManager::GetGrenadePickupSound()   { return grenadePickupSound; }
Sound& AudioManager::GetGameOverSound()        { return gameOverSound; }
Sound& AudioManager::GetBossBulletSound()      { return bossBulletSound; }
Sound& AudioManager::GetBossOpenSound()        { return bossOpenSound; }
Sound& AudioManager::GetBossBigBulletSound()   { return bossBigBulletSound; }
Sound& AudioManager::GetPlayerDeathSound()     { return playerDeathSound; }
Sound& AudioManager::GetGoSignSound()          { return goSignSound; }
Sound& AudioManager::GetJetpackStartSound()    { return jetpackStartSound; }
Sound& AudioManager::GetJetpackMidSound()      { return jetpackMidSound; }
Sound& AudioManager::GetJetpackStopSound()     { return jetpackStopSound; }

void AudioManager::PlayRandomBossDeathSound()
{
    int idx = GetRandomValue(0, BOSS_DEATH_SOUND_COUNT - 1);
    if (bossDeathSounds[idx].stream.buffer != nullptr)
        ::PlaySound(bossDeathSounds[idx]);
}

Music& AudioManager::GetIntroMusic()      { return introMusic; }
Music& AudioManager::GetTitleMusic()      { return titleMusic; }
Music& AudioManager::GetGameMusic()       { return gameMusic; }
Music& AudioManager::GetHowtoplayMusic()  { return howtoplayMusic; }
Music& AudioManager::GetTheEndMusic()     { return theEndMusic; }
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
    UnloadMusicStream(theEndMusic);
    UnloadMusicStream(titleMusic);
    UnloadMusicStream(gameMusic);
    for (int i = 0; i < DEATH_SOUND_COUNT; i++)
        UnloadSound(deathSounds[i]);
    UnloadSound(shootSound);
    UnloadSound(grenadeSound);
    UnloadSound(machinegunEquipSound);
    UnloadSound(machinegunShootSound);
    UnloadSound(creditSound);
    UnloadSound(thankYouSound);
    UnloadSound(missionCompleteSound);
    UnloadSound(jetpackPickupSound);
    UnloadSound(grenadePickupSound);
    UnloadSound(playerDeathSound);
    UnloadSound(goSignSound);
    UnloadSound(jetpackStartSound);
    UnloadSound(jetpackMidSound);
    UnloadSound(jetpackStopSound);
    UnloadSound(gameOverSound);
    UnloadSound(bossBulletSound);
    UnloadSound(bossOpenSound);
    UnloadSound(bossBigBulletSound);
    for (int i = 0; i < BOSS_DEATH_SOUND_COUNT; i++)
        UnloadSound(bossDeathSounds[i]);
}