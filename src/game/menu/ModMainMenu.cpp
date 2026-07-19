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

int FModSound_tryToStopMusicForBGMusic();

namespace AbyssEngine {
    namespace AEMath {
        float Sinf(float value);
    }
}

void **g_ModMainMenu_releaseReload;
void **g_ModMainMenu_releaseImageFactory;
void **g_ModMainMenu_releaseSound;

int *g_ModMainMenu_resumeObj;
int *g_ModMainMenu_resumeArg;

int *g_ModMainMenu_suspendObj;

int *g_ModMainMenu_touchEndFlag;

AbyssEngine::String **g_ModMainMenu_r2d_string;
int *g_ModMainMenu_r2d_textId;
int *g_ModMainMenu_r2d_screenW;
int *g_ModMainMenu_r2d_screenH;

void **g_ModMainMenu_initSoundRes;

void (*g_ModMainMenu_initAddSound)(void *, int);

void *g_ModMainMenu_initOptions;
void **g_ModMainMenu_initRecord;
int *g_ModMainMenu_initTouchFlag;
int *g_ModMainMenu_initMusicSlot;
int *g_ModMainMenu_initMusic;

void **g_ModMainMenu_updateLayout;
void **g_ModMainMenu_updateListener;

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

    PaintCanvas::gCanvas->ReleaseAllResources();

    Globals::gGlobals->loadFont(GameText::getLanguage());

    void **reload = g_ModMainMenu_releaseReload;
    if (*reload != nullptr) {
        ((Layout *) (*reload))->reload();
        ((ImageFactory *) (*g_ModMainMenu_releaseImageFactory))->reload();
        ((Layout *) (*reload))->initTip();
    }

    void *sound = *g_ModMainMenu_releaseSound;
    if (sound != nullptr)
        ((FModSound *) sound)->freeAllEvents();
}

void ModMainMenu::OnResume() {
    int *holder = g_ModMainMenu_resumeObj;
    if (*holder == 0)
        return;
    if (FModSound_tryToStopMusicForBGMusic() != 0)
        return;
    int arg = *g_ModMainMenu_resumeArg;
    ((FModSound *) (intptr_t) * holder)->setVolume(1, (float) arg);
}

void ModMainMenu::OnRender3D() {
    PaintCanvas::gCanvas->ClearBuffer(0);
    this->cutScene->renderBG();
    PaintCanvas::gCanvas->Begin3d();
    this->cutScene->render3D();
    PaintCanvas::gCanvas->End3d();
}

void ModMainMenu::OnTouchMove(int x, int y, void *touch) {
    (void) touch;
    if (this->logoActive != 0)
        return;
    this->touchWindow->OnTouchMove(x, y, nullptr);
}

void ModMainMenu::OnTouchMove(int x, int y) {
    (void) x;
    (void) y;
}

void ModMainMenu::OnSuspend() {
    int obj = *g_ModMainMenu_suspendObj;
    if (obj != 0)
        ((RecordHandler *) (intptr_t) obj)->saveOptions();
}

void ModMainMenu::OnTouchEnd(int x, int y, void *touch) {
    (void) touch;
    if (this->logoActive == 0) {
        this->touchWindow->OnTouchEnd(x, y, nullptr);
        Level *level = *(Level **) this->cutScene;
        ((StarSystem *) (intptr_t) level->getStarSystem())->initLight();
        return;
    }

    this->logoActive = 0;
    *g_ModMainMenu_touchEndFlag = 0;
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
    ((PaintCanvas *) (long) this->paintCanvas)->Begin2d();
    ((PaintCanvas *) (long) this->paintCanvas)->SetColor((unsigned int) this->paintCanvas);
    this->cutScene->render2D();

    if (this->logoActive == 0) {
        this->touchWindow->draw();
    } else {
        int time = this->fadeTimer;
        int color = time <= 0x0f3b
                        ? (int) (((float) time / 3900.0f) * 255.0f) - 0x100
                        : -1;
        (void) color;
        ((PaintCanvas *) (long) this->paintCanvas)->SetColor((unsigned int) this->paintCanvas);

        PaintCanvas::gCanvas->DrawImage2D((unsigned int) this->logoImage, 0, 0, (unsigned char) 'D');

        if (this->fadeTimer >= 0x0f3c) {
            int canvas = (int) (intptr_t) PaintCanvas::gCanvas;
            float pulse = AbyssEngine::AEMath::Sinf(
                (float) ApplicationManager::gAppManager->GetSystemTimeMillis() * 0.003f);
            float signedPulse = pulse > 0.0f ? pulse : -pulse;
            int alpha = (unsigned int) (signedPulse * 255.0f);
            (void) alpha;
            PaintCanvas::gCanvas->SetColor((unsigned char) canvas, 0xff, 0xff, 0xff);

            AbyssEngine::String **stringHolder = g_ModMainMenu_r2d_string;
            int *textIdHolder = g_ModMainMenu_r2d_textId;
            int drawCanvas = (int) (intptr_t) PaintCanvas::gCanvas;
            AbyssEngine::String *drawStr = *stringHolder;
            int text = (int) (long) ((GameText *) (*textIdHolder))->getText(0xc7);

            int screenW = *g_ModMainMenu_r2d_screenW;
            int textWidth = PaintCanvas::gCanvas->GetTextWidth((unsigned int) drawCanvas, *drawStr);

            int screenH = *g_ModMainMenu_r2d_screenH;
            int imageHeight = PaintCanvas::gCanvas->GetImage2DHeight((unsigned int) (intptr_t) PaintCanvas::gCanvas);
            PaintCanvas::gCanvas->DrawString((unsigned int) drawCanvas, *drawStr, text,
                                (screenW >> 1) - (textWidth >> 1),
                                (bool) ((char) (screenH >> 1) + (char) (imageHeight >> 1) + '\n'));
        }
    }

    ((PaintCanvas *) (intptr_t) this->paintCanvas)->End2d();
}

