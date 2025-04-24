
#include "sound.h"

Mix_Chunk* Sound::bullet_sound = nullptr;
Mix_Chunk* Sound::jump_sound = nullptr;
Mix_Chunk* Sound::explosion_sound = nullptr;
Mix_Music* Sound::background_music = nullptr;

bool Sound::Init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer error: " << Mix_GetError() << "\n";
        return false;
    }

    bullet_sound = Mix_LoadWAV("assets/sounds/bullet.wav");
    jump_sound = Mix_LoadWAV("assets/sounds/jump.wav");
    explosion_sound = Mix_LoadWAV("assets/sounds/explosion.wav");
    background_music = Mix_LoadMUS("assets/sounds/background.ogg");

    if (!bullet_sound || !jump_sound || !explosion_sound || !background_music) {
        std::cout << "Failed to load sound: " << Mix_GetError() << "\n";
        return false;
    }

    return true;
}

void Sound::PlayBullet() {
    Mix_PlayChannel(-1, bullet_sound, 0);
}

void Sound::PlayJump() {
    Mix_PlayChannel(-1, jump_sound, 0);
}

void Sound::PlayExplosion() {
    Mix_PlayChannel(-1, explosion_sound, 0);
}

void Sound::PlayBackground() {
    Mix_PlayMusic(background_music, -1);
}

void Sound::CleanUp() {
    Mix_FreeChunk(bullet_sound);
    Mix_FreeChunk(jump_sound);
    Mix_FreeChunk(explosion_sound);
    Mix_FreeMusic(background_music);
    Mix_CloseAudio();
}
