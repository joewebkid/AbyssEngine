#include <cassert>
#include <cstdint>

struct Sink {
    int calls = 0;
    int lastX = 0;
    int lastY = 0;
    void OnTouchMove(int x, int y) { ++calls; lastX = x; lastY = y; }
};
struct Layout : Sink {
    unsigned char choiceWindowOpen = 0;
    int field_0xc = 0;
    int field_0x10 = 0;
};
struct DialogueWindow : Sink {};
struct ChoiceWindow : Sink {};
struct HangarWindow : Sink {};
struct StarMap : Sink {};
struct SpaceLounge : Sink {};
struct StatusWindow : Sink {};
struct MissionsWindow : Sink {};
struct TouchButton : Sink {};
struct NewsTicker : Sink { int OnTouchMove(int x, int y) { Sink::OnTouchMove(x, y); return 0; } };
struct MenuTouchWindow : Sink {
    void *lastTouch = nullptr;
    void OnTouchMove(int x, int y, void *touch) {
        Sink::OnTouchMove(x, y);
        lastTouch = touch;
    }
};
struct Globals {
    static Layout *layout;
    static int h;
};
Layout *Globals::layout = nullptr;
int Globals::h = 400;

struct ModStation {
    struct FlagWord { unsigned char bytes[4] = {}; };
    void *activeTouch = nullptr;
    int touchX = 0, touchY = 0;
    FlagWord m_nStarMapWindowOpen, modalFlags, subWindowFlags;
    void *dialogueWindow = nullptr, *choiceWindow = nullptr, *medalChoiceWindow = nullptr;
    void *hangarWindow = nullptr, *spaceLounge = nullptr, *statusWindow = nullptr;
    StarMap *starMap = nullptr;
    MissionsWindow *missionsWindow = nullptr;
    void *dlcMenu = nullptr, *buttonLaunch = nullptr, *buttonCredits = nullptr;
    int *buttonRow = nullptr;
    NewsTicker *newsTicker = nullptr;
    std::intptr_t hangarLeftX = 0;
    int camScrollPos = 0, camScrollVel = 0, field_0xf4 = 0, camAngle = 0;
    void OnTouchMove(int x, int y, void *touch);
};

// PRODUCTION_FUNCTION

int main() {
    int touchToken = 1;
    int otherToken = 2;
    Layout layout;
    DialogueWindow dialogue;
    MenuTouchWindow menu;
    Globals::layout = &layout;
    ModStation station;
    station.activeTouch = &touchToken;
    station.dlcMenu = &menu;
    station.dialogueWindow = &dialogue;
    station.m_nStarMapWindowOpen.bytes[2] = 1;

    station.OnTouchMove(17, 23, &otherToken);
    assert(station.touchX == 0 && menu.calls == 0);
    station.OnTouchMove(17, 23, &touchToken);
    assert(station.touchX == 17 && station.touchY == 23);
    assert(menu.calls == 1 && menu.lastTouch == &touchToken);

    layout.choiceWindowOpen = 1;
    station.OnTouchMove(21, 25, &touchToken);
    assert(layout.calls == 1 && menu.calls == 1);
    layout.choiceWindowOpen = 0;

    station.modalFlags.bytes[1] = 1;
    station.OnTouchMove(31, 37, &touchToken);
    assert(dialogue.calls == 1 && dialogue.lastX == 31 && menu.calls == 1);
    station.modalFlags.bytes[1] = 0;

    station.m_nStarMapWindowOpen.bytes[0] = 1;
    station.OnTouchMove(41, 43, &touchToken);
    assert(station.touchX == 41 && station.touchY == 43 && menu.calls == 1);
}
