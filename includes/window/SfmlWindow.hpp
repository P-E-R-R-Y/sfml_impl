/**
 * @file SfmlWindow.hpp
 * @author Perry Chouteau (perry.chouteau@outlook.com)
 * @brief
 * @date 2026-08-10
 *
 * @addtogroup sfml
 * @{
 */

#ifndef SFMLWINDOW_HPP_
#define SFMLWINDOW_HPP_

//Sfml
#include <SFML/Graphics.hpp>

//Interface
#include "IWindow2.hpp"

//encapsulation
#include "SfmlGamepad.hpp"
#include "SfmlKeyboard.hpp"
#include "SfmlMouse.hpp"
#include "SfmlPolygon.hpp"
#include "SfmlSprite.hpp"
#include "SfmlText.hpp"

#include <array>
#include <optional>
#include <vector>

/**
 * @brief Sfml Window class - implements IWindow2 only, sfml has no 3D.
 *
 * All input state is rebuilt from the window's own events, never through a
 * global OS query : sf::Keyboard::isKeyPressed() would need the "Input
 * Monitoring" permission on macOS, while events reach the focused window
 * normally.
 *
 * sfml has a queue and raylib has not, so this is where the two are made to
 * agree : pollEvent() drains the whole queue into _events and folds it into
 * state, endDraw() drops it. Between the two, every read is idempotent -
 * which is what lets an arcade and the game running inside it read the same
 * key in the same frame without either one stealing it.
 */
class SfmlWindow : public graphic::IWindow2 {

    public:
        SfmlWindow(int32_t screenWidth, int32_t screenHeight, std::string title)
            : _window(sf::VideoMode({static_cast<uint32_t>(screenWidth), static_cast<uint32_t>(screenHeight)}), title) {
            // without this, a held key re-sends KeyPressed in bursts and
            // isKeyPressed() would fire on every OS repeat.
            _window.setKeyRepeatEnabled(false);
            syncView();
        }

        ~SfmlWindow() = default;

        //lifecycle
        bool isOpen() override {
            return _window.isOpen();
        }

        void close() override {
            _window.close();
        }

        Vector2f getPosition() override {
            const sf::Vector2i position = _window.getPosition();
            return {static_cast<double>(position.x), static_cast<double>(position.y)};
        }

        void setPosition(Vector2f position) override {
            _window.setPosition({static_cast<int>(position.x), static_cast<int>(position.y)});
        }

        Vector2f getSize() override {
            const sf::Vector2u size = _window.getSize();
            return {static_cast<double>(size.x), static_cast<double>(size.y)};
        }

        void setSize(Vector2f size) override {
            _window.setSize({static_cast<unsigned>(size.x), static_cast<unsigned>(size.y)});
            /* setSize() n'emet pas forcement de Resized selon la plateforme,
             * donc on recale la vue tout de suite plutot que d'attendre un
             * evenement qui ne viendra peut-etre pas. */
            syncView();
        }

        void setFrameLimit(int32_t limit) override {
            _window.setFramerateLimit(limit);
        }

        int32_t getDelta() override {
            return static_cast<int32_t>(_deltaTime.asMilliseconds());
        }

        /**
         * @brief Drains the whole queue into _events and folds it into
         *        state. Called again in the same frame it finds the native
         *        queue empty, appends nothing, and answers the same thing.
         */
        bool pollEvent() override {
            while (const std::optional<sf::Event> event = _window.pollEvent()) {
                _events.push_back(*event);
                feedEvent(*event);
            }
            return !_events.empty();
        }

        void eventClose() override {
            for (const auto &event : _events) {
                const auto *pressed = event.getIf<sf::Event::KeyPressed>();

                if (event.is<sf::Event::Closed>() ||
                    (pressed && pressed->scancode == sf::Keyboard::Scancode::Escape)) {
                    _window.close();
                    return;
                }
            }
        }


        //2D
        void beginDraw() override {
            _window.clear();
        }

        void endDraw() override {
            _window.display();
            _deltaTime = _deltaClock.restart();
            _events.clear();   // la frontiere de frame, cf. IWindow::pollEvent
        }

        void drawPoly(graphic::IPolygon *polygon) override;
        void drawSprite(graphic::ISprite *sprite) override;
        void drawText(graphic::IText *text) override;

        friend class SfmlKeyboard;
        friend class SfmlMouse;
        friend class SfmlGamepad;

