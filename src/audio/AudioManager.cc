#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "AudioManager.h"
#include <Logger.h>

struct AudioManager::AudioData
{
    ma_engine engine;
    ma_sound musicSound;
    bool musicLoaded = false;
    ma_sound_group groups[3];
};

bool AudioManager::init()
{
    data = new AudioData();

    if (ma_engine_init(nullptr, &data->engine) != MA_SUCCESS)
    {
        Logger::error("AudioManager: failed to init miniaudio engine");
        delete data;
        data = nullptr;
        return false;
    }

    ma_sound_group_init(&data->engine, 0, nullptr, 
                        &data->groups[(int)AudioChannel::MUSIC]);
    ma_sound_group_init(&data->engine, 0, nullptr, 
                        &data->groups[(int)AudioChannel::SFX]);
    ma_sound_group_init(&data->engine, 0, nullptr, 
                        &data->groups[(int)AudioChannel::UI]);

    Logger::info("AudioManager: initialized");
    return true;
}

void AudioManager::shutdown()
{
    if (!data) return;

    if (data->musicLoaded)
    {
        ma_sound_stop(&data->musicSound);
        ma_sound_uninit(&data->musicSound);
    }

    ma_sound_group_uninit(&data->groups[(int)AudioChannel::MUSIC]);
    ma_sound_group_uninit(&data->groups[(int)AudioChannel::SFX]);
    ma_sound_group_uninit(&data->groups[(int)AudioChannel::UI]);
    ma_engine_uninit(&data->engine);

    delete data;
    data = nullptr;
}

void AudioManager::playMusic(const std::string& path, bool loop)
{
    if (!data) return;

    if (data->musicLoaded)
    {
        ma_sound_stop(&data->musicSound);
        ma_sound_uninit(&data->musicSound);
        data->musicLoaded = false;
    }

    ma_result result = ma_sound_init_from_file(&data->engine, path.c_str(),
        MA_SOUND_FLAG_ASYNC, &data->groups[(int)AudioChannel::MUSIC],
        nullptr, &data->musicSound);

    if (result != MA_SUCCESS)
    {
        Logger::error("AudioManager: failed to load music: " + path);
        return;
    }

    ma_sound_set_looping(&data->musicSound, loop ? MA_TRUE : MA_FALSE);
    ma_sound_set_volume(&data->musicSound, 
                        volumes[(int)AudioChannel::MUSIC] * masterVolume);
    ma_sound_start(&data->musicSound);
    data->musicLoaded = true;
}

void AudioManager::stopMusic()
{
    if (!data || !data->musicLoaded) return;
    ma_sound_stop(&data->musicSound);
}

void AudioManager::pauseMusic()
{
    if (!data || !data->musicLoaded) return;
    ma_sound_stop(&data->musicSound);
}

void AudioManager::resumeMusic()
{
    if (!data || !data->musicLoaded) return;
    ma_sound_start(&data->musicSound);
}

void AudioManager::playSFX(const std::string& path)
{
    if (!data) return;
    ma_engine_play_sound(&data->engine, path.c_str(), 
                         &data->groups[(int)AudioChannel::SFX]);
}

void AudioManager::playUI(const std::string& path)
{
    if (!data) return;
    ma_engine_play_sound(&data->engine, path.c_str(), 
                         &data->groups[(int)AudioChannel::UI]);
}

void AudioManager::setVolume(AudioChannel channel, float volume)
{
    if (!data) return;
    volumes[(int)channel] = 
                        volume < 0.0f ? 0.0f : volume > 1.0f ? 1.0f : volume;
    ma_sound_group_set_volume(&data->groups[(int)channel], 
                              volumes[(int)channel] * masterVolume);
    if (channel == AudioChannel::MUSIC && data->musicLoaded)
        ma_sound_set_volume(&data->musicSound, 
                            volumes[(int)channel] * masterVolume);
}

float AudioManager::getVolume(AudioChannel channel) const
{
    return volumes[(int)channel];
}

void AudioManager::setMasterVolume(float volume)
{
    masterVolume = volume < 0.0f ? 0.0f : volume > 1.0f ? 1.0f : volume;
    if (data) ma_engine_set_volume(&data->engine, masterVolume);
}