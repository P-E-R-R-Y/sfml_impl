/**
 * @file SfmlAudioModule.hpp
 * @author Perry Chouteau (perry.chouteau@outlook.com)
 * @brief
 * @date 2026-08-10
 *
 * @copyright Copyright (c) 2026
 *
 * @addtogroup sfml
 * @{
 */

#ifndef SFMLAUDIO_MODULE_HPP
#define SFMLAUDIO_MODULE_HPP

#include "IAudioModule.hpp"

#include "audio/SfmlMusic.hpp"
#include "audio/SfmlSound.hpp"
#include "audio/SfmlSoundBuffer.hpp"

/**
 * @brief sfml has no explicit audio device init/close step (unlike raylib),
 *        nothing to do at construction/destruction here.
 */
class SfmlAudioModule : public IAudioModule {

public:
    SfmlAudioModule() = default;
    ~SfmlAudioModule() = default;

    const char *type() const override { return IAudioModule::contract; }
    const char *name() const override { return "sfml"; }

    // music
    audio::IMusic *createMusic(std::string path) override {
        return new SfmlMusic(path);
    }
    void deleteMusic(audio::IMusic *music) override {
        delete music;
    }

    // sound buffer
    audio::ISoundBuffer *createSoundBuffer(std::string path) override {
        return new SfmlSoundBuffer(path);
    }
    void deleteSoundBuffer(audio::ISoundBuffer *buffer) override {
        delete buffer;
    }

    // sound
    audio::ISound *createSound(audio::ISoundBuffer *buffer) override {
        return new SfmlSound(*static_cast<SfmlSoundBuffer *>(buffer));
    }
    void deleteSound(audio::ISound *sound) override {
        delete sound;
    }
};

/** @} */

#endif /* !SFMLAUDIO_MODULE_HPP */
