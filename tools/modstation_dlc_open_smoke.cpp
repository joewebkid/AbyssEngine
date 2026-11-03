#include <cassert>

struct Vector { float x, y; };
struct TouchButton {
    float x, y;
    int reads = 0;
    Vector getPosition() { ++reads; return {x, y}; }
};
template<class T> struct Array {
    unsigned count;
    T *data;
    unsigned size() const { return count; }
    T &operator[](unsigned i) { return data[i]; }
};
struct Status {
    int levelChecks = 0;
    void checkForLevelUp() { ++levelChecks; }
};
struct Globals {
    static Status *status;
    static int sub_menu_buttons_x[10];
    static int sub_menu_buttons_y[10];
};
Status *Globals::status = nullptr;
int Globals::sub_menu_buttons_x[10];
int Globals::sub_menu_buttons_y[10];

struct MenuTouchWindow {
    static int constructions;
    static int lastKind;
    static Array<void *> *nextButtons;
    Array<void *> *buttons;
    explicit MenuTouchWindow(int kind) : buttons(nextButtons) {
        ++constructions;
        lastKind = kind;
    }
};
int MenuTouchWindow::constructions = 0;
int MenuTouchWindow::lastKind = -1;
Array<void *> *MenuTouchWindow::nextButtons = nullptr;

int layoutResult = 0, layoutCalls = 0, layoutX = -1, layoutY = -1;
struct Layout {
    int OnTouchEnd(int x, int y) {
        ++layoutCalls;
        layoutX = x;
        layoutY = y;
        return layoutResult;
    }
};
struct ModStation {
    struct Flags { unsigned char bytes[4] = {}; } m_nStarMapWindowOpen;
    void *dlcMenu = nullptr;
    void testDlcOpen(int x, int y);
};

// PRODUCTION_DLC_OPEN

int main() {
    TouchButton buttons[12];
    void *pointers[12];
    for (int i = 0; i < 12; ++i) {
        buttons[i].x = static_cast<float>(i) + 0.75f;
        buttons[i].y = -static_cast<float>(i) - 0.75f;
        pointers[i] = &buttons[i];
    }
    Array<void *> menuButtons{12, pointers};
    MenuTouchWindow::nextButtons = &menuButtons;
    Status status;
    Globals::status = &status;
    ModStation station;
    for (int i = 0; i < 10; ++i) {
        Globals::sub_menu_buttons_x[i] = -123;
        Globals::sub_menu_buttons_y[i] = -123;
    }

    station.testDlcOpen(17, 23);
    assert(layoutCalls == 1 && layoutX == 17 && layoutY == 23);
    assert(station.dlcMenu == nullptr && status.levelChecks == 0);
    assert(station.m_nStarMapWindowOpen.bytes[2] == 0);

    layoutResult = 1;
    station.testDlcOpen(31, 41);
    assert(layoutCalls == 2 && layoutX == 31 && layoutY == 41);
    assert(station.dlcMenu != nullptr);
    assert(MenuTouchWindow::constructions == 1 && MenuTouchWindow::lastKind == 2);
    assert(status.levelChecks == 1 && station.m_nStarMapWindowOpen.bytes[2] == 1);
    for (int i = 0; i < 10; ++i) {
        assert(Globals::sub_menu_buttons_x[i] == i);
        assert(Globals::sub_menu_buttons_y[i] == -i);
        assert(buttons[i].reads == 2);
    }
    assert(buttons[10].reads == 0 && buttons[11].reads == 0);

    station.testDlcOpen(33, 43);
    assert(MenuTouchWindow::constructions == 1 && status.levelChecks == 2);
    for (int i = 0; i < 10; ++i) assert(buttons[i].reads == 4);
    delete static_cast<MenuTouchWindow *>(station.dlcMenu);
}
