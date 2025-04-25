#include "SoundManager.h"
#include <iostream>

// Khởi tạo biến static
SoundManager* SoundManager::s_instance = nullptr;

SoundManager* SoundManager::GetInstance() {
    if (s_instance == nullptr) {
        s_instance = new SoundManager();
    }
    return s_instance;
}

SoundManager::SoundManager() {
    background_music = nullptr;
    volume = 64; // Mức âm lượng mặc định (từ 0-128)
}

SoundManager::~SoundManager() {
    Clean();
}

bool SoundManager::Init() {
    // Khởi tạo SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    // Thiết lập số lượng kênh âm thanh (16 kênh)
    Mix_AllocateChannels(16);

    // Thiết lập âm lượng mặc định
    Mix_Volume(-1, volume);

    return true;
}

void SoundManager::Clean()
{
    // Giải phóng nhạc nền
    if (background_music != nullptr) {
        Mix_FreeMusic(background_music);
        background_music = nullptr;
    }

    // Giải phóng các hiệu ứng âm thanh
    for (auto& pair : sound_effects) {
        if (pair.second != nullptr) {
            Mix_FreeChunk(pair.second);
            pair.second = nullptr;
        }
    }
    sound_effects.clear();

    // Đóng SDL_mixer
    Mix_CloseAudio();

    // Xóa instance
    if (s_instance != nullptr) {
        delete s_instance;
        s_instance = nullptr;
    }
}

bool SoundManager::LoadSound(SoundType type, const std::string& path) {
    // Kiểm tra nếu là nhạc nền
    if (type == SOUND_MUSIC) {
        background_music = Mix_LoadMUS(path.c_str());
        if (background_music == nullptr) {
            std::cout << "Failed to load music! SDL_mixer Error: " << Mix_GetError() << std::endl;
            return false;
        }
        return true;
    }

    // Nếu là hiệu ứng âm thanh
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (chunk == nullptr) {
        std::cout << "Failed to load sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    // Lưu vào map
    sound_effects[type] = chunk;
    return true;
}

void SoundManager::PlaySound(SoundType type, int loops) {
    // Kiểm tra xem có sound effect không
    auto it = sound_effects.find(type);
    if (it != sound_effects.end()) {
        // Phát âm thanh ở kênh trống đầu tiên (-1)
        Mix_PlayChannel(-1, it->second, loops);
    }
}

void SoundManager::PlayMusic(int loops) {
    if (background_music != nullptr) {
        Mix_PlayMusic(background_music, loops);
    }
}

void SoundManager::StopMusic() {
    Mix_HaltMusic();
}

void SoundManager::PauseMusic() {
    Mix_PauseMusic();
}

void SoundManager::ResumeMusic() {
    Mix_ResumeMusic();
}

void SoundManager::SetVolume(int vol) {
    // Giới hạn âm lượng từ 0-128
    volume = (vol < 0) ? 0 : (vol > 128) ? 128 : vol;

    // Thiết lập âm lượng cho tất cả các kênh
    Mix_Volume(-1, volume);

    // Thiết lập âm lượng cho nhạc
    Mix_VolumeMusic(volume);
}

bool SoundManager::IsMusicPlaying() {
    return Mix_PlayingMusic() != 0;
}
