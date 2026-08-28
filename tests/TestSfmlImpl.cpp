#include <gtest/gtest.h>

#include "IModuleManager.hpp"
#include "IGraphic2Module.hpp"
#include "IGraphic3Module.hpp"
#include "IAudioModule.hpp"

#include <string>

struct SfmlImplTest : ::testing::Test {
    IModuleManager modules;
};

TEST_F(SfmlImplTest, LoadsGraphic2AndAudioButNotGraphic3) {
    ASSERT_TRUE(modules.Load(SFML_IMPL_PATH, "sfml"));

    IGraphic2Module *graphic2 = modules.Get<IGraphic2Module>("sfml");
    IGraphic3Module *graphic3 = modules.Get<IGraphic3Module>("sfml");
    IAudioModule *audioModule = modules.Get<IAudioModule>("sfml");

    ASSERT_NE(graphic2, nullptr);
    ASSERT_NE(audioModule, nullptr);
    EXPECT_STREQ(graphic2->name(), "sfml");

    // no 3D : partial coverage is not an error, just a nullptr
    EXPECT_EQ(graphic3, nullptr);
}

TEST_F(SfmlImplTest, FullWalkthrough) {
    ASSERT_TRUE(modules.Load(SFML_IMPL_PATH, "sfml"));
    IGraphic2Module *graphic = modules.Get<IGraphic2Module>("sfml");
    IAudioModule *audio = modules.Get<IAudioModule>("sfml");
    ASSERT_NE(graphic, nullptr);
    ASSERT_NE(audio, nullptr);

    graphic::IWindow2 *window = graphic->createWindow(200, 200, "sfml_impl test");
    ASSERT_NE(window, nullptr);
    window->setFrameLimit(60);

    // texture/font are shared, independent of the sprites/text built from them
    graphic::ITexture *texture = graphic->createTexture(std::string(ASSETS_DIR) + "/image.png");
    graphic::IFont *font = graphic->createFont(std::string(ASSETS_DIR) + "/font.ttf");
    ASSERT_TRUE(texture->isReady());
    ASSERT_TRUE(font->isReady());

    graphic::ISprite *sprite = graphic->createSprite(texture);
    graphic::IText *text = graphic->createText("hello sfml_impl", font);
    graphic::IPolygon *polygon = graphic->createPolygon({{0, 100}, {100, 0}, {200, 100}, {100, 200}});
    ASSERT_NE(sprite, nullptr);
    ASSERT_NE(text, nullptr);
    ASSERT_NE(polygon, nullptr);

    // deleting a sprite never touches the texture it came from
    graphic->deleteSprite(sprite);
    sprite = graphic->createSprite(texture);
    ASSERT_TRUE(texture->isReady());

    graphic::IKeyboard *keyboard = graphic->createKeyboard(window);
    graphic::IMouse *mouse = graphic->createMouse(window);
    graphic::IGamepad *gamepad = graphic->createGamepad(window);
    ASSERT_NE(keyboard, nullptr);
    ASSERT_NE(mouse, nullptr);
    ASSERT_NE(gamepad, nullptr);

    audio::ISoundBuffer *soundBuffer = audio->createSoundBuffer(std::string(ASSETS_DIR) + "/step.wav");
    audio::IMusic *music = audio->createMusic(std::string(ASSETS_DIR) + "/ambience.wav");
    ASSERT_TRUE(soundBuffer->isReady());
    ASSERT_TRUE(music->isReady());

    audio::ISound *sound = audio->createSound(soundBuffer);
    ASSERT_NE(sound, nullptr);
    sound->setPosition({1, 2, 3}); // sfml genuinely supports this, unlike raylib

    // a handful of frames, bounded - not a real-time loop
    for (int frame = 0; frame < 5; frame++) {
        window->pollEvent();
        window->eventClose();
        keyboard->isKeyDown(graphic::IKeyboard::KEY_SPACE);
        mouse->getPosition();

        window->beginDraw();
        window->drawPoly(polygon);
        window->drawSprite(sprite);
        window->drawText(text);
        window->endDraw();

        music->update();
    }

    sound->play();
    music->play();

    audio->deleteSound(sound);
    audio->deleteSoundBuffer(soundBuffer);
    audio->deleteMusic(music);

    graphic->deletePolygon(polygon);
    graphic->deleteSprite(sprite);
    graphic->deleteText(text);
    graphic->deleteTexture(texture);
    graphic->deleteFont(font);

    graphic->deleteKeyboard(keyboard);
    graphic->deleteMouse(mouse);
    graphic->deleteGamepad(gamepad);
    graphic->deleteWindow(window);
}


/**
 * @brief L'inverse de la chaine accepts : la 2D ne fournit pas la 3D.
 *
 * IGraphic3Module::accepts ne contient que "graphic3", donc un vendor qui
 * declare "graphic2" reste invisible a une demande de 3D. Une couverture
 * partielle est un nullptr, pas une erreur.
 */
TEST_F(SfmlImplTest, AGraphic2VendorNeverAnswersAGraphic3Request) {
    ASSERT_TRUE(modules.Load(SFML_IMPL_PATH, "sfml"));

    ASSERT_NE(modules.Get<IGraphic2Module>("sfml"), nullptr);
    EXPECT_EQ(modules.Get<IGraphic3Module>("sfml"), nullptr);

    EXPECT_EQ(modules.GetAllByType<IGraphic2Module>().size(), 1u);
    EXPECT_TRUE(modules.GetAllByType<IGraphic3Module>().empty());
}
