/**
 * @file SfmlKeyboard.hpp
 * @author Perry Chouteau (perry.chouteau@outlook.com)
 * @brief
 * @date 2026-08-10
 *
 * @addtogroup sfml
 * @{
 */

#ifndef SFMLKEYBOARD_HPP_
#define SFMLKEYBOARD_HPP_

//Sfml
#include <SFML/Window.hpp>

//Interface
#include "IKeyboard.hpp"

#include <vector>
#include <unordered_map>

class SfmlWindow;

/**
 * @brief Maps the contract's keys to sfml scancodes, and reads the state
 *        the window accumulated while popping its events.
 *
 * The window is handed over at construction (the module passes it), so
 * the link can never be missing. The methods are defined in
 * SfmlWindow.hpp, once SfmlWindow is complete.
 *
 * The reference is CONST : querying the keyboard cannot consume anything,
 * and the compiler guarantees it. Only pollEvent() and endDraw() mutate, on
 * the window side. That is what lets an arcade and the game running inside
 * it read the same key in the same frame.
 *
 * Modifiers are keys like any other : Shift+E is isKeyDown(KEY_LEFT_SHIFT)
 * and isKeyPressed(KEY_E), composed by the caller.
 */
class SfmlKeyboard : public graphic::IKeyboard {

    public:
        SfmlKeyboard(const SfmlWindow &window) : _window(window) {}

        ~SfmlKeyboard() override = default;

        std::vector<Keys> whichKeyDown() const override;
        bool isKeyPressed(Keys key) const override;
        bool isKeyReleased(Keys key) const override;
        bool isKeyDown(Keys key) const override;
        bool isKeyUp(Keys key) const override;

    private:
        const SfmlWindow &_window;

