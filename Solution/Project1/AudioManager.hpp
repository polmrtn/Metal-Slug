#pragma once
#include "raylib.h"

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    void PlayMusic(Music music);
    void StopMusic(Music music);
    void UpdateMusic(Music music);
    void StopIntroMusic();
    void PlaySound(Sound sound);
    void StopSound(Sound sound);
    void Init();

    Music& GetIntroMusic();
    Music& GetTitleMusic();
    Music& GetGameMusic();
    Music& GetHowtoplayMusic();
    Sound& GetGameSound();
    void   PlayRandomDeathSound();
    Sound& GetGameOverSound();
    Sound& GetBossBulletSound();
    Sound& GetBossOpenSound();
    Sound& GetBossBigBulletSound();
    void   PlayRandomBossDeathSound();
    Sound& GetThankYouSound();
    Sound& GetMissionCompleteSound();
    Sound& GetJetpackPickupSound();
    Sound& GetGrenadePickupSound();
    Sound& GetShootSound();
    Sound& GetGrenadeSound();
    Sound& GetMachinegunEquipSound();
    Sound& GetMachinegunShootSound();
    Sound& GetCreditSound();
    Sound& GetPlayerDeathSound();
    Sound& GetGoSignSound();
    Sound& GetJetpackStartSound();
    Sound& GetJetpackMidSound();
    Sound& GetJetpackStopSound();

private:
    Music introMusic;
    Music titleMusic;
    Music gameMusic;
    Music howtoplayMusic;
    Sound gameSound;
    static constexpr int DEATH_SOUND_COUNT = 6;
    Sound deathSounds[DEATH_SOUND_COUNT];
    Sound gameOverSound;
    Sound bossBulletSound;
    Sound bossOpenSound;
    Sound bossBigBulletSound;
    static constexpr int BOSS_DEATH_SOUND_COUNT = 3;
    Sound bossDeathSounds[BOSS_DEATH_SOUND_COUNT];
    Sound shootSound;
    Sound grenadeSound;
    Sound machinegunEquipSound;
    Sound machinegunShootSound;
    Sound creditSound;
    Sound thankYouSound;
    Sound missionCompleteSound;
    Sound jetpackPickupSound;
    Sound grenadePickupSound;
    Sound playerDeathSound;
    Sound goSignSound;
    Sound jetpackStartSound;
    Sound jetpackMidSound;
    Sound jetpackStopSound;
};