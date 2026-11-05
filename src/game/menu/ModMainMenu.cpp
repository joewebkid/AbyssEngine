#include "game/menu/ModMainMenu.h"
#include "game/world/Galaxy.h"
#include "game/world/SolarSystem.h"
#include "game/world/Level.h"
#include "game/world/StarSystem.h"
#include "game/core/CutScene.h"
#include "engine/render/PaintCanvas.h"
#include "game/core/Globals.h"
#include "engine/audio/FModSound.h"
#include "game/ui/MenuTouchWindow.h"
#include "game/ui/Layout.h"
#include "game/mission/Status.h"
#include "game/mission/RecordHandler.h"
#include "game/mission/GameRecord.h"
#include "engine/core/ApplicationManager.h"
#include "engine/core/GameText.h"
#include "engine/render/ImageFactory.h"
#include "engine/core/AERandom.h"
#include <cstring>

int FModSound_tryToStopMusicForBGMusic();

namespace AbyssEngine {
    namespace AEMath {
        float Sinf(float value);
    }
}

ModMainMenu::ModMainMenu() {
    this->initialized = 0;
    this->hasSavedGame = 0;
    this->state = 100;
    this->frameTime = 0;
    this->touchWindow = nullptr;
    this->cutScene = nullptr;
}

ModMainMenu::~ModMainMenu() {
    OnRelease();
}

long long ModMainMenu::OnKeyPress(long long key, long long mod) {
    (void) mod;
    return key;
}

long long ModMainMenu::OnKeyRelease(long long key, long long mod) {
    (void) mod;
    return key;
}

int ModMainMenu::ShowLoadingScreen() {
    return 1;
}

void ModMainMenu::OnRelease() {
    if (this->cutScene != nullptr)
        delete this->cutScene;
    this->cutScene = nullptr;

    if (this->touchWindow != nullptr)
        delete this->touchWindow;
    this->touchWindow = nullptr;

    Globals::Canvas->ReleaseAllResources();

    static_cast<Globals *>(Globals::globals)->loadFont(GameText::getLanguage());

    if (Globals::layout != nullptr) {
        Globals::layout->reload();
        static_cast<ImageFactory *>(Globals::imageFactory)->reload();
        Globals::layout->initTip();
    }

    if (Globals::sound != nullptr)
        Globals::sound->freeAllEvents();
}

void ModMainMenu::OnResume() {
    FModSound **soundSlot = &Globals::sound;
    if (*soundSlot == nullptr)
        return;
    if ((*soundSlot)->tryToStopMusicForBGMusic() != 0)
        return;
    float volume;
    std::memcpy(&volume, Globals::options, sizeof(volume));
    (*soundSlot)->setVolume(1, volume);
}

void ModMainMenu::OnRender3D() {
    PaintCanvas::gCanvas->ClearBuffer(0);
    this->cutScene->renderBG();
    PaintCanvas::gCanvas->Begin3d();
    this->cutScene->render3D();
    PaintCanvas::gCanvas->End3d();
}

void ModMainMenu::OnTouchMove(int x, int y, void *touch) {
    if (this->logoActive != 0)
        return;
    this->touchWindow->OnTouchMove(x, y, touch);
}

void ModMainMenu::OnTouchMove(int x, int y) {
    (void) x;
    (void) y;
}

void ModMainMenu::OnSuspend() {
    if (Globals::recordHandler != nullptr)
        static_cast<RecordHandler *>(Globals::recordHandler)->saveOptions();
}

void ModMainMenu::OnTouchEnd(int x, int y, void *touch) {
    if (this->logoActive == 0) {
        this->touchWindow->OnTouchEnd(x, y, touch);
        Level *level = *(Level **) this->cutScene;
        ((StarSystem *) (intptr_t) level->getStarSystem())->initLight();
        return;
    }

    this->logoActive = 0;
    Globals::logoIsShown = 0;
}

void ModMainMenu::OnTouchEnd(int x, int y) {
    (void) x;
    (void) y;
}

void ModMainMenu::OnTouchBegin(int x, int y, void *touch) {
    if (this->logoActive != 0)
        return;
    this->touchWindow->OnTouchBegin(x, y, touch);
}

void ModMainMenu::OnTouchBegin(int x, int y) {
    (void) x;
    (void) y;
}

