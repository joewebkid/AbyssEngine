#include <cassert>
#include <cstdint>

struct Sink {
    int calls = 0;
    int lastX = 0;
    int lastY = 0;
    void OnTouchMove(int x, int y) { ++calls; lastX = x; lastY = y; }
    void OnTouchBegin(int x, int y) { ++calls; lastX = x; lastY = y; }
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
struct NewsTicker : Sink {
    int OnTouchMove(int x, int y) { Sink::OnTouchMove(x, y); return 0; }
    int OnTouchBegin(int x, int y) { Sink::OnTouchBegin(x, y); return 0; }
};
struct MenuTouchWindow : Sink {
    void *lastTouch = nullptr;
    void OnTouchMove(int x, int y, void *touch) {
        Sink::OnTouchMove(x, y);
        lastTouch = touch;
    }
    void OnTouchBegin(int x, int y, void *touch) {
        Sink::OnTouchBegin(x, y);
        lastTouch = touch;
    }
};
struct Radio {
    int finished = 0;
    int calls = 0;
    int lastMessageShown() { ++calls; return finished; }
};
struct Status {
    int calls = 0;
    bool lastFlag = false;
    void nextCampaignMission(bool flag) { ++calls; lastFlag = flag; }
};
struct ApplicationManager {
    unsigned int module = 0;
    int calls = 0;
    void SetCurrentApplicationModule(unsigned int id) { ++calls; module = id; }
};
struct Globals {
    static Layout *layout;
    static int h;
    static Status *status;
    static void *appManager;
    static int switch_to_target_setting;
};
Layout *Globals::layout = nullptr;
int Globals::h = 400;
Status *Globals::status = nullptr;
void *Globals::appManager = nullptr;
int Globals::switch_to_target_setting = 0;

struct ModStation {
    struct FlagWord { unsigned char bytes[4] = {}; };
    void *activeTouch = nullptr;
    int touchX = 0, touchY = 0;
    FlagWord m_nStarMapWindowOpen, modalFlags, subWindowFlags, scrollFlags, dragFlags;
    char stationActive = 1;
    std::intptr_t activeMission = 0;
    void *dialogueWindow = nullptr, *choiceWindow = nullptr, *medalChoiceWindow = nullptr;
    void *hangarWindow = nullptr, *spaceLounge = nullptr, *statusWindow = nullptr;
    StarMap *starMap = nullptr;
    MissionsWindow *missionsWindow = nullptr;
    void *dlcMenu = nullptr, *buttonLaunch = nullptr, *buttonCredits = nullptr;
    int *buttonRow = nullptr;
    NewsTicker *newsTicker = nullptr;
    std::intptr_t hangarLeftX = 0;
    int camScrollPos = 0, camScrollVel = 0, field_0xf4 = 0, camAngle = 0, field_0xfc = 0;
    void OnTouchBegin(int x, int y, void *touch);
    void OnTouchMove(int x, int y, void *touch);
};

// PRODUCTION_BEGIN
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

    Status status;
    ApplicationManager manager;
    Radio radio;
    Globals::status = &status;
    Globals::appManager = &manager;
    Globals::switch_to_target_setting = 7;
    ModStation waitingStation;
    waitingStation.activeMission = reinterpret_cast<std::intptr_t>(&radio);
    waitingStation.m_nStarMapWindowOpen.bytes[0] = 1;
    waitingStation.OnTouchBegin(3, 4, &touchToken);
    assert(radio.calls == 1 && status.calls == 0 && manager.calls == 0);
    assert(Globals::switch_to_target_setting == 7);
    assert(waitingStation.m_nStarMapWindowOpen.bytes[0] == 1);

    radio.finished = 1;
    ModStation radioStation;
    radioStation.activeMission = reinterpret_cast<std::intptr_t>(&radio);
    radioStation.m_nStarMapWindowOpen.bytes[0] = 1;
    radioStation.OnTouchBegin(5, 6, &touchToken);
    assert(radioStation.activeTouch == &touchToken && radioStation.scrollFlags.bytes[0] == 1);
    assert(radioStation.touchX == 5 && radioStation.touchY == 6);
    assert(radio.calls == 2 && status.calls == 1 && status.lastFlag);
    assert(Globals::switch_to_target_setting == 0 && manager.calls == 1 && manager.module == 5);
    assert(radioStation.stationActive == 0 && radioStation.m_nStarMapWindowOpen.bytes[0] == 0);
    radioStation.OnTouchBegin(9, 10, &otherToken);
    assert(radio.calls == 2 && radioStation.touchX == 5);

    ModStation menuStation;
    menuStation.dlcMenu = &menu;
    menuStation.m_nStarMapWindowOpen.bytes[2] = 1;
    menuStation.OnTouchBegin(47, 53, &otherToken);
    assert(menu.calls == 2 && menu.lastTouch == &otherToken);
    assert(menuStation.activeTouch == &otherToken && menuStation.touchY == 53);

    layout.choiceWindowOpen = 1;
    ModStation layoutStation;
    layoutStation.dlcMenu = &menu;
    layoutStation.m_nStarMapWindowOpen.bytes[2] = 1;
    layoutStation.OnTouchBegin(59, 61, &touchToken);
    assert(layout.calls == 2 && menu.calls == 2);
}
