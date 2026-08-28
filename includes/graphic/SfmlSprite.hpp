/**
 * @file SfmlSprite.hpp
 * @author Perry Chouteau (perry.chouteau@outlook.com)
 * @brief
 * @date 2026-08-10
 *
 * @addtogroup sfml
 * @{
 */

#ifndef SFMLSPRITE_HPP_
#define SFMLSPRITE_HPP_

//Sfml
#include <SFML/Graphics.hpp>

//Interface
#include "ISprite.hpp"

//encapsulation
#include "SfmlTexture.hpp"

/**
 * @brief Sfml Sprite class - references a SfmlTexture, does not own it.
 *        Deleting the sprite never touches the texture.
 */
class SfmlSprite : public graphic::ISprite {

    public:
        SfmlSprite(SfmlTexture &texture) : _texture(texture), _sprite(texture.handle()) {
        }

        ~SfmlSprite() override = default;

        bool isReady() const override {
            return _texture.isReady();
        }

        Rect<float> getBounds() const override {
            sf::FloatRect rect = _sprite.getGlobalBounds();
            return {rect.position.x, rect.position.y, rect.size.x, rect.size.y};
        }

        void setCrop(Rect<float> rect) override {
            _sprite.setTextureRect(sf::IntRect{
                sf::Vector2i{static_cast<int32_t>(rect.x), static_cast<int32_t>(rect.y)},
                sf::Vector2i{static_cast<int32_t>(rect.w), static_cast<int32_t>(rect.h)}});
        }

        Vector2f getPosition() const override {
            sf::Vector2f pos = _sprite.getPosition();
            return {static_cast<double>(pos.x), static_cast<double>(pos.y)};
        }
        void setPosition(Vector2f position) override {
            _sprite.setPosition({static_cast<float>(position.x), static_cast<float>(position.y)});
        }

        float getRotation() const override {
            return _sprite.getRotation().asDegrees();
        }

        void setRotation(float angle, bool isRad = false) override {
            if (isRad)
                _sprite.setRotation(sf::radians(angle));
            else
                _sprite.setRotation(sf::degrees(angle));
        }

        Vector2f getSize() const override {
            sf::Vector2f scale = _sprite.getScale();
            sf::Vector2f size = _sprite.getLocalBounds().size;
            return {static_cast<double>(size.x * scale.x), static_cast<double>(size.y * scale.y)};
        }

        void setSize(Vector2f size) override {
            sf::Vector2f bounds = _sprite.getLocalBounds().size;
            if (bounds.x == 0 || bounds.y == 0) return;
            _sprite.setScale({
                static_cast<float>(size.x / bounds.x),
                static_cast<float>(size.y / bounds.y)
            });
        }

        friend class SfmlWindow;

    private:
        SfmlTexture &_texture;
        sf::Sprite _sprite;
};

/** @} */

#endif /* !SFMLSPRITE_HPP_ */
