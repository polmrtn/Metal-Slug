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
    Music& GetTitleMusic();
    Music& GetGameMusic();
    Sound& GetGameSound();
    Sound& GetDeathSound();
    Sound& GetShootSound();
private:
    Music titleMusic;
    Music gameMusic;
    Sound gameSound;
    Sound deathSound;
    Sound shootSound;
};