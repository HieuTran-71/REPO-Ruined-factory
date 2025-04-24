
#ifndef SOUND_H
#define SOUND_H

#include "CommonFunc.h"
#include <string>

class Sound {
public:
    static bool Init();
    static void CleanUp();

    static void PlayBullet();
    static void PlayJump();
    static void PlayExplosion();
    static void PlayBackground();

private:
    static Mix_Chunk* bullet_sound;
    static Mix_Chunk* jump_sound;
    static Mix_Chunk* explosion_sound;
    static Mix_Music* background_music;
};

#endif
