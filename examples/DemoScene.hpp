/**
 * @file DemoScene.hpp
 * @brief La demo, une seule fois. Contrats uniquement.
 *
 * Ce fichier est identique a l'octet pres entre raylib_impl et sfml_impl,
 * et il est le MEME pour l'exemple statique et pour l'exemple dynamique.
 * C'est tout l'interet : si les deux binaires se comportent pareil, c'est
 * que les deux chemins de construction produisent bien les memes objets.
 *
 * Ce qui change, ce n'est jamais la scene :
 *
 * - static.cpp nomme les classes concretes et les tient en unique_ptr ;
 * - dynamic.cpp ne connait qu'un chemin de .dylib et passe par le
 *   ModuleManager, donc par les paires createXxx / deleteXxx.
 *
 * Les contrats optionnels sont des POINTEURS et valent nullptr quand le
 * vendor ne les fournit pas (sfml n'a pas de 3D, un backend sans manette
 * rend nullptr). Une capacite absente n'est pas une erreur.
 *
 * Un seul polygone et un seul texte servent a dessiner TOUS les boutons :
 * on les repositionne entre deux draw. C'est ce que permet une API a etat
 * retenu, et ca evite de faire construire dix objets de plus a l'appelant.
 */

#ifndef DEMOSCENE_HPP_
#define DEMOSCENE_HPP_

#include "IAnimationSet.hpp"
#include "ICamera.hpp"
#include "IFont.hpp"
#include "IGamepad.hpp"
#include "IKeyboard.hpp"
#include "IMesh.hpp"
#include "IModel.hpp"
#include "IMouse.hpp"
#include "IMusic.hpp"
#include "IPolygon.hpp"
#include "ISound.hpp"
#include "ISoundBuffer.hpp"
#include "ISprite.hpp"
#include "IText.hpp"
#include "ITexture.hpp"
#include "IWindow2.hpp"
#include "IWindow3.hpp"

#include "KeyNames.hpp"

#include <cstdio>
#include <string>

/**
 * @brief Tout ce que la scene manipule. Les references sont obligatoires,
 *        les pointeurs sont les capacites optionnelles.
 */
struct Scene {
    graphic::IWindow2      &window;
    graphic::IWindow3      *window3 = nullptr;   // nullptr : vendor 2D seulement

    graphic::IKeyboard     &keyboard;
    graphic::IMouse        &mouse;
    graphic::IGamepad      *gamepad = nullptr;   // nullptr : pas de manette

    graphic::ITexture      &texture;             // celle du sprite, et d'elle seule
    graphic::ISprite       &sprite;
    graphic::IFont         &font;
    graphic::IText         &hud;                 // le bandeau d'etat
    graphic::IText         &label;               // reutilise pour chaque bouton
    graphic::IPolygon      &poly;                // reutilise pour chaque bouton
    graphic::IPolygon      &star;                // concave : le vrai test du decoupage

    audio::ISoundBuffer    &buffer;
    audio::ISound          &sound1;
    audio::ISound          &sound2;
    audio::IMusic          &music;

    graphic::IMesh         *mesh = nullptr;      // les cinq vont ensemble
    graphic::IAnimationSet *animations = nullptr;
    graphic::IModel        *model = nullptr;
    graphic::ICamera       *camera = nullptr;
    /* La peau du modele, distincte de celle du sprite. Sans ce champ,
     * scene.texture servait aux deux et le crop du sprite se calculait sur
     * les dimensions du renard. */
    graphic::ITexture      *modelTexture = nullptr;
};

namespace demo {

    enum Action { SOUND_1, SOUND_2, MUSIC_TOGGLE, MUSIC_FORWARD, MUSIC_RESET, QUIT, ACTION_COUNT };

    inline const char *actionName(int action) {
        switch (action) {
            case SOUND_1:       return "son 1";
            case SOUND_2:       return "son 2 (superpose)";
            case MUSIC_TOGGLE:  return "musique play / pause";
            case MUSIC_FORWARD: return "musique +10s";
            case MUSIC_RESET:   return "musique reset";
            default:            return "quitter";
        }
    }

    inline const int   BUTTON_W = 250;
    inline const int   BUTTON_H = 40;
    inline const float BUTTON_Y = 70.f;
    inline const float BUTTON_STEP = 52.f;
    inline const float MARGIN = 24.f;
    inline const float HUD_HEIGHT = 200.f;

