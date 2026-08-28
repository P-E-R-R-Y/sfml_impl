/**
 * @file SfmlSoundBuffer.hpp
 * @author Perry Chouteau (perry.chouteau@outlook.com)
 * @brief
 * @date 2026-08-10
 *
 * @addtogroup sfml
 * @{
 */

#ifndef SFMLSOUNDBUFFER_HPP_
#define SFMLSOUNDBUFFER_HPP_

//Sfml
#include <SFML/Audio.hpp>

//Interface
#include "ISoundBuffer.hpp"

#include <iostream>
#include <optional>

/**
 * @brief Sfml SoundBuffer class - owns the loaded samples. SfmlSound only
 *        references it (sf::Sound itself already takes a const
 *        sf::SoundBuffer& natively, this contract matches sfml's own model).
 */
class SfmlSoundBuffer : public audio::ISoundBuffer {

    public:
        SfmlSoundBuffer(std::string path) {
            try {
                _buffer.emplace(path);
            } catch (const sf::Exception &e) {
                std::cerr << "Failed to load sound buffer: " << e.what() << std::endl;
            }
        }

        ~SfmlSoundBuffer() = default;

        bool isReady() const override {
            return _buffer.has_value();
        }

        float getLength() const override {
            return _buffer.has_value() ? _buffer->getDuration().asSeconds() : 0.f;
        }

        const sf::SoundBuffer &handle() const {
            return _buffer.value();
        }

    private:
        std::optional<sf::SoundBuffer> _buffer;
};

/** @} */

#endif /* !SFMLSOUNDBUFFER_HPP_ */
