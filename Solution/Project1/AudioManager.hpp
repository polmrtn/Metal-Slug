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
    Sound& GetDeathSound();
    Sound& GetShootSound();
    Sound& GetGrenadeSound();
    Sound& GetMachinegunEquipSound();
    Sound& GetMachinegunShootSound();

private:
    Music introMusic;
    Music titleMusic;
    Music gameMusic;
    Music howtoplayMusic;
    Sound gameSound;
    Sound deathSound;
    Sound shootSound;
    Sound grenadeSound;
    Sound machinegunEquipSound;
    Sound machinegunShootSound;
};