#include "game/menu/MTitle.h"
#include "engine/core/GameText.h"
#include "engine/audio/FModSound.h"
#include "engine/core/ApplicationManager.h"
#include "engine/render/ImageFactory.h"
#include "game/ui/Layout.h"
#include "engine/render/PaintCanvas.h"
#include "game/core/Globals.h"

static Layout *g_currentLayout = nullptr;
static ImageFactory *g_imageFactory = nullptr;
static FModSound *g_sound = nullptr;

MTitle::MTitle() {
    this->renderPriority = 100;
}

MTitle::~MTitle() {
    OnRelease();
}

int MTitle::OnInitialize() {
    PaintCanvas *canvas = PaintCanvas::gCanvas;
    canvas->Image2DCreate(7000, this->logoImage);
    canvas->Image2DCreate(0x1b59, this->logoImage2);

    g_sound->play(0x91, (Vector *) 0, (Vector *) 0, 0.0f);

    this->step = 0;
    this->timer = 0;
    this->renderPriority = 100;
    return 0;
}

void MTitle::OnRelease() {
    PaintCanvas::gCanvas->ReleaseAllResources();

    Globals::gGlobals->loadFont(GameText::getLanguage());

    if (g_currentLayout != 0) {
        g_currentLayout->reload();
        g_imageFactory->reload();
    }
}

long long MTitle::OnKeyPress(long long key, long long mod) {
}

long long MTitle::OnKeyRelease(long long key, long long mod) {
}

void MTitle::OnTouchBegin(int x, int y) {
}

void MTitle::OnTouchMove(int x, int y) {
}

void MTitle::OnUpdate() {
}

void MTitle::OnRender2D() {
    PaintCanvas *canvas = PaintCanvas::gCanvas;
    canvas->Begin2d();
    canvas->SetColor((unsigned int) -1);

    g_currentLayout->drawBG();
    g_currentLayout->drawHeader();
    g_currentLayout->drawEmptyFooter(0);

    int delta = (int) this->applicationManager->GetElapsedTimeMillis();
    if (delta > 50)
        delta = 50;
    this->timer += delta;
    int t = this->timer;

    int image;
    float fade;
    if (t > 4000) {
        this->step += 1;
        this->timer = 0;
        if (this->step == 2) {
            this->applicationManager->SetCurrentApplicationModule(1);
            return;
        }
        image = (int) (this->step == 0 ? this->logoImage2 : this->logoImage);
        fade = 0.0f;
    } else {
        image = (int) (this->step == 0 ? this->logoImage2 : this->logoImage);
        if (t < 1000) {
            fade = (float) t / 1000.0f;
        } else if (t < 3001) {
            fade = 1.0f;
        } else {
            fade = (float) (t - 3000) / -1000.0f + 1.0f;
        }
    }

    int color = (int) (fade * 255.0f) - 0x100;
    canvas->SetColor((unsigned int) color);
    canvas->DrawImage2D((unsigned int) image, 0, 0, (unsigned char) 0, (unsigned char) 0x44);
    canvas->End2d();
}

void MTitle::OnRender3D() {
    PaintCanvas *canvas = PaintCanvas::gCanvas;
    canvas->ClearBuffer(0xff);
    canvas->Begin3d();
}

void MTitle::OnTouchEnd(int x, int y) {
    this->timer = 5000;
}

int MTitle::ShowLoadingScreen() {
    return 0;
}