    private:
        /* Input state rebuilt FROM THE EVENTS, never through a global
         * query like sf::Keyboard::isKeyPressed() : on macOS that one
         * requires the "Input Monitoring" permission, while events are
         * delivered normally to the focused window.
         *
         * Private : only SfmlKeyboard/Mouse/Gamepad reach it, as friends.
         * Nothing from sf:: leaks into the window's public API, which
         * exposes the IWindow2 contract and nothing else. */
        /**
         * @brief Remet la vue a l'echelle 1:1 sur la surface courante.
         *
         * Sans ca, agrandir la fenetre ETIRE la vue par defaut : le dessin
         * part dans un repere mis a l'echelle pendant que MouseMoved
         * continue de rendre des pixels. Les deux ne coincident plus, et un
         * bouton n'est plus la ou on le voit.
         *
         * Une unite de vue = un pixel, donc getPosition() de la souris et
         * les coordonnees de dessin restent le meme repere quelle que soit
         * la taille. raylib n'a pas ce probleme : il dessine directement en
         * pixels ecran.
         */
        void syncView() {
            const sf::Vector2f size(_window.getSize());

            _window.setView(sf::View(sf::FloatRect({0.f, 0.f}, size)));
        }

        static size_t index(sf::Keyboard::Scancode code) {
            const auto raw = static_cast<int>(code);
            return (raw < 0) ? 0 : static_cast<size_t>(raw);
        }

        /* Only what isKeyDown/isKeyUp need is tracked here : a key stays
         * "down" between its KeyPressed and its KeyReleased, which no
         * single event can tell. The fronts need no state - they are read
         * back off _events, which holds the frame.
         *
         * sf::Keyboard::isKeyPressed() would do the same in one line, but
         * it is a GLOBAL query : on macOS it requires the "Input
         * Monitoring" permission, while events reach the focused window
         * normally. */
        void feedEvent(const sf::Event &event) {
            if (const auto *pressed = event.getIf<sf::Event::KeyPressed>())
                _keysDown[index(pressed->scancode)] = true;
            else if (const auto *released = event.getIf<sf::Event::KeyReleased>())
                _keysDown[index(released->scancode)] = false;
            else if (const auto *down = event.getIf<sf::Event::MouseButtonPressed>())
                _mouseDown[size_t(down->button)] = true;
            else if (const auto *up = event.getIf<sf::Event::MouseButtonReleased>())
                _mouseDown[size_t(up->button)] = false;
            else if (const auto *moved = event.getIf<sf::Event::MouseMoved>())
                _mousePosition = moved->position;   // deja relative a la fenetre
            else if (event.is<sf::Event::Resized>())
                syncView();                         // sinon le dessin se decale de la souris
            else if (event.is<sf::Event::FocusLost>()) {
                // without this, a key released out of focus stays "down"
                _keysDown.fill(false);
                _mouseDown.fill(false);
            }
        }

        sf::RenderWindow _window;

        /* Les evenements de la frame. Rempli par pollEvent(), vide par
         * endDraw() : entre les deux, tout le monde y lit la meme chose. */
        std::vector<sf::Event> _events;

        sf::Clock _deltaClock;
        sf::Time _deltaTime;

        std::array<bool, sf::Keyboard::ScancodeCount> _keysDown{};

        std::array<bool, sf::Mouse::ButtonCount> _mouseDown{};

        sf::Vector2i _mousePosition{};
};

void SfmlWindow::drawPoly(graphic::IPolygon *polygon) {
    SfmlPolygon *sfmlPolygon = static_cast<SfmlPolygon *>(polygon);

    // vertices are already built (at triangulation time), in local
    // coordinates : position goes through a transform, so nothing is
    // recomputed or reallocated per frame.
    sf::Transform transform;
    transform.translate(sfmlPolygon->_position);

    _window.draw(sfmlPolygon->_vertices, transform);
}

void SfmlWindow::drawSprite(graphic::ISprite *sprite) {
    SfmlSprite *sfmlSprite = static_cast<SfmlSprite *>(sprite);
    _window.draw(sfmlSprite->_sprite);
}

void SfmlWindow::drawText(graphic::IText *text) {
    SfmlText *sfmlText = static_cast<SfmlText *>(text);
    _window.draw(sfmlText->_text);
}

