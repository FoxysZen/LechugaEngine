#pragma once
#include <string>

enum class AudioChannel { MUSIC, SFX, UI };

class AudioManager
{
    public:
        AudioManager() = default;
        ~AudioManager() = default;
    
        bool init();
        void shutdown();
    
        void playMusic(const std::string& path, bool loop = true);
        void stopMusic();
        void pauseMusic();
        void resumeMusic();
    
        void playSFX(const std::string& path);
        void playUI(const std::string& path);
    
        void setVolume(AudioChannel channel, float volume);
        float getVolume(AudioChannel channel) const;
        void setMasterVolume(float volume);
    
    private:
        struct AudioData;
        AudioData* data = nullptr;
    
        float volumes[3] = {1.0f, 1.0f, 1.0f};
        float masterVolume = 1.0f;
};