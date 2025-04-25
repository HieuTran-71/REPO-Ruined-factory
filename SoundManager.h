#ifndef SOUND_MANAGER_H_
#define SOUND_MANAGER_H_

#include "CommonFunc.h"
#include <map>

// Định nghĩa các loại âm thanh
enum SoundType {
    SOUND_MUSIC,     // Nhạc nền
    SOUND_BULLET,    // Tiếng đạn
    SOUND_EXPLOSION, // Tiếng nổ
    SOUND_JUMP,      // Tiếng nhảy
    SOUND_COLLECT,   // Tiếng nhặt vật phẩm
    SOUND_GAMEOVER,  // Tiếng kết thúc game
    SOUND_VICTORY    // Tiếng chiến thắng
};

class SoundManager {
public:
    // Constructor & Destructor
    SoundManager();
    ~SoundManager();

    // Khởi tạo hệ thống âm thanh
    bool Init();

    // Giải phóng tài nguyên âm thanh
    void Clean();

    // Load âm thanh từ file
    bool LoadSound(SoundType type, const std::string& path);

    // Phát âm thanh
    void PlaySound(SoundType type, int loops = 0);

    // Phát nhạc nền
    void PlayMusic(int loops = -1);

    // Dừng nhạc
    void StopMusic();

    // Tạm dừng nhạc
    void PauseMusic();

    // Tiếp tục phát nhạc
    void ResumeMusic();

    // Thiết lập âm lượng (0-128)
    void SetVolume(int volume);

    // Kiểm tra xem nhạc có đang phát không
    bool IsMusicPlaying();

    // Singleton pattern
    static SoundManager* GetInstance();

private:
    // Singleton instance
    static SoundManager* s_instance;

    // Danh sách các âm thanh
    std::map<SoundType, Mix_Chunk*> sound_effects;

    // Nhạc nền
    Mix_Music* background_music;

    // Âm lượng
    int volume;
};

#endif
