/**
 * @file KeyNames.hpp
 * @brief Nom lisible de chaque touche, pour l'affichage des exemples.
 *
 * IKeyboard::Keys est contigu de 0 (KEY_ESCAPE) a KEY_LENGTH, donc un
 * simple tableau indexe suffit - pas de map a construire au demarrage.
 */

#ifndef KEYNAMES_HPP_
#define KEYNAMES_HPP_

#include "IKeyboard.hpp"

inline const char *keyName(graphic::IKeyboard::Keys key) {
    static const char *names[] = {
        "ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "BACKSPACE",
        "TAB", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]",
        "CAPS", "LCTRL", "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", "ENTER",
        "LSHIFT", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", "PERIOD", ".", "/", "RSHIFT",
        "LALT", "LSUPER", "SPACE", "RALT", "RSUPER", "RCTRL",
        "PAD0", "PAD1", "PAD2", "PAD3", "PAD4", "PAD5", "PAD6", "PAD7", "PAD8", "PAD9",
        "PAD-", "PAD+", "PAD.", "PADENTER",
        "UP", "DOWN", "LEFT", "RIGHT",
    };
    const int count = static_cast<int>(sizeof(names) / sizeof(names[0]));

    if (key < 0 || static_cast<int>(key) >= count)
        return "?";
    return names[static_cast<int>(key)];
}

#endif /* !KEYNAMES_HPP_ */
