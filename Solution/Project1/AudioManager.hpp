#pragma once
#include "raylib.h"

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    void PlayMusic(Music music);
    void StopMusic(Music music);
    void UpdateMusic(Music music);
    void PlaySound(Sound sound);
    void StopSound(Sound sound);
    void Init();

    Music& GetTitleMusic();
    Music& GetGameMusic();
    Sound& GetGameSound();
    Sound& GetDeathSound();
    Sound& GetShootSound();
    Sound& GetGrenadeSound();
    Sound& GetMachinegunEquipSound();
    Sound& GetMachinegunShootSound();

private:
    Music titleMusic;
    Music gameMusic;
    Sound gameSound;
    Sound deathSound;
    Sound shootSound;
    Sound grenadeSound;
    Sound machinegunEquipSound;
    Sound machinegunShootSound;
};