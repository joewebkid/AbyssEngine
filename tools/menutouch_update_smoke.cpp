// Real extracted update body; resources, constructors and service calls are doubles.
#include "game/ui/MenuTouchWindow.h"
#include "game/core/Globals.h"
#include "game/ui/ChoiceWindow.h"
#include "game/ui/MissionsWindow.h"
#include "game/mission/Status.h"
#include "game/mission/RecordHandler.h"
#include "engine/core/GameText.h"
#include "engine/render/Engine.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

unsigned char Globals::options[100] = {};
unsigned char Globals::isTelekomCustomer = 0;
void *Globals::appManager = nullptr;
void *Globals::recordHandler = nullptr;
Status *Globals::status = nullptr;
GameText *Globals::gameText = nullptr;
Layout *Globals::layout = nullptr;
PaintCanvas *Globals::Canvas = nullptr;
int Globals::h = 600;
int Globals::w = 800;

static unsigned char appData[0x50];
static int textId, saves, visibleSystem, choiceDt, scrollDt, missionDt, queriedImage;
// Fixture constructors establish C++ object lifetime without loading resources.
ChoiceWindow::ChoiceWindow() {}
ChoiceWindow::~ChoiceWindow() = default;
ScrollTouchWindow::ScrollTouchWindow(int, int, int, int) {}
ScrollTouchWindow::~ScrollTouchWindow() = default;
MissionsWindow::MissionsWindow() {}
MissionsWindow::~MissionsWindow() = default;
Status::Status() {}
Status::~Status() = default;
GameText::GameText() {}
GameText::~GameText() = default;
RecordHandler::RecordHandler() {}
RecordHandler::~RecordHandler() = default;
Layout::Layout() { std::memset(this, 0, sizeof(*this)); }
Layout::~Layout() = default;
AbyssEngine::PaintCanvas::PaintCanvas(Engine *) {}
AbyssEngine::PaintCanvas::~PaintCanvas() = default;
AbyssEngine::AEMath::Matrix::Matrix() {} // Unexecuted rendering boundary.
AbyssEngine::ApplicationManager::ApplicationManager(Engine *) {}
AbyssEngine::ApplicationManager::~ApplicationManager() = default;
static ChoiceWindow choice;
static ScrollTouchWindow scroll(0, 0, 0, 0);
static MissionsWindow missions;
MenuTouchWindow::MenuTouchWindow(int) {
    std::memset(this, 0, sizeof(*this));
    choiceWindow = &choice;
    scrollWindowA = scrollWindowB = &scroll;
    missionsWindow = &missions;
}
MenuTouchWindow::~MenuTouchWindow() = default;
AbyssEngine::String::String() : data(nullptr), length(0) {}
AbyssEngine::String::~String() = default;
static String text;
String *GameText::getText(int id) { textId = id; return &text; }
void ChoiceWindow::set(const String &) {}
void ChoiceWindow::update(int dt) { choiceDt = dt; }
void ScrollTouchWindow::update(int dt) { scrollDt = dt; }
void MissionsWindow::update(int dt) { missionDt = dt; }
void RecordHandler::saveOptions() { ++saves; }
void Status::setSystemVisibility(int system, bool value) {
    assert(value); visibleSystem = system;
}
void *AbyssEngine::ApplicationManager::GetApplicationData() { return appData; }
void *AbyssEngine::ApplicationManager::GetEngine() {
    std::abort(); // Store-init renderer result is outside this smoke.
}
int PaintCanvas::GetImage2DHeight(unsigned int image) { queriedImage = image; return 30; }
void TouchButton::setYPosition(int y) { this->y = y; }
uint8_t TouchButton::isVisible() { return visible; }
Vector TouchButton::getPosition() { return {float(x), float(y), 0}; }

static void reset() {
    std::memset(appData, 0, sizeof(appData));
    std::memset(Globals::options, 0, sizeof(Globals::options));
    textId = saves = visibleSystem = choiceDt = scrollDt = missionDt = queriedImage = -1;
    Globals::isTelekomCustomer = 0;
}

int main() {
    Status status;
    GameText gameText;
    RecordHandler recordHandler;
    Layout layout;
    AbyssEngine::PaintCanvas canvas(nullptr);
    AbyssEngine::ApplicationManager appManager(nullptr);
    Globals::status = &status;
    Globals::gameText = &gameText;
    Globals::recordHandler = &recordHandler;
    Globals::appManager = &appManager;
    Globals::layout = &layout;
    Globals::Canvas = &canvas;
    for (unsigned int code = 0; code != 5; ++code) {
        reset();
        MenuTouchWindow w(0);
        w.menuState = 5;
        std::memcpy(appData + 0x48, &code, sizeof(code));
        appData[0x41] = 1;
        w.update(37);
        assert(textId == 92 + int(code) && appData[0x41] == 0);
        assert(w.dlcResultDialogShowing && w.messageShowing && choiceDt == 37);
        assert(Globals::options[code == 4 ? 0x39 : 0x35 + code] == 1);
        if (code == 4) assert(Globals::options[0x35] && Globals::options[0x37]);
        if (code == 1) assert(Globals::status->mode_0x114 == 3);
        if (code == 0 || code == 2 || code == 4) assert(visibleSystem == 25);
    }
    reset();
    {
        MenuTouchWindow w(0);
        w.menuState = 5;
        w.dlcMessageShowing = 1;
        unsigned int images[] = {1234};
        w.heapBufA = images;
        w.scrollbarImageId = 9876;
        appData[0x40] = 1;
        layout.field_0xc = 10;
        layout.field_0x10 = 20;
        layout.field_0x20 = 30;
        layout.field_0x24 = 40;
        layout.field_0x2c = 8;
        w.update(29);
        assert(queriedImage == 1234 && w.menuState == 15 && !appData[0x40]);
        assert(w.contentHeight == 500 && w.pageHeight == 190);
        assert(Globals::options[0x3b] == 1 && scrollDt == 29);
    }
    reset();
    {
        MenuTouchWindow w(0);
        w.menuState = 5;
        w.purchaseRestorePending = 1; // Error must work without dlcMessageShowing.
        w.pad_0x17b = 9;
        appData[0x42] = 1;
        w.update(19);
        assert(textId == 100 && appData[0x42] == 0 && !w.purchaseRestorePending);
        assert(w.dlcErrorDialogShowing && !w.pad_0x17b && choiceDt == 19);
    }
    reset();
    {
        MenuTouchWindow w(0);
        w.menuState = 5;
        w.purchaseRestorePending = 1;
        appData[0x41] = 1;
        unsigned int restore = ~0u;
        std::memcpy(appData + 0x48, &restore, sizeof(restore));
        w.update(23);
        assert(textId == 76 && !w.purchaseRestorePending && !appData[0x41]);
    }
    reset();
    {
        MenuTouchWindow w(0);
        w.menuState = 5;
        appData[5] = 1;
        int result = 2;
        std::memcpy(appData + 8, &result, sizeof(result));
        w.update(11);
        assert(appData[5] == 1 && textId == -1); // Screenshot polling only in state 13.
        w.menuState = 13;
        w.update(11);
        assert(appData[5] == 0 && textId == 58 && w.screenshotState == -1);
    }
    reset();
    {
        MenuTouchWindow w(0);
        w.menuState = 4;
        w.update(41);
        assert(scrollDt == 41);
        w.menuState = 9;
        w.update(43);
        assert(missionDt == 43);
    }
    std::puts("MenuTouchWindow update host smoke: OK");
}
