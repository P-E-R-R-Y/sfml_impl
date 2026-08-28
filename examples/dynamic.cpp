/**
 * @file dynamic.cpp
 * @brief Le vendor charge A CHAUD : le binaire ne connait aucun vendor.
 *
 * Il n'inclut aucun header Sfml*, ne linke pas SFML, et ne sait qu'une
 * chose : le chemin d'une .so/.dylib. ModuleManager fait le dlopen, resout
 * getGraphic2Module() / getGraphic3Module() / getAudioModule() par leur
 * symbole, et rend des pointeurs de contrat.
 *
 * Le meme binaire accepte raylib_impl en argv[1] sans etre recompile. Et
 * Get<IGraphic3Module>() rend nullptr derriere sfml, puisque ce symbole
 * n'y est pas exporte - une capacite absente, pas une erreur.
 *
 * La scene appelee a la fin est celle de static.cpp, au fichier pres :
 * seule la CONSTRUCTION change, et c'est ce qui prouve que les deux
 * chemins donnent le meme resultat.
 */

#include <cstdio>
#include <string>

#include "IAudioModule.hpp"
#include "IGraphic2Module.hpp"
#include "IGraphic3Module.hpp"
#include "ModuleManager.hpp"

#include "DemoScene.hpp"

int main(int argc, char **argv) {
    const std::string path = (argc > 1) ? argv[1] : SFML_IMPL_PATH;
    const std::string assets = (argc > 2) ? argv[2] : ASSETS_DIR;

    ModuleManager<IGraphic2Module, IGraphic3Module, IAudioModule> modules;

    // Load() rend false quand la cle est deja prise ; un chemin invalide
    // leve l'exception de SharedLibrary (dlopen a echoue).
    try {
        modules.Load(path, "vendor");
    } catch (const std::exception &error) {
        std::printf("[dynamic] chargement impossible de %s : %s\n", path.c_str(), error.what());
        return 84;
    }

    // "vendor" est la cle qu'on vient de choisir. Rien d'autre ne nomme
    // cette ligne, donc il n'y a pas de seconde chaine a se tromper.
    IGraphic2Module *graphic = modules.Get<IGraphic2Module>("vendor");
    IAudioModule *audio = modules.Get<IAudioModule>("vendor");

    if (!graphic || !audio) {
        std::printf("[dynamic] %s ne fournit pas les contrats requis\n", path.c_str());
        return 84;
    }

    // Contrat optionnel : absent veut dire nullptr, la scene s'adapte.
    IGraphic3Module *graphic3 = modules.Get<IGraphic3Module>("vendor");

    std::printf("[dynamic] '%s' charge depuis %s\n", graphic->name(), path.c_str());
    std::printf("[dynamic]   IGraphic2Module : oui\n");
    std::printf("[dynamic]   IGraphic3Module : %s\n", graphic3 ? "oui" : "non");
    std::printf("[dynamic]   IAudioModule    : oui\n");

    /* Une seule fenetre : si le vendor sait faire de la 3D on prend la
     * sienne, un IWindow3 EST un IWindow2 donc tout le 2D marche dessus. */
    graphic::IWindow3 *window3 = graphic3
        ? graphic3->createWindow(960, 540, "vendor - dynamic")
        : nullptr;
    graphic::IWindow2 *window = window3
        ? static_cast<graphic::IWindow2 *>(window3)
        : graphic->createWindow(960, 540, "vendor - dynamic");

    graphic::IKeyboard *keyboard = graphic->createKeyboard(window);
    graphic::IMouse *mouse = graphic->createMouse(window);
    graphic::IGamepad *gamepad = graphic->createGamepad(window);

    graphic::ITexture *texture = graphic->createTexture(assets + "/image.png");
    graphic::ISprite *sprite = graphic->createSprite(texture);
    graphic::IFont *font = graphic->createFont(assets + "/font.ttf");
    graphic::IText *hud = graphic->createText("", font);
    graphic::IText *label = graphic->createText("", font);
    // un seul rectangle, repositionne pour dessiner chacun des boutons
    graphic::IPolygon *poly = graphic->createPolygon(
        {{-125, -20}, {125, -20}, {125, 20}, {-125, 20}});
    // concave : un sommet sur deux rentre, aucune primitive du vendor ne
    // sait le dessiner sans passer par le decoupage en oreilles
    graphic::IPolygon *star = graphic->createPolygon(demo::starPoints(5, 90.0, 36.0));

    audio::ISoundBuffer *buffer = audio->createSoundBuffer(assets + "/step.wav");
    // deux voix sur le meme buffer, comme dans static.cpp
    audio::ISound *sound1 = audio->createSound(buffer);
    audio::ISound *sound2 = audio->createSound(buffer);
    audio::IMusic *music = audio->createMusic(assets + "/ambience.wav");

    graphic::IMesh *mesh = graphic3 ? graphic3->createMesh(assets + "/fox.glb") : nullptr;
    graphic::IAnimationSet *animations = graphic3
        ? graphic3->createAnimationSet(assets + "/fox.glb")
        : nullptr;
    graphic::IModel *model = mesh ? graphic3->createModel(mesh) : nullptr;
    graphic::ICamera *camera = graphic3
        ? graphic3->createCamera({110.f, 80.f, 110.f}, {0.f, 0.f, 0.f}, 45.f)
        : nullptr;
    // la peau du modele, distincte de celle du sprite
    graphic::ITexture *foxTexture = graphic3
        ? graphic3->createTexture(assets + "/fox2.png")
        : nullptr;

    Scene scene{
        .window     = *window,
        .window3    = window3,
        .keyboard   = *keyboard,
        .mouse      = *mouse,
        .gamepad    = gamepad,
        .texture    = *texture,
        .sprite     = *sprite,
        .font       = *font,
        .hud        = *hud,
        .label      = *label,
        .poly       = *poly,
        .star       = *star,
        .buffer     = *buffer,
        .sound1     = *sound1,
        .sound2     = *sound2,
        .music      = *music,
        .mesh       = mesh,
        .animations = animations,
        .model      = model,
        .camera     = camera,
        .modelTexture = foxTexture,
    };

    const int code = runScene(scene);

    /* Ici la destruction est manuelle et dans l'ordre inverse : chaque
     * createXxx a sa paire deleteXxx, et le delete doit repartir dans la
     * dll qui a fait le new. C'est exactement ce que static.cpp obtient
     * gratuitement avec unique_ptr. */
    if (graphic3) {
        graphic3->deleteTexture(foxTexture);
        graphic3->deleteCamera(camera);
        graphic3->deleteModel(model);
        graphic3->deleteAnimationSet(animations);
        graphic3->deleteMesh(mesh);
    }

    audio->deleteMusic(music);
    audio->deleteSound(sound2);
    audio->deleteSound(sound1);
    audio->deleteSoundBuffer(buffer);

    graphic->deletePolygon(star);
    graphic->deletePolygon(poly);
    graphic->deleteText(label);
    graphic->deleteText(hud);
    graphic->deleteFont(font);
    graphic->deleteSprite(sprite);
    graphic->deleteTexture(texture);

    graphic->deleteGamepad(gamepad);
    graphic->deleteMouse(mouse);
    graphic->deleteKeyboard(keyboard);
    graphic->deleteWindow(window);

    return code;
}
