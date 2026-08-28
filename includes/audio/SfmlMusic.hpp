/**
 * @file SfmlMusic.hpp
 * @author Perry Chouteau (perry.chouteau@outlook.com)
 * @brief
 * @date 2026-08-10
 *
 * @addtogroup sfml
 * @{
 */

#ifndef SFMLMUSIC_HPP_
#define SFMLMUSIC_HPP_

//Sfml
#include <SFML/Audio.hpp>

//Interface
#include "IMusic.hpp"

/**
 * @brief sfml music stream.
 *
 * sf::Music already carries the three states (stopped/playing/paused), so
 * play()/pause()/stop() just guard on getStatus() - raylib, by contrast,
 * had to track an enum by hand. setTime() clamps the position, sfml
 * falling back to zero past the end.
 */
class SfmlMusic : public audio::IMusic {

    public:
        SfmlMusic(std::string path) : _music(path) {
            _music.setVolume(100.f);
        }

        ~SfmlMusic() override = default;

        bool isReady() const override {
            return true;
        }

        // play/pause/stop do ONLY what they say. sf::Music already carries
        // the three states, so it is enough not to restart a live stream.
        void play() override {
            if (_music.getStatus() == sf::Music::Status::Playing)
                return;
            //resumes if paused, restarts from zero if stopped
            _music.play();
        }

        void pause() override {
            if (_music.getStatus() != sf::Music::Status::Playing)
                return;
            _music.pause();
        }

        void stop() override {
            if (_music.getStatus() == sf::Music::Status::Stopped)
                return;
            _music.stop();
        }

        void update() override {
        }

        void setVolume(float volume) override {
            _music.setVolume(volume * 100.f);
        }

        float getVolume() const override {
            return _music.getVolume() * 0.01f;
        }

        void setLoop(bool loop) override {
            _music.setLooping(loop);
        }

        bool getLoop() const override {
            return _music.isLooping();
        }

        void setTime(float position) override {
            _music.setPlayingOffset(sf::seconds(clampTime(position)));
        }

        float getTime() const override {
            return _music.getPlayingOffset().asSeconds();
        }

        float getLength() const override {
            return _music.getDuration().asSeconds();
        }

        //sfml (OpenAL) genuinely supports positional audio for streams too
        void setPosition(Vector3f position) override {
            _position = position;
            _music.setPosition({static_cast<float>(position.x), static_cast<float>(position.y), static_cast<float>(position.z)});
        }
        Vector3f getPosition() const override {
            return _position;
        }

        //sfml has no velocity/doppler API on sf::Music : graceful no-op
        void setVelocity(Vector3f velocity) override {
            _velocity = velocity;
        }
        Vector3f getVelocity() const override {
            return _velocity;
        }

    private:
        float clampTime(float position) const {
            const float length = getLength();

            if (position < 0.f) return 0.f;
            return (position > length) ? length : position;
        }

        sf::Music _music;
        Vector3f _position{};
        Vector3f _velocity{};
};

/** @} */

#endif /* !SFMLMUSIC_HPP_ */
