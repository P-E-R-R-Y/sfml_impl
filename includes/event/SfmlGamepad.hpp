/**
 * @file SfmlGamepad.hpp
 * @author Perry Chouteau (perry.chouteau@outlook.com)
 * @brief
 * @date 2026-08-10
 *
 * @addtogroup sfml
 * @{
 */

#ifndef SFMLGAMEPAD_HPP_
#define SFMLGAMEPAD_HPP_

//Sfml
#include <SFML/Window.hpp>

//Interface
#include "IGamepad.hpp"

#include <unordered_map>

class SfmlWindow;

/**
 * @brief Unlike raylib's GamepadButton (normalized Xbox-style layout),
 *        sf::Joystick exposes raw numbered buttons/axes with no guaranteed
 *        meaning across controllers/platforms. This uses the common Xbox-
 *        style button ordering (0=A,1=B,2=X,3=Y) most drivers report, but
 *        it isn't guaranteed the way raylib's mapping is.
 *
 * Axes are reported as a percentage in [-100, 100] : divided by 100 to
 * yield the contract's [-1, 1], with no dead zone - a dead zone is a
 * gameplay choice and belongs to the game.
 */
class SfmlGamepad : public graphic::IGamepad {

    public:
        SfmlGamepad(const SfmlWindow &window, unsigned int index = 0) : _window(window), _index(index) {}

        ~SfmlGamepad() override = default;

        bool isAvailable() const override {
            return sf::Joystick::isConnected(_index);
        }

        // defined in SfmlWindow.hpp, once SfmlWindow is complete
        bool isButtonPressed(Button button) const override;
        bool isButtonReleased(Button button) const override;
        bool isButtonDown(Button button) const override;
        bool isButtonUp(Button button) const override;

        float getAxisMovement(Axis axis) const override {
            //sf::Joystick reports axes as a percentage in [-100, 100]
            return sf::Joystick::getAxisPosition(_index, _axes.at(axis)) / 100.f;
        }

    private:
        const SfmlWindow &_window;
        unsigned int _index;

        const std::unordered_map<IGamepad::Button, unsigned int> _buttons = {
            {IGamepad::BUTTON_A, 0},
            {IGamepad::BUTTON_B, 1},
            {IGamepad::BUTTON_X, 2},
            {IGamepad::BUTTON_Y, 3},
            {IGamepad::BUTTON_LEFT_BUMPER, 4},
            {IGamepad::BUTTON_RIGHT_BUMPER, 5},
            {IGamepad::BUTTON_BACK, 6},
            {IGamepad::BUTTON_START, 7},
            {IGamepad::BUTTON_LEFT_THUMB, 8},
            {IGamepad::BUTTON_RIGHT_THUMB, 9},
            //no standard raw button index for a d-pad, most drivers expose it as a POV hat
            {IGamepad::BUTTON_DPAD_UP, 10},
            {IGamepad::BUTTON_DPAD_RIGHT, 11},
            {IGamepad::BUTTON_DPAD_DOWN, 12},
            {IGamepad::BUTTON_DPAD_LEFT, 13},
        };

        const std::unordered_map<IGamepad::Axis, sf::Joystick::Axis> _axes = {
            {IGamepad::AXIS_LEFT_X, sf::Joystick::Axis::X},
            {IGamepad::AXIS_LEFT_Y, sf::Joystick::Axis::Y},
            {IGamepad::AXIS_RIGHT_X, sf::Joystick::Axis::U},
            {IGamepad::AXIS_RIGHT_Y, sf::Joystick::Axis::V},
            {IGamepad::AXIS_LEFT_TRIGGER, sf::Joystick::Axis::Z},
            {IGamepad::AXIS_RIGHT_TRIGGER, sf::Joystick::Axis::R},
        };
};

/** @} */

#endif /* !SFMLGAMEPAD_HPP_ */
