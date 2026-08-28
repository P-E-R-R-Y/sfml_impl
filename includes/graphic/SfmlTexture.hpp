/**
 * @file SfmlTexture.hpp
 * @author Perry Chouteau (perry.chouteau@outlook.com)
 * @brief
 * @date 2026-08-10
 *
 * @addtogroup sfml
 * @{
 */

#ifndef SFMLTEXTURE_HPP_
#define SFMLTEXTURE_HPP_

//Sfml
#include <SFML/Graphics.hpp>

//Interface
#include "ITexture.hpp"

#include <iostream>
#include <optional>

/**
 * @brief Sfml Texture class - owns the loaded texture. SfmlSprite only
 *        references it (sf::Sprite itself already takes a const
 *        sf::Texture& natively, this contract matches sfml's own model).
 */
class SfmlTexture : public graphic::ITexture {

    public:
        SfmlTexture(std::string path) {
            try {
                _texture.emplace(path);
            } catch (const sf::Exception &e) {
                std::cerr << "Failed to load texture: " << e.what() << std::endl;
            }
        }

        ~SfmlTexture() = default;

        bool isReady() const override {
            return _texture.has_value();
        }

        Vector2f getSize() const override {
            if (!_texture.has_value())
                return {};
            sf::Vector2u size = _texture->getSize();
            return {static_cast<double>(size.x), static_cast<double>(size.y)};
        }

        const sf::Texture &handle() const {
            return _texture.value();
        }

    private:
        std::optional<sf::Texture> _texture;
};

/** @} */

#endif /* !SFMLTEXTURE_HPP_ */