    /** @brief Le coin haut-gauche de l'image, sous le bandeau de texte. */
    inline Vector2f spriteHome() { return {MARGIN, MARGIN + HUD_HEIGHT}; }

    /**
     * @brief Une etoile a N branches, centree sur l'origine.
     *
     * 2N sommets qui alternent grand et petit rayon. Un sommet sur deux est
     * RENTRANT : c'est un polygone concave, donc ni sf::ConvexShape ni
     * DrawTriangleFan ne savent le dessiner. Il ne peut apparaitre que si le
     * decoupage en oreilles a fait son travail - un rectangle, lui, aurait
     * l'air correct meme avec une triangulation ratee.
     *
     * 2N sommets donnent 2N-2 triangles : 8 pour une etoile a 5 branches.
     */
    inline std::vector<Vector2f> starPoints(int branches, double outer, double inner) {
        std::vector<Vector2f> points;

        for (int i = 0; i < branches * 2; i++) {
            const double angle = -1.5707963 + i * 3.1415926 / branches;
            const double radius = (i % 2 == 0) ? outer : inner;

            points.push_back({std::cos(angle) * radius, std::sin(angle) * radius});
        }
        return points;
    }

    /**
     * @brief Le centre du bouton n, ancre au bord DROIT.
     *
     * Recalcule a chaque frame depuis window.getSize() : une position en dur
     * laisserait les boutons au milieu des que la fenetre grandit, et le
     * test de survol taperait a cote de ce qui est dessine. Le dessin et le
     * hit-test lisent la meme fonction, donc ils ne peuvent pas diverger.
     */
    inline Vector2f buttonCenter(Vector2f windowSize, int action) {
        return {windowSize.x - MARGIN - BUTTON_W / 2.f,
                BUTTON_Y + float(action) * BUTTON_STEP};
    }

    inline bool overButton(Vector2f windowSize, Vector2f point, int action) {
        const Vector2f center = buttonCenter(windowSize, action);

        return point.x >= center.x - BUTTON_W / 2 && point.x <= center.x + BUTTON_W / 2 &&
               point.y >= center.y - BUTTON_H / 2 && point.y <= center.y + BUTTON_H / 2;
    }

    inline bool overSprite(const Scene &scene, Vector2f point) {
        const Vector2f position = scene.sprite.getPosition();
        const Vector2f size = scene.sprite.getSize();

        return point.x >= position.x && point.x <= position.x + size.x &&
               point.y >= position.y && point.y <= position.y + size.y;
    }

    inline std::string round1(float value) {
        char buffer[32];
        std::snprintf(buffer, sizeof(buffer), "%.1f", value);
        return buffer;
    }

    inline const char *yesNo(bool value) { return value ? "oui" : "non"; }

    /** @brief isReady() de tout ce qui l'expose, une fois au demarrage. */
    inline void reportReady(const Scene &scene) {
        std::printf("[scene] texture %s  police %s  sprite %s  texte %s  polygone %s\n",
                    yesNo(scene.texture.isReady()), yesNo(scene.font.isReady()),
                    yesNo(scene.sprite.isReady()), yesNo(scene.hud.isReady()),
                    yesNo(scene.poly.isReady()));
        std::printf("[scene] buffer %s (%s s)  son1 %s  son2 %s  musique %s (%s s)\n",
                    yesNo(scene.buffer.isReady()), round1(scene.buffer.getLength()).c_str(),
                    yesNo(scene.sound1.isReady()), yesNo(scene.sound2.isReady()),
                    yesNo(scene.music.isReady()), round1(scene.music.getLength()).c_str());
        const Color color = scene.poly.getColor();

        std::printf("[scene] bouton %zu points, couleur %d,%d,%d   hud %u px, %zu caracteres\n",
                    scene.poly.getPoints().size(), color.r, color.g, color.b,
                    scene.hud.getFontSize(), scene.hud.getText().size());
        std::printf("[scene] etoile concave %zu points -> %zu triangles attendus\n",
                    scene.star.getPoints().size(), scene.star.getPoints().size() - 2);
        std::printf("[scene] 3D %s   manette %s\n",
                    scene.model ? "fournie" : "absente",
                    scene.gamepad ? yesNo(scene.gamepad->isAvailable()) : "absente");

        if (scene.mesh)
            std::printf("[scene] mesh %s\n", yesNo(scene.mesh->isReady()));
        if (scene.animations) {
            std::printf("[scene] %d clips :", scene.animations->getSize());
            for (int clip = 0; clip < scene.animations->getSize(); clip++)
                std::printf(" %s(%df)", scene.animations->getName(clip).c_str(),
                            scene.animations->getFrameCount(clip));
            std::printf("\n");
        }
    }

