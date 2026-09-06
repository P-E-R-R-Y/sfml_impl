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

    /**
     * @brief Revendique "opengl" : aucun autre vendor OpenGL en meme temps.
     *
     * sfml gere ses contextes GL elle-meme et les rend courants sur le
     * thread au fil de ses appels. Un autre vendor qui manipule le meme
     * contexte en parallele - raylib, qui le garde en globale - lui prend
     * le contexte courant sous les pieds : le rendu part dans la mauvaise
     * fenetre, ou le driver tombe. Rien de tout cela ne remonte comme une
     * erreur rattrapable.
     *
     * D'ou le refus a l'acquisition plutot qu'une detection apres coup.
     *
     * @return const char *const*
     */
    const char *const *claims() const override {
        static const char *claimed[] = { "opengl", nullptr };
        return claimed;
    }

    /**
     * @brief Ouvre une fenetre. sfml sait en tenir plusieurs.
     *
     * La PREMIERE est retenue comme celle que window() prete : sfml en ouvre
     * autant qu'on veut, mais le clavier ne suit que celle qui a le focus,
     * il faut donc en designer une.
     *
     * @return graphic::IWindow2*
     */
    graphic::IWindow2 *createWindow(int32_t screenWidth, int32_t screenHeight, std::string title) override {
        SfmlWindow *created = new SfmlWindow(screenWidth, screenHeight, title);

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

    /* La fenetre est LUE, contrairement a raylib : sfml attache son etat
     * d'entree a une fenetre precise, il faut donc dire laquelle. Elle est
     * donnee a la construction et n'est jamais rebranchee ensuite. */
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
