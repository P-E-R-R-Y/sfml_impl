/**
 * @file SfmlFont.hpp
 * @author Perry Chouteau (perry.chouteau@outlook.com)
 * @brief
 * @date 2026-08-10
 *
 * @addtogroup sfml
 * @{
 */

#ifndef SFMLFONT_HPP_
#define SFMLFONT_HPP_

//Sfml
#include <SFML/Graphics.hpp>

//Interface
#include "IFont.hpp"

#include <iostream>
#include <optional>

/**
 * @brief Sfml Font class - owns the loaded font. SfmlText only references
 *        it (sf::Text itself already takes a const sf::Font& natively).
 */
class SfmlFont : public graphic::IFont {

    public:
        SfmlFont(std::string path) {
            try {
                _font.emplace(path);
            } catch (const sf::Exception &e) {
                std::cerr << "Failed to load font: " << e.what() << std::endl;
            }
        }

        ~SfmlFont() = default;

        bool isReady() const override {
            return _font.has_value();
        }

        const sf::Font &handle() const {
            return _font.value();
        }

    private:
        std::optional<sf::Font> _font;
};

/** @} */

#endif /* !SFMLFONT_HPP_ */
