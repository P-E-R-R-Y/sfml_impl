/**
 * @file static.cpp
 * @brief sfml lie EN DUR : utilitaires + vendor + contrats, rien d'autre.
 *
 * Pas de dll, pas de symbole d'entree, pas de ModuleManager - tout ca
 * n'existe que pour retrouver du code dans un binaire separe au runtime,
 * ce qui n'a aucun sens quand le code est deja dans l'executable.
 *
 * Les classes concretes sont nommees ici, une seule fois, pour construire.
 * Tout de suite apres, on ne manipule plus que des contrats : runScene ne
 * connait ni sfml ni SfmlWindow, et c'est le meme fichier que dans
 * raylib_impl - et le meme que celui qu'appelle dynamic.cpp.
 *
 * unique_ptr plutot que new/delete : le module fournissait des paires
 * createX/deleteX qu'il fallait appeler dans le bon ordre. Ici la
 * destruction est automatique, en ordre inverse de declaration, y compris
 * si une exception traverse main(). Rien a oublier.
 */

#include <cstdio>
#include <memory>
#include <string>

#include "SfmlFont.hpp"
#include "SfmlGamepad.hpp"
#include "SfmlKeyboard.hpp"
#include "SfmlMouse.hpp"
#include "SfmlMusic.hpp"
#include "SfmlPolygon.hpp"
#include "SfmlSound.hpp"
#include "SfmlSoundBuffer.hpp"
#include "SfmlSprite.hpp"
#include "SfmlText.hpp"
#include "SfmlTexture.hpp"
#include "SfmlWindow.hpp"

#include "DemoScene.hpp"

int main(int argc, char **argv) {
    const std::string assets = (argc > 1) ? argv[1] : ASSETS_DIR;

    /* Aucune initialisation de peripherique audio ici : sfml n'en a pas.
     * raylib, lui, exige un InitAudioDevice() explicite - une asymetrie
     * que le module cachait, et que le lien statique fait remonter. */

    auto window   = std::make_unique<SfmlWindow>(960, 540, "sfml_impl - static");

    /* Le clavier, la souris et la manette se construisent DEPUIS la
     * fenetre : c'est elle qui tient les events dont ils dependent. raylib
     * n'en a pas besoin (ses requetes sont globales), d'ou la seule vraie
     * difference entre les deux static.cpp. */
    auto keyboard = std::make_unique<SfmlKeyboard>(*window);
    auto mouse    = std::make_unique<SfmlMouse>(*window);
    auto gamepad  = std::make_unique<SfmlGamepad>(*window);

    auto texture  = std::make_unique<SfmlTexture>(assets + "/image.png");
    auto sprite   = std::make_unique<SfmlSprite>(*texture);
    auto font     = std::make_unique<SfmlFont>(assets + "/font.ttf");
    auto hud      = std::make_unique<SfmlText>("", *font);
    auto label    = std::make_unique<SfmlText>("", *font);
    // un seul rectangle, repositionne pour dessiner chacun des boutons
    auto poly     = std::make_unique<SfmlPolygon>(
        std::vector<Vector2f>{{-125, -20}, {125, -20}, {125, 20}, {-125, 20}});
    // concave : un sommet sur deux rentre, sf::ConvexShape ne saurait pas
    auto star     = std::make_unique<SfmlPolygon>(demo::starPoints(5, 90.0, 36.0));

    auto buffer   = std::make_unique<SfmlSoundBuffer>(assets + "/step.wav");
    /* Deux sons sur le MEME buffer : un objet son est une voix. Rejouer le
     * son 1 le redemarre, jouer le 2 pendant que le 1 tourne les superpose. */
    auto sound1   = std::make_unique<SfmlSound>(*buffer);
    auto sound2   = std::make_unique<SfmlSound>(*buffer);
    auto music    = std::make_unique<SfmlMusic>(assets + "/ambience.wav");

    std::printf("[static] sfml lie en dur, aucun dlopen\n");

    // sfml ne fournit pas la 3D : window3, mesh, animations, model et
    // camera restent nullptr, et la scene s'en accommode.
    Scene scene{
        .window   = *window,
        .keyboard = *keyboard,
        .mouse    = *mouse,
        .gamepad  = gamepad.get(),
        .texture  = *texture,
        .sprite   = *sprite,
        .font     = *font,
        .hud      = *hud,
        .label    = *label,
        .poly     = *poly,
        .star     = *star,
        .buffer   = *buffer,
        .sound1   = *sound1,
        .sound2   = *sound2,
        .music    = *music,
    };

    return runScene(scene);
}
