/**
 * @file sfml.cpp
 * @author Perry Chouteau (perry.chouteau@outlook.com)
 * @brief sfml implementation of IGraphic2Module/IAudioModule
 * @date 2026-08-10
 *
 * @addtogroup sfml
 * @{
 */

#ifndef SFML_CPP_
#define SFML_CPP_

#include "SfmlAudioModule.hpp"
#include "SfmlGraphicModule.hpp"

//no getGraphic3Module() : sfml has no native 3D, so it only ever
//implements IGraphic2Module - Get<IGraphic3Module>("sfml") stays nullptr

extern "C" IGraphic2Module *getGraphic2Module() {
    static SfmlGraphicModule instance;
    return &instance;
}

extern "C" IAudioModule *getAudioModule() {
    static SfmlAudioModule instance;
    return &instance;
}

/**
 * @brief Tout ce que cette dll fournit, sans avoir a nommer un contrat.
 *
 * Le point d'entree d'AnonymousModuleManager : un nom unique, le meme pour
 * toutes les dll, donc trouvable sans connaitre les contrats a l'avance.
 * Le chargeur trie ensuite par IModule::type().
 *
 * IModule ** termine par nullptr, et pas un std::vector : un vecteur ne
 * traverse pas un dlopen sans supposer la meme ABI de bibliotheque standard
 * des deux cotes. La conversion vers IModule * se fait ICI, ou le type
 * complet est connu, donc le decalage eventuel est applique par le
 * compilateur et pas devine par l'appelant.
 *
 * Le tableau est statique : il appartient a la dll et vit aussi longtemps
 * qu'elle.
 */
extern "C" IModule **getModules() {
    static IModule *list[] = {
        getGraphic2Module(),
        getAudioModule(),
        nullptr
    };

    return list;
}

/** @} */

#endif /* !SFML_CPP_ */