    struct State {
        int   sounds = 0;
        int   clicks = 0;
        bool  musicPlaying = false;
        bool  dragging = false;
        bool  cropped = false;
        float spriteAngle = 0.f;
        Vector2f grab{};
        std::string lastAction = "-";
    };

    inline void trigger(Scene &scene, State &state, int action) {
        state.lastAction = actionName(action);

        switch (action) {
            /* Deux sons sur le MEME buffer. Rejouer le son 1 le redemarre
             * depuis zero ; jouer le 2 pendant que le 1 tourne les
             * superpose. Un objet son = une voix, c'est la seule facon
             * d'en avoir deux qui sonnent ensemble. */
            case SOUND_1: scene.sound1.play(); state.sounds++; break;
            case SOUND_2: scene.sound2.play(); state.sounds++; break;

            case MUSIC_TOGGLE:
                state.musicPlaying = !state.musicPlaying;
                state.musicPlaying ? scene.music.play() : scene.music.pause();
                break;
            case MUSIC_FORWARD:
                scene.music.setTime(scene.music.getTime() + 10.f);
                break;
            case MUSIC_RESET:
                scene.music.stop();
                scene.music.setTime(0.f);
                state.musicPlaying = false;
                break;
            default:
                scene.window.close();
                break;
        }
    }