/* Input.
 *
 * isKeyPressed / isKeyReleased : the frame's events are still in _events,
 *         so a front is a lookup in there. Nothing is consumed, so any
 *         number of readers get the same answer.
 * isKeyDown / isKeyUp          : the state the window keeps, since no
 *         single event can say a key is still held.
 *
 * Both are valid anywhere in the frame. A handful of events per frame, so
 * the scan costs nothing next to the 87-entry walk whichKeyDown() does.
 */

/** @brief true si un evenement de la frame satisfait le predicat. */
template <typename E, typename Match>
static bool anyEvent(const std::vector<sf::Event> &events, Match match) {
    for (const auto &event : events)
        if (const auto *typed = event.getIf<E>())
            if (match(*typed))
                return true;
    return false;
}

std::vector<graphic::IKeyboard::Keys> SfmlKeyboard::whichKeyDown() const {
    std::vector<Keys> keys;

    for (const auto &[key, code] : _keys)
        if (_window._keysDown[SfmlWindow::index(code)])
            keys.push_back(key);
    return keys;
}

bool SfmlKeyboard::isKeyPressed(Keys key) const {
    const auto code = _keys.at(key);

    return anyEvent<sf::Event::KeyPressed>(_window._events,
        [code](const auto &pressed) { return pressed.scancode == code; });
}

bool SfmlKeyboard::isKeyReleased(Keys key) const {
    const auto code = _keys.at(key);

    return anyEvent<sf::Event::KeyReleased>(_window._events,
        [code](const auto &released) { return released.scancode == code; });
}

bool SfmlKeyboard::isKeyDown(Keys key) const { return _window._keysDown[SfmlWindow::index(_keys.at(key))]; }
bool SfmlKeyboard::isKeyUp(Keys key) const { return !isKeyDown(key); }

bool SfmlMouse::isButtonPressed(Buttons key) const {
    const auto button = _buttons.at(key);

    return anyEvent<sf::Event::MouseButtonPressed>(_window._events,
        [button](const auto &pressed) { return pressed.button == button; });
}

bool SfmlMouse::isButtonReleased(Buttons key) const {
    const auto button = _buttons.at(key);

    return anyEvent<sf::Event::MouseButtonReleased>(_window._events,
        [button](const auto &released) { return released.button == button; });
}

bool SfmlMouse::isButtonDown(Buttons key) const { return _window._mouseDown[size_t(_buttons.at(key))]; }
bool SfmlMouse::isButtonUp(Buttons key) const { return !isButtonDown(key); }

Vector2f SfmlMouse::getPosition() const {
    const sf::Vector2i position = _window._mousePosition;
    return Vector2f{static_cast<double>(position.x), static_cast<double>(position.y)};
}

void SfmlMouse::setPosition(Vector2f position) {
    const sf::Vector2i target{static_cast<int>(position.x), static_cast<int>(position.y)};

    _window._mousePosition = target;
    sf::Mouse::setPosition(target, _window._window);
}

float SfmlMouse::GetMouseWheelMove() const {
    float delta = 0.f;

    // additionne : deux crans dans la meme frame, sinon on en perd un
    for (const auto &event : _window._events)
        if (const auto *scroll = event.getIf<sf::Event::MouseWheelScrolled>())
            if (scroll->wheel == sf::Mouse::Wheel::Vertical)
                delta += scroll->delta;
    return delta;
}

// the pad has no carrying event here : we probe sf::Joystick, which is not
// subject to Input Monitoring
bool SfmlGamepad::isButtonDown(Button button) const {
    return isAvailable() && sf::Joystick::isButtonPressed(_index, _buttons.at(button));
}
bool SfmlGamepad::isButtonUp(Button button) const { return !isButtonDown(button); }

bool SfmlGamepad::isButtonPressed(Button button) const {
    const auto index = _index;
    const auto raw = _buttons.at(button);

    return anyEvent<sf::Event::JoystickButtonPressed>(_window._events,
        [index, raw](const auto &pressed) { return pressed.joystickId == index && pressed.button == raw; });
}

bool SfmlGamepad::isButtonReleased(Button button) const {
    const auto index = _index;
    const auto raw = _buttons.at(button);

    return anyEvent<sf::Event::JoystickButtonReleased>(_window._events,
        [index, raw](const auto &released) { return released.joystickId == index && released.button == raw; });
}

/** @} */

#endif /* !SFMLWINDOW_HPP_ */
