/**
 * @file SfmlText.hpp
 * @author Perry Chouteau (perry.chouteau@outlook.com)
 * @brief
 * @date 2026-08-10
 *
 * @addtogroup sfml
 * @{
 */

#ifndef SFMLTEXT_HPP_
#define SFMLTEXT_HPP_

//Sfml
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

//Interface
#include "IText.hpp"

//encapsulation
#include "SfmlFont.hpp"

/**
 * @brief Sfml Text class - references a SfmlFont, does not own it.
 *        Deleting the text never touches the font.
 */
class SfmlText : public graphic::IText {

    public:
        SfmlText(std::string data, SfmlFont &font) : _font(&font), _text(font.handle()) {
            _text.setString(data);
        }

        ~SfmlText() override = default;

        bool isReady() const override {
            return _font->isReady();
        }

        void setText(const std::string text) override {
            _text.setString(text);
        }

        std::string getText() const override {
            return _text.getString();
        }

        void setFont(graphic::IFont *font) override {
            _font = static_cast<SfmlFont *>(font);
            _text.setFont(_font->handle());
        }

        void setFontSize(unsigned int size) override {
            _text.setCharacterSize(size);
        }
        unsigned int getFontSize() const override {
            return _text.getCharacterSize();
        }

        void setTextColor(Color color) override {
            _text.setFillColor(sf::Color(color.r, color.g, color.b, color.a));
        }

        Color getTextColor() const override {
            const sf::Color color = _text.getFillColor();
            return Color{color.r, color.g, color.b, color.a};
        }

        void setPosition(Vector2f position) override {
            _text.setPosition({static_cast<float>(position.x), static_cast<float>(position.y)});
        }
        Vector2f getPosition() const override {
            const sf::Vector2f pos = _text.getPosition();
            return Vector2f{static_cast<double>(pos.x), static_cast<double>(pos.y)};
        }

        void setRotation(float angle) override {
            _text.setRotation(sf::degrees(angle));
        }

        float getRotation() const override {
            return _text.getRotation().asDegrees();
        }

        friend class SfmlWindow;

    private:
        SfmlFont *_font;
        sf::Text _text;
};

/** @} */

#endif /* !SFMLTEXT_HPP_ */
