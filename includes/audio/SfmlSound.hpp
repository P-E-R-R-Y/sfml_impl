/**
 * @file SfmlSound.hpp
 * @author Perry Chouteau (perry.chouteau@outlook.com)
 * @brief
 * @date 2026-08-10
 *
 * @addtogroup sfml
 * @{
 */

#ifndef SFMLSOUND_HPP_
#define SFMLSOUND_HPP_

//Sfml
#include <SFML/Audio.hpp>

//Interface
#include "ISound.hpp"

//encapsulation
#include "SfmlSoundBuffer.hpp"

/**
 * @brief Sfml Sound class - references a SfmlSoundBuffer, does not own it.
 *        Deleting the sound never touches the buffer.
 */
class SfmlSound : public audio::ISound {

    public:
        SfmlSound(SfmlSoundBuffer &buffer) : _buffer(buffer), _sound(buffer.handle()) {
            _sound.setVolume(100.f);
        }

        ~SfmlSound() override = default;

        bool isReady() const override {
            return _buffer.isReady();
        }

        void play() override {
            //sfml already restarts a sound that's already playing - matches
            //the contract's documented play() semantics natively
            _sound.play();
        }

        void pause() override {
            _sound.pause();
        }

        void stop() override {
            _sound.stop();
        }

        void setVolume(float volume) override {
            _sound.setVolume(volume * 100.f);
        }

        float getVolume() const override {
            return _sound.getVolume() * 0.01f;
        }

        //sfml (OpenAL) genuinely supports positional audio, unlike raylib
        void setPosition(Vector3f position) override {
            _position = position;
            _sound.setPosition({static_cast<float>(position.x), static_cast<float>(position.y), static_cast<float>(position.z)});
        }
        Vector3f getPosition() const override {
            return _position;
        }

        //sfml has no velocity/doppler API on sf::Sound : graceful no-op
        void setVelocity(Vector3f velocity) override {
            _velocity = velocity;
        }
        Vector3f getVelocity() const override {
            return _velocity;
        }

    private:
        SfmlSoundBuffer &_buffer;
        sf::Sound _sound;
        Vector3f _position{};
        Vector3f _velocity{};
};

/** @} */

#endif /* !SFMLSOUND_HPP_ */