    /**
     * @brief Les fronts : lus une fois par frame, dans la condition.
     *
     * Sans evenement ils sont faux de toute facon, donc sauter le bloc ne
     * change rien - c'est une optimisation, pas une regle. Les etats
     * (isKeyDown, whichKeyDown) sont lus plus bas, dehors.
     */
    inline void readFronts(Scene &scene, State &state) {
        using KB = graphic::IKeyboard;
        using MS = graphic::IMouse;

        graphic::IKeyboard &kb = scene.keyboard;
        graphic::IMouse &mouse = scene.mouse;

        scene.window.eventClose();

        // les memes actions au clavier qu'a la souris
        if (kb.isKeyPressed(KB::KEY_1)) trigger(scene, state, SOUND_1);
        if (kb.isKeyPressed(KB::KEY_2)) trigger(scene, state, SOUND_2);
        if (kb.isKeyPressed(KB::KEY_P)) trigger(scene, state, MUSIC_TOGGLE);
        if (kb.isKeyPressed(KB::KEY_F)) trigger(scene, state, MUSIC_FORWARD);
        if (kb.isKeyPressed(KB::KEY_R)) trigger(scene, state, MUSIC_RESET);

        // isKeyReleased : la pause ne part qu'au RELACHEMENT de la touche
        if (kb.isKeyReleased(KB::KEY_3)) scene.sound1.pause();
        if (kb.isKeyPressed(KB::KEY_4))  { scene.sound1.stop(); scene.sound2.stop(); }
        if (kb.isKeyPressed(KB::KEY_L))  scene.music.setLoop(!scene.music.getLoop());
        if (kb.isKeyPressed(KB::KEY_B))  scene.music.setTime(scene.music.getTime() - 10.f);

        // le sprite : rotation, taille, decoupe
        if (kb.isKeyPressed(KB::KEY_A)) scene.sprite.setRotation(state.spriteAngle -= 15.f);
        if (kb.isKeyPressed(KB::KEY_E)) scene.sprite.setRotation(state.spriteAngle += 15.f);
        if (kb.isKeyPressed(KB::KEY_G)) scene.sprite.setSize(scene.sprite.getSize() * 0.8);
        if (kb.isKeyPressed(KB::KEY_H)) scene.sprite.setSize(scene.sprite.getSize() * 1.25);
        if (kb.isKeyPressed(KB::KEY_C)) {
            const Vector2f size = scene.texture.getSize();

            state.cropped = !state.cropped;
            scene.sprite.setCrop(state.cropped
                ? Rect<float>{float(size.x) / 4.f, float(size.y) / 4.f,
                              float(size.x) / 2.f, float(size.y) / 2.f}
                : Rect<float>{0.f, 0.f, float(size.x), float(size.y)});
        }

        // la fenetre se deplace et se redimensionne depuis le contrat
        if (kb.isKeyPressed(KB::KEY_W)) scene.window.setPosition(scene.window.getPosition() + Vector2f{40, 0});
        if (kb.isKeyPressed(KB::KEY_X)) scene.window.setPosition(scene.window.getPosition() - Vector2f{40, 0});
        if (kb.isKeyPressed(KB::KEY_Y)) scene.window.setSize(scene.window.getSize() * 0.9);
        if (kb.isKeyPressed(KB::KEY_U)) scene.window.setSize(scene.window.getSize() * 1.1);
        if (kb.isKeyPressed(KB::KEY_M)) mouse.setPosition(scene.window.getSize() / 2.0);
        if (kb.isKeyPressed(KB::KEY_V)) {
            const Color color = scene.hud.getTextColor();
            scene.hud.setTextColor({color.g, color.b, color.r, color.a});
        }

        // la molette regle le volume des deux voix
        const float wheel = mouse.GetMouseWheelMove();
        if (wheel != 0.f) {
            const float volume = scene.sound1.getVolume() + wheel * 0.1f;

            scene.sound1.setVolume(volume);
            scene.sound2.setVolume(volume);
        }

        if (mouse.isButtonPressed(MS::BUTTON_LEFT)) {
            const Vector2f point = mouse.getPosition();

            state.clicks++;
            for (int action = 0; action < ACTION_COUNT; action++)
                if (overButton(scene.window.getSize(), point, action))
                    trigger(scene, state, action);

            // drag & drop : on attrape le sprite s'il est sous le curseur
            if (overSprite(scene, point)) {
                state.dragging = true;
                state.grab = {point.x - scene.sprite.getPosition().x,
                              point.y - scene.sprite.getPosition().y};
            }
        }
        if (mouse.isButtonReleased(MS::BUTTON_LEFT))
            state.dragging = false;
        if (mouse.isButtonPressed(MS::BUTTON_RIGHT))
            scene.sprite.setPosition(spriteHome());

        // 3D : parcourir les clips, revenir au debut, zoomer
        if (scene.model) {
            graphic::IModel &model = *scene.model;

            if (kb.isKeyPressed(KB::KEY_J))
                model.setAnimation((model.getAnimation() + model.getAnimationsSize() - 1)
                                   % model.getAnimationsSize());
            if (kb.isKeyPressed(KB::KEY_K))
                model.setAnimation((model.getAnimation() + 1) % model.getAnimationsSize());
            if (kb.isKeyPressed(KB::KEY_SPACE)) model.setFrame(model.getFirstFrame());
            if (kb.isKeyPressed(KB::KEY_I))     model.setSize(model.getSize() * 1.2);
            if (kb.isKeyPressed(KB::KEY_Z))     model.setSize(model.getSize() * 0.8);
        }
        if (scene.camera) {
            graphic::ICamera &camera = *scene.camera;

            if (kb.isKeyPressed(KB::KEY_COMMA))  camera.setFov(camera.getFov() - 5.f);
            if (kb.isKeyPressed(KB::KEY_PERIOD)) camera.setFov(camera.getFov() + 5.f);
            if (kb.isKeyPressed(KB::KEY_SLASH))
                camera.setProjection(camera.getProjection() == graphic::ICamera::PERSPECTIVE
                                     ? graphic::ICamera::ORTHOGRAPHIC
                                     : graphic::ICamera::PERSPECTIVE);
        }
    }

    /** @brief Les etats : vrais tant que ca dure, meme sans evenement. */
    inline void readStates(Scene &scene, State &state) {
        using KB = graphic::IKeyboard;

        graphic::IKeyboard &kb = scene.keyboard;

        if (kb.isKeyDown(KB::KEY_ESCAPE))
            scene.window.close();

        if (state.dragging) {
            const Vector2f point = scene.mouse.getPosition();
            const bool horizontalOnly = kb.isKeyDown(KB::KEY_LEFT_SHIFT) ||
                                        kb.isKeyDown(KB::KEY_RIGHT_SHIFT);

            // Maj tenue = deplacement horizontal seulement : une touche
            // tenue qui qualifie un drag souris
            scene.sprite.setPosition({point.x - state.grab.x,
                                      horizontalOnly ? scene.sprite.getPosition().y
                                                     : point.y - state.grab.y});
            return;
        }

        Vector2f position = scene.sprite.getPosition();
        const double step = 4.0;

        if (kb.isKeyDown(KB::KEY_LEFT))  position.x -= step;
        if (kb.isKeyDown(KB::KEY_RIGHT)) position.x += step;
        if (kb.isKeyDown(KB::KEY_UP))    position.y -= step;
        if (kb.isKeyDown(KB::KEY_DOWN))  position.y += step;

        // la manette pousse dans le meme sens, quand il y en a une
        if (scene.gamepad && scene.gamepad->isAvailable()) {
            position.x += scene.gamepad->getAxisMovement(graphic::IGamepad::AXIS_LEFT_X) * step;
            position.y += scene.gamepad->getAxisMovement(graphic::IGamepad::AXIS_LEFT_Y) * step;
        }
        scene.sprite.setPosition(position);
    }

