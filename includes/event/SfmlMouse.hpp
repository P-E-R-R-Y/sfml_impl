/**
 * @file SfmlMouse.hpp
 * @author Perry Chouteau (perry.chouteau@outlook.com)
 * @brief
 * @date 2026-08-10
 *
 * @addtogroup sfml
 * @{
 */

#ifndef SFMLMOUSE_HPP_
#define SFMLMOUSE_HPP_

//Sfml
#include <SFML/Window.hpp>

//Interface
#include "IMouse.hpp"

#include <unordered_map>

class SfmlWindow;

/**
 * @brief Maps the contract's buttons to sfml buttons, and reads the state
 *        the window accumulated while popping its events. Definitions in
 *        SfmlWindow.hpp, once SfmlWindow is complete.
 */
class SfmlMouse : public graphic::IMouse {

    public:
        SfmlMouse(SfmlWindow &window) : _window(window) {}

        ~SfmlMouse() override = default;

        bool isButtonPressed(Buttons key) const override;
        bool isButtonReleased(Buttons key) const override;
        bool isButtonDown(Buttons key) const override;
        bool isButtonUp(Buttons key) const override;

        // window-relative, like raylib - the contract mandates it, otherwise
        // no hit-test written against it would be portable
        Vector2f getPosition() const override;
        void setPosition(Vector2f position) override;

        float GetMouseWheelMove() const override;

    private:
        SfmlWindow &_window;

        const std::unordered_map<Buttons, sf::Mouse::Button> _buttons = {
            { Buttons::BUTTON_LEFT, sf::Mouse::Button::Left },
            { Buttons::BUTTON_RIGHT, sf::Mouse::Button::Right },
            { Buttons::BUTTON_MIDDLE, sf::Mouse::Button::Middle },

            { Buttons::EXTRA_BUTTON_1, sf::Mouse::Button::Extra1 },
            { Buttons::EXTRA_BUTTON_2, sf::Mouse::Button::Extra2 },
        };
};

/** @} */

#endif /* !SFMLMOUSE_HPP_ */