void ModMainMenu::OnRender2D() {
    PaintCanvas *canvas = sizeof(void *) == 4
                              ? reinterpret_cast<PaintCanvas *>((intptr_t) this->paintCanvas)
                              : Globals::Canvas;
    canvas->Begin2d();
    canvas->SetColor(0xffffffffu);
    this->cutScene->render2D();

    if (this->logoActive == 0) {
        this->touchWindow->draw();
    } else {
        int time = this->fadeTimer;
        int color = time <= 0x0f3b
                        ? (int) (((float) time / 3900.0f) * 255.0f) - 0x100
                        : -1;
        canvas->SetColor((unsigned int) color);

        Globals::Canvas->DrawImage2D(this->logoImage, 0, 0, (unsigned char) 0x44,
                                     (unsigned char) 0x44);

        if (this->fadeTimer >= 0x0f3c) {
            float pulseSign = AbyssEngine::AEMath::Sinf(
                (float) static_cast<ApplicationManager *>(Globals::appManager)->GetSystemTimeMillis() * 0.003f);
            float pulseValue = AbyssEngine::AEMath::Sinf(
                (float) static_cast<ApplicationManager *>(Globals::appManager)->GetSystemTimeMillis() * 0.003f);
            float pulse = pulseSign > 0.0f ? pulseValue : -pulseValue;
            unsigned int alpha = (unsigned int) (pulse * 255.0f);
            Globals::Canvas->SetColor(0xff, 0xff, 0xff, (unsigned char) alpha);

            AbyssEngine::String *drawText = Globals::gameText->getText(0xc7);
            int textWidth = Globals::Canvas->GetTextWidth(Globals::font,
                *Globals::gameText->getText(0xc7));
            int imageHeight = Globals::Canvas->GetImage2DHeight(this->logoImage);
            int x = (Globals::w >> 1) - (textWidth >> 1);
            int y = (Globals::h >> 1) + (imageHeight >> 1) + 10;
            Globals::Canvas->DrawString(Globals::font, *drawText, x, y, false);
        }
    }

    canvas->End2d();
}

int ModMainMenu::OnInitialize() {
    if (this->cutScene == nullptr) {
        // ARM 0x1758f0 uses Globals::globals and the addSoundResourceToList GOT entry.
        static_cast<Globals *>(Globals::globals)->startNewSoundResourceList();
        static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x7e);
        static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x15);
        static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x12);
        static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x13);
        static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x14);

        Status::gStatus->resetGame();
        AERandom::gRandom->reset();

        Station *station = Galaxy::gGalaxy->getStation(AERandom::gRandom->nextInt(100));
        Status::gStatus->setStation(station);

        CutScene *cutscene = new CutScene(2);
        this->cutScene = cutscene;
        cutscene->initialize();

        PaintCanvas *canvas = sizeof(void *) == 4
                                  ? reinterpret_cast<PaintCanvas *>((intptr_t)this->paintCanvas)
                                  : PaintCanvas::gCanvas;
        int texture;
        if (Status::gStatus->inAlienOrbit() != 0) {
            texture = 0x2f08;
        } else {
            texture = (((SolarSystem *) (intptr_t) Status::gStatus->getSystem())->getTextureIndex() +
                       0x2efe) &
                      0xffff;
        }
        unsigned int texSlot = 0xffffffff;
        canvas->TextureCreate((unsigned short) texture, texSlot, false);
        canvas->ChangeCubeTexture(texSlot);
        return 100;
    }

    int state = this->state;
    if (state == 0x1e)
        goto state30;
    if (state == 0x3c)
        goto state60;
    if (state == 0x50)
        goto state80;
    if (state != 100)
        goto music;

    {
        if (Globals::options[0x48] == 0) {
            RecordHandler *recordHandler = static_cast<RecordHandler *>(Globals::recordHandler);
            void *record = recordHandler->recordStoreReadPreview(0);
            if (record != nullptr) {
                this->hasSavedGame = 1;
                delete (GameRecord *) record;
            }
            Globals::options[0x48] = 1;
            recordHandler->saveOptions();
        }

        Status::gStatus->setPlayingTime(0);
        MenuTouchWindow *window = new MenuTouchWindow(0);
        this->touchWindow = window;
        if (this->hasSavedGame != 0) {
            window->showSupernovaMessage();
            this->hasSavedGame = 0;
        }
        this->state = 0x50;
    }
    return 80;

state60:
    this->state = 0x1e;
    return 30;

state30:
    this->state = 1;

music: {
        int music = Globals::switch_to_target_setting;
        if (music != -1)
            static_cast<Globals *>(Globals::globals)->playMusicAndFadeOutCurrent(music);
        Globals::switch_to_target_setting = -1;
    }
    this->initialized = 1;
    this->state = 100;
    return 0;

state80:
    unsigned int logoImageHandle;
    PaintCanvas::gCanvas->Image2DCreate(0x1b5a, logoImageHandle);
    this->logoImage = logoImageHandle;
    this->logoActive = 1;
    this->fadeTimer = 0;
    Globals::logoIsShown = 1;
    this->state = 0x3c;
    return 60;
}

void ModMainMenu::OnUpdate() {
    ApplicationManager *app = (ApplicationManager *) this->appManager;
    int elapsed = (int) app->GetElapsedTimeMillis();
    int frameTime;
    if (elapsed < 0x97 && (elapsed = (int) app->GetElapsedTimeMillis()) < 0) {
        frameTime = 0;
    } else {
        elapsed = (int) app->GetElapsedTimeMillis();
        if (elapsed > 0x96)
            frameTime = 0x96;
        else
            frameTime = (int) app->GetElapsedTimeMillis();
    }

    this->frameTime = frameTime;

    Layout **layoutSlot = &Globals::layout;
    (*layoutSlot)->update(frameTime);
    this->cutScene->update();
    if (this->logoActive == 0)
        this->touchWindow->update(this->frameTime);
    (*layoutSlot)->update(this->frameTime);

    this->fadeTimer = this->frameTime + this->fadeTimer;
    Globals::sound->updateAll(nullptr, nullptr, nullptr, nullptr);
}