void ModMainMenu::OnInitialize() {
    if (this->cutScene == nullptr) {
        void **soundRes = g_ModMainMenu_initSoundRes;
        static_cast<Globals *>(*soundRes)->startNewSoundResourceList();
        void (*addSound)(void *, int) = g_ModMainMenu_initAddSound;
        addSound(*soundRes, 0x7e);
        addSound(*soundRes, 0x15);
        addSound(*soundRes, 0x12);
        addSound(*soundRes, 0x13);
        addSound(*soundRes, 0x14);

        Status::gStatus->resetGame();
        AERandom::gRandom->reset();

        int station = Galaxy::gGalaxy->getStation(AERandom::gRandom->nextInt(100));
        Status::gStatus->setStation((Station *) (intptr_t) station);

        CutScene *cutscene = new CutScene(2);
        this->cutScene = cutscene;
        cutscene->initialize();

        int canvas = this->paintCanvas;
        int texture;
        if (Status::gStatus->inAlienOrbit() != 0) {
            texture = 0x2f08;
        } else {
            texture = (((SolarSystem *) (intptr_t) Status::gStatus->getSystem())->getTextureIndex() +
                       0x2efe) &
                      0xffff;
        }
        unsigned int texSlot = 0xffffffff;
        ((PaintCanvas *) (long) canvas)->TextureCreate((unsigned short) texture, texSlot, true);
        ((PaintCanvas *) (long) this->paintCanvas)->ChangeCubeTexture((unsigned int) this->paintCanvas);
        return;
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
        OptionsRecord *options = (OptionsRecord *) g_ModMainMenu_initOptions;
        if (options->firstRunPreviewChecked == 0) {
            void **recordHolder = g_ModMainMenu_initRecord;
            void *record = ((RecordHandler *) (*recordHolder))->recordStoreReadPreview(0);
            if (record != nullptr) {
                this->hasSavedGame = 1;
                delete (GameRecord *) record;
            }
            options->firstRunPreviewChecked = 1;
            ((RecordHandler *) (*recordHolder))->saveOptions();
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
    return;

state60:
    this->state = 0x1e;
    return;

state30:
    this->state = 1;

music: {
        int *musicSlot = g_ModMainMenu_initMusicSlot;
        if (*musicSlot != -1)
            Globals::gGlobals->playMusicAndFadeOutCurrent(*g_ModMainMenu_initMusic);
        *musicSlot = -1;
    }
    this->initialized = 1;
    this->state = 100;
    return;

state80:
    unsigned int logoImageHandle;
    PaintCanvas::gCanvas->Image2DCreate(0x1b5a, logoImageHandle);
    this->logoImage = logoImageHandle;
    this->logoActive = 1;
    this->fadeTimer = 0;
    *g_ModMainMenu_initTouchFlag = 1;
    this->state = 0x3c;
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

    void **layout = g_ModMainMenu_updateLayout;
    ((Layout *) (*layout))->update(frameTime);
    this->cutScene->update();
    if (this->logoActive == 0)
        this->touchWindow->update(this->frameTime);
    ((Layout *) (*layout))->update(this->frameTime);

    void **listener = g_ModMainMenu_updateListener;
    this->fadeTimer = this->frameTime + this->fadeTimer;
    ((FModSound *) (*listener))->updateAll(nullptr, nullptr, nullptr, nullptr);
}
