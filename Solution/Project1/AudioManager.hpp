#pragma once
#include "raylib.h"

class AudioManager {
public:
    AudioManager();
    ~AudioManager();
    void PlayMusic(Music music);
    void StopMusic(Music music);
    void UpdateMusic(Music music);
    Music& GetTitleMusic();
    Music& GetGameMusic();
private:
    Music titleMusic;
    Music gameMusic;
};