    /** @brief Six lignes courtes, pas un vidage de getters. */
    inline void buildHud(Scene &scene, const State &state) {
        using KB = graphic::IKeyboard;
        using MS = graphic::IMouse;

        std::string keys;
        for (KB::Keys key : scene.keyboard.whichKeyDown()) {
            if (!keys.empty()) keys += " ";
            keys += keyName(key);
        }

        const Vector2f mouse = scene.mouse.getPosition();
        const Vector2f size = scene.window.getSize();
        const Rect<float> bounds = scene.sprite.getBounds();

        // le texte en haut a gauche, l'image dessous : les deux restent
        // visibles quelle que soit la taille de la fenetre
        scene.hud.setPosition({MARGIN, MARGIN});

        std::string text;
        text += "glisser l'image   MAJ = horizontal   clic droit = replacer\n";
        text += "1/2 sons  3 pause  4 stop  P musique  F/B +-10s  L loop\n";
        text += "A/E pivot  G/H taille  C crop  V couleur  W/X/Y/U fenetre  ECHAP\n";
        text += "\n";
        text += "action : " + state.lastAction + "   sons x" + std::to_string(state.sounds) +
                "   clics x" + std::to_string(state.clicks) + "\n";
        text += "souris " + std::to_string(int(mouse.x)) + "," + std::to_string(int(mouse.y)) +
                "   bouton " + yesNo(scene.mouse.isButtonDown(MS::BUTTON_LEFT)) +
                "   relache " + yesNo(scene.mouse.isButtonUp(MS::BUTTON_MIDDLE)) +
                "   delta " + std::to_string(scene.window.getDelta()) + " ms\n";
        text += "touches : " + (keys.empty() ? std::string("(aucune)") : keys) +
                "   espace relache " + yesNo(scene.keyboard.isKeyUp(KB::KEY_SPACE)) + "\n";
        text += "image " + round1(scene.sprite.getRotation()) + " deg  " +
                std::to_string(int(bounds.w)) + "x" + std::to_string(int(bounds.h)) +
                "   volume " + round1(scene.sound1.getVolume()) +
                " (pan " + round1(float(scene.sound1.getPosition().x)) +
                ", v " + round1(float(scene.sound1.getVelocity().x)) + ")\n";
        text += "musique " + round1(scene.music.getTime()) + " / " +
                round1(scene.music.getLength()) + " s   volume " +
                round1(scene.music.getVolume()) + "   loop " + yesNo(scene.music.getLoop()) +
                "   v " + round1(float(scene.music.getVelocity().x)) + "\n";

        if (scene.model && scene.camera)
            text += "clip " + std::to_string(scene.model->getAnimation() + 1) + "/" +
                    std::to_string(scene.model->getAnimationsSize()) +
                    "  frame " + std::to_string(scene.model->getFrame()) +
                    " [" + std::to_string(scene.model->getFirstFrame()) + "-" +
                    std::to_string(scene.model->getLastFrame()) + "]" +
                    "   camera fov " + round1(scene.camera->getFov()) +
                    " mode " + std::to_string(int(scene.camera->getMode())) +
                    " proj " + std::to_string(int(scene.camera->getProjection())) +
                    " cible " + round1(float(scene.camera->getTarget().y)) +
                    " haut " + round1(float(scene.camera->getUp().y)) +
                    "   J/K clip  I/Z taille\n";

        scene.hud.setText(text);
    }