        const std::unordered_map<graphic::IKeyboard::Keys, sf::Keyboard::Scancode> _keys = {
            //line 0
            {graphic::IKeyboard::Keys::KEY_ESCAPE, sf::Keyboard::Scancode::Escape},
            {graphic::IKeyboard::Keys::KEY_F1, sf::Keyboard::Scancode::F1},
            {graphic::IKeyboard::Keys::KEY_F2, sf::Keyboard::Scancode::F2},
            {graphic::IKeyboard::Keys::KEY_F3, sf::Keyboard::Scancode::F3},
            {graphic::IKeyboard::Keys::KEY_F4, sf::Keyboard::Scancode::F4},
            {graphic::IKeyboard::Keys::KEY_F5, sf::Keyboard::Scancode::F5},
            {graphic::IKeyboard::Keys::KEY_F6, sf::Keyboard::Scancode::F6},
            {graphic::IKeyboard::Keys::KEY_F7, sf::Keyboard::Scancode::F7},
            {graphic::IKeyboard::Keys::KEY_F8, sf::Keyboard::Scancode::F8},
            {graphic::IKeyboard::Keys::KEY_F9, sf::Keyboard::Scancode::F9},
            {graphic::IKeyboard::Keys::KEY_F10, sf::Keyboard::Scancode::F10},
            {graphic::IKeyboard::Keys::KEY_F11, sf::Keyboard::Scancode::F11},
            {graphic::IKeyboard::Keys::KEY_F12, sf::Keyboard::Scancode::F12},

            //line 1
            {graphic::IKeyboard::Keys::KEY_1, sf::Keyboard::Scancode::Num1},
            {graphic::IKeyboard::Keys::KEY_2, sf::Keyboard::Scancode::Num2},
            {graphic::IKeyboard::Keys::KEY_3, sf::Keyboard::Scancode::Num3},
            {graphic::IKeyboard::Keys::KEY_4, sf::Keyboard::Scancode::Num4},
            {graphic::IKeyboard::Keys::KEY_5, sf::Keyboard::Scancode::Num5},
            {graphic::IKeyboard::Keys::KEY_6, sf::Keyboard::Scancode::Num6},
            {graphic::IKeyboard::Keys::KEY_7, sf::Keyboard::Scancode::Num7},
            {graphic::IKeyboard::Keys::KEY_8, sf::Keyboard::Scancode::Num8},
            {graphic::IKeyboard::Keys::KEY_9, sf::Keyboard::Scancode::Num9},
            {graphic::IKeyboard::Keys::KEY_0, sf::Keyboard::Scancode::Num0},
            {graphic::IKeyboard::Keys::KEY_MINUS, sf::Keyboard::Scancode::Hyphen},
            {graphic::IKeyboard::Keys::KEY_EQUAL, sf::Keyboard::Scancode::Equal},
            {graphic::IKeyboard::Keys::KEY_BACKSPACE, sf::Keyboard::Scancode::Backspace},

            //line 2
            {graphic::IKeyboard::Keys::KEY_TAB, sf::Keyboard::Scancode::Tab},
            {graphic::IKeyboard::Keys::KEY_Q, sf::Keyboard::Scancode::Q},
            {graphic::IKeyboard::Keys::KEY_W, sf::Keyboard::Scancode::W},
            {graphic::IKeyboard::Keys::KEY_E, sf::Keyboard::Scancode::E},
            {graphic::IKeyboard::Keys::KEY_R, sf::Keyboard::Scancode::R},
            {graphic::IKeyboard::Keys::KEY_T, sf::Keyboard::Scancode::T},
            {graphic::IKeyboard::Keys::KEY_Y, sf::Keyboard::Scancode::Y},
            {graphic::IKeyboard::Keys::KEY_U, sf::Keyboard::Scancode::U},
            {graphic::IKeyboard::Keys::KEY_I, sf::Keyboard::Scancode::I},
            {graphic::IKeyboard::Keys::KEY_O, sf::Keyboard::Scancode::O},
            {graphic::IKeyboard::Keys::KEY_P, sf::Keyboard::Scancode::P},
            {graphic::IKeyboard::Keys::KEY_LEFT_BRACKET, sf::Keyboard::Scancode::LBracket},
            {graphic::IKeyboard::Keys::KEY_RIGHT_BRACKET, sf::Keyboard::Scancode::RBracket},

            //line 3
            {graphic::IKeyboard::Keys::KEY_CAPS_LOCK, sf::Keyboard::Scancode::CapsLock},
            {graphic::IKeyboard::Keys::KEY_LEFT_CONTROL, sf::Keyboard::Scancode::LControl},
            {graphic::IKeyboard::Keys::KEY_A, sf::Keyboard::Scancode::A},
            {graphic::IKeyboard::Keys::KEY_S, sf::Keyboard::Scancode::S},
            {graphic::IKeyboard::Keys::KEY_D, sf::Keyboard::Scancode::D},
            {graphic::IKeyboard::Keys::KEY_F, sf::Keyboard::Scancode::F},
            {graphic::IKeyboard::Keys::KEY_G, sf::Keyboard::Scancode::G},
            {graphic::IKeyboard::Keys::KEY_H, sf::Keyboard::Scancode::H},
            {graphic::IKeyboard::Keys::KEY_J, sf::Keyboard::Scancode::J},
            {graphic::IKeyboard::Keys::KEY_K, sf::Keyboard::Scancode::K},
            {graphic::IKeyboard::Keys::KEY_L, sf::Keyboard::Scancode::L},
            {graphic::IKeyboard::Keys::KEY_SEMICOLON, sf::Keyboard::Scancode::Semicolon},
            {graphic::IKeyboard::Keys::KEY_APOSTROPHE, sf::Keyboard::Scancode::Apostrophe},
            {graphic::IKeyboard::Keys::KEY_BACKTICK, sf::Keyboard::Scancode::Grave},
            {graphic::IKeyboard::Keys::KEY_ENTER, sf::Keyboard::Scancode::Enter},

            //line 4
            {graphic::IKeyboard::Keys::KEY_LEFT_SHIFT, sf::Keyboard::Scancode::LShift},
            {graphic::IKeyboard::Keys::KEY_BACKSLASH, sf::Keyboard::Scancode::Backslash},
            {graphic::IKeyboard::Keys::KEY_Z, sf::Keyboard::Scancode::Z},
            {graphic::IKeyboard::Keys::KEY_X, sf::Keyboard::Scancode::X},
            {graphic::IKeyboard::Keys::KEY_C, sf::Keyboard::Scancode::C},
            {graphic::IKeyboard::Keys::KEY_V, sf::Keyboard::Scancode::V},
            {graphic::IKeyboard::Keys::KEY_B, sf::Keyboard::Scancode::B},
            {graphic::IKeyboard::Keys::KEY_N, sf::Keyboard::Scancode::N},
            {graphic::IKeyboard::Keys::KEY_M, sf::Keyboard::Scancode::M},
            {graphic::IKeyboard::Keys::KEY_COMMA, sf::Keyboard::Scancode::Comma},
            {graphic::IKeyboard::Keys::KEY_PERIOD, sf::Keyboard::Scancode::Period},
            {graphic::IKeyboard::Keys::KEY_SLASH, sf::Keyboard::Scancode::Slash},
            {graphic::IKeyboard::Keys::KEY_RIGHT_SHIFT, sf::Keyboard::Scancode::RShift},

            //line 5
            {graphic::IKeyboard::Keys::KEY_LEFT_ALT, sf::Keyboard::Scancode::LAlt},
            {graphic::IKeyboard::Keys::KEY_LEFT_SUPER, sf::Keyboard::Scancode::LSystem},
            {graphic::IKeyboard::Keys::KEY_SPACE, sf::Keyboard::Scancode::Space},
            {graphic::IKeyboard::Keys::KEY_RIGHT_ALT, sf::Keyboard::Scancode::RAlt},
            {graphic::IKeyboard::Keys::KEY_RIGHT_SUPER, sf::Keyboard::Scancode::RSystem},
            {graphic::IKeyboard::Keys::KEY_RIGHT_CONTROL, sf::Keyboard::Scancode::RControl},

            //keypad
            {graphic::IKeyboard::Keys::KEY_PAD_0, sf::Keyboard::Scancode::Numpad0},
            {graphic::IKeyboard::Keys::KEY_PAD_1, sf::Keyboard::Scancode::Numpad1},
            {graphic::IKeyboard::Keys::KEY_PAD_2, sf::Keyboard::Scancode::Numpad2},
            {graphic::IKeyboard::Keys::KEY_PAD_3, sf::Keyboard::Scancode::Numpad3},
            {graphic::IKeyboard::Keys::KEY_PAD_4, sf::Keyboard::Scancode::Numpad4},
            {graphic::IKeyboard::Keys::KEY_PAD_5, sf::Keyboard::Scancode::Numpad5},
            {graphic::IKeyboard::Keys::KEY_PAD_6, sf::Keyboard::Scancode::Numpad6},
            {graphic::IKeyboard::Keys::KEY_PAD_7, sf::Keyboard::Scancode::Numpad7},
            {graphic::IKeyboard::Keys::KEY_PAD_8, sf::Keyboard::Scancode::Numpad8},
            {graphic::IKeyboard::Keys::KEY_PAD_9, sf::Keyboard::Scancode::Numpad9},
            {graphic::IKeyboard::Keys::KEY_PAD_MINUS, sf::Keyboard::Scancode::NumpadMinus},
            {graphic::IKeyboard::Keys::KEY_PAD_PLUS, sf::Keyboard::Scancode::NumpadPlus},
            {graphic::IKeyboard::Keys::KEY_PAD_DOT, sf::Keyboard::Scancode::NumpadDecimal},
            {graphic::IKeyboard::Keys::KEY_PAD_ENTER, sf::Keyboard::Scancode::NumpadEnter},

            //arrows
            {graphic::IKeyboard::Keys::KEY_UP, sf::Keyboard::Scancode::Up},
            {graphic::IKeyboard::Keys::KEY_DOWN, sf::Keyboard::Scancode::Down},
            {graphic::IKeyboard::Keys::KEY_LEFT, sf::Keyboard::Scancode::Left},
            {graphic::IKeyboard::Keys::KEY_RIGHT, sf::Keyboard::Scancode::Right},
        };
};

/** @} */

#endif /* !SFMLKEYBOARD_HPP_ */
