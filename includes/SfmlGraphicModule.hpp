/**
 * @file SfmlGraphicModule.hpp
 * @author Perry Chouteau (perry.chouteau@outlook.com)
 * @brief
 * @date 2026-08-10
 *
 * @copyright Copyright (c) 2026
 *
 * @addtogroup sfml
 * @{
 */

#ifndef SFMLGRAPHIC_MODULE_HPP
#define SFMLGRAPHIC_MODULE_HPP

#include "IGraphic2Module.hpp"

#include "window/SfmlWindow.hpp"

#include "event/SfmlGamepad.hpp"
#include "event/SfmlKeyboard.hpp"
#include "event/SfmlMouse.hpp"

#include "graphic/SfmlFont.hpp"
#include "graphic/SfmlPolygon.hpp"
#include "graphic/SfmlSprite.hpp"
#include "graphic/SfmlText.hpp"
#include "graphic/SfmlTexture.hpp"

/**
 * @brief Sfml only implements IGraphic2Module - no IGraphic3Module, sfml
 *        has no native 3D. No fake IModel here.
 */
class SfmlGraphicModule : public IGraphic2Module {

public:
    SfmlGraphicModule() = default;
    ~SfmlGraphicModule() = default;

    const char *type() const override { return IGraphic2Module::contract; }
    const char *name() const override { return "sfml"; }

    // window
    graphic::IWindow2 *createWindow(int32_t screenWidth, int32_t screenHeight, std::string title) override {
        SfmlWindow *created = new SfmlWindow(screenWidth, screenHeight, title);

        /* On retient la premiere : c'est celle que window() pretera aux
         * invites. sfml sait en ouvrir plusieurs, mais une seule a le
         * clavier - celle qui a le focus. */
        if (!_window)
            _window = created;
        return created;
    }
    void deleteWindow(graphic::IWindow2 *window) override {
        if (window == _window)
            _window = nullptr;
        delete window;
    }

    graphic::IWindow2 *window() override { return _window; }

    // input - the window is handed over at construction, the way an
    // ITexture is to an ISprite : linked once, never reconciled later
    graphic::IKeyboard *createKeyboard(graphic::IWindow *window) override {
        return new SfmlKeyboard(*static_cast<SfmlWindow *>(window));
    }
    void deleteKeyboard(graphic::IKeyboard *keyboard) override { delete keyboard; }

    graphic::IMouse *createMouse(graphic::IWindow *window) override {
        return new SfmlMouse(*static_cast<SfmlWindow *>(window));
    }
    void deleteMouse(graphic::IMouse *mouse) override { delete mouse; }

    graphic::IGamepad *createGamepad(graphic::IWindow *window) override {
        return new SfmlGamepad(*static_cast<SfmlWindow *>(window));
    }
    void deleteGamepad(graphic::IGamepad *gamepad) override { delete gamepad; }

    // texture
    graphic::ITexture *createTexture(std::string path) override {
        return new SfmlTexture(path);
    }
    void deleteTexture(graphic::ITexture *texture) override {
        delete texture;
    }

    // font
    graphic::IFont *createFont(std::string path) override {
        return new SfmlFont(path);
    }
    void deleteFont(graphic::IFont *font) override {
        delete font;
    }

    // polygon
    graphic::IPolygon *createPolygon(std::vector<Vector2f> points) override {
        return new SfmlPolygon(points);
    }
    void deletePolygon(graphic::IPolygon *polygon) override {
        delete polygon;
    }

    // sprite
    graphic::ISprite *createSprite(graphic::ITexture *texture) override {
        return new SfmlSprite(*static_cast<SfmlTexture *>(texture));
    }
    void deleteSprite(graphic::ISprite *sprite) override {
        delete sprite;
    }

    // text
    graphic::IText *createText(std::string text, graphic::IFont *font) override {
        return new SfmlText(text, *static_cast<SfmlFont *>(font));
    }
    void deleteText(graphic::IText *text) override {
        delete text;
    }

private:
    /// La premiere fenetre ouverte : celle que window() prete.
    graphic::IWindow2 *_window = nullptr;
};

/** @} */

#endif /* !SFMLGRAPHIC_MODULE_HPP */