    /** @brief Un seul polygone et un seul texte, repositionnes N fois. */
    inline void drawButtons(Scene &scene, Vector2f windowSize, Vector2f mouse) {
        for (int action = 0; action < ACTION_COUNT; action++) {
            const Vector2f center = buttonCenter(windowSize, action);
            const bool hovered = overButton(windowSize, mouse, action);

            /* Clair sur fond noir : beginDraw() efface en noir chez les deux
             * vendors, un gris ardoise y serait invisible. */
            scene.poly.setPosition(center);
            scene.poly.setColor(hovered ? Color{120, 175, 255, 255} : Color{95, 105, 135, 255});
            scene.window.drawPoly(&scene.poly);

            scene.label.setText(actionName(action));
            scene.label.setPosition({center.x - BUTTON_W / 2 + 14, center.y - 11});
            scene.window.drawText(&scene.label);
        }
    }

    inline void draw(Scene &scene) {
        scene.window.beginDraw();

        // la 3D d'abord : elle occupe le fond, le 2D passe par-dessus
        if (scene.window3 && scene.model && scene.camera) {
            scene.window3->beginMode3(scene.camera);
            scene.window3->drawModel(scene.model);
            scene.window3->drawText3D(&scene.label, {0.f, 40.f, 0.f});
            scene.window3->endMode3();
        }

        drawButtons(scene, scene.window.getSize(), scene.mouse.getPosition());

        /* L'etoile tourne : chaque frame la redessine sous un autre angle,
         * donc une triangulation fausse se voit tout de suite - des trous
         * apparaissent entre les branches. */
        scene.window.drawPoly(&scene.star);

        scene.window.drawSprite(&scene.sprite);
        scene.window.drawText(&scene.hud);
        scene.window.endDraw();
    }

} // namespace demo

/**
 * @brief La boucle. Le meme corps derriere n'importe quel vendor, lie en
 *        dur ou charge a chaud.
 */
inline int runScene(Scene &scene) {
    demo::State state;

    scene.window.setFrameLimit(60);

    scene.sprite.setSize({200.f, 150.f});
    scene.sprite.setPosition(demo::spriteHome());

    scene.hud.setFont(&scene.font);
    scene.hud.setFontSize(16);
    /* Saturee, et pas un gris : V permute les canaux RVB, ce qui ne se voit
     * que si les trois different vraiment. Le cycle est ambre -> violet ->
     * menthe -> ambre. */
    scene.hud.setTextColor({255, 205, 80, 255});
    scene.hud.setRotation(0.f);
    // sa position est recalculee a chaque frame dans buildHud()

    scene.label.setFont(&scene.font);
    scene.label.setFontSize(17);
    scene.label.setTextColor({245, 245, 245, 255});

    scene.poly.setColor({95, 105, 135, 255});
    scene.star.setColor({255, 205, 80, 255});
    scene.star.setPosition({330.f, 340.f});

    /* Le son est place dans l'espace : sfml le sait, raylib l'ignore, et
     * le contrat ne casse ni d'un cote ni de l'autre. */
    scene.sound1.setVolume(0.6f);
    scene.sound2.setVolume(0.6f);
    scene.sound1.setPosition({-1.f, 0.f, 0.f});
    scene.sound2.setPosition({1.f, 0.f, 0.f});
    scene.sound1.setVelocity({0.f, 0.f, 0.f});
    scene.music.setVolume(0.4f);
    scene.music.setLoop(true);
    scene.music.setPosition({0.f, 0.f, 0.f});
    scene.music.setVelocity({0.f, 0.f, 0.f});

    if (scene.model && scene.animations) {
        scene.model->setTexture(scene.modelTexture ? scene.modelTexture : &scene.texture);
        scene.model->setAnimations(scene.animations);
        scene.model->setAnimation(0);
        scene.model->setFrameRange(0, scene.animations->getFrameCount(0) - 1);
        scene.model->setSize(scene.model->getSize() / 2.0);
        scene.model->setPosition({0.f, -10.f, 0.f});
        scene.model->setRotation({0.f, 0.f, 0.f});
    }
    if (scene.camera) {
        scene.camera->setMode(graphic::ICamera::ORBITAL);
        scene.camera->setProjection(graphic::ICamera::PERSPECTIVE);
        scene.camera->setUp({0.f, 1.f, 0.f});
        scene.camera->setTarget({0.f, 0.f, 0.f});
    }

    demo::reportReady(scene);

    while (scene.window.isOpen()) {
        if (scene.window.pollEvent())
            demo::readFronts(scene, state);

        demo::readStates(scene, state);

        scene.music.update();
        if (scene.model)
            scene.model->updateAnimation();

        demo::buildHud(scene, state);
        demo::draw(scene);
    }
    return 0;
}

#endif /* !DEMOSCENE_HPP_ */
