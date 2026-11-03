#include <cassert>
#include <cstdint>

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
struct Globals {
    static int sub_menu_buttons_x[10];
    static int sub_menu_buttons_y[10];
};
int Globals::sub_menu_buttons_x[10];
int Globals::sub_menu_buttons_y[10];
struct MenuTouchWindow {
    int result = 0;
    int calls = 0;
    int lastX = -1, lastY = -1;
    void *lastTouch = nullptr;
    int OnTouchEnd(int x, int y, void *touch) {
        ++calls;
        lastX = x;
        lastY = y;
        lastTouch = touch;
        return result;
    }
};
struct ModStation {
    struct Flags { unsigned char bytes[4] = {}; } m_nStarMapWindowOpen;
    void *dlcMenu = nullptr;
    int *buttonRow = nullptr;
    void testDlcClose(int x, int y, void *touch);
};

// PRODUCTION_DLC_CLOSE

int main() {
    TouchButton buttons[12];
    TouchButton *pointers[12];
    for (int i = 0; i < 12; ++i) {
        buttons[i].x = static_cast<float>(i) + 0.75f;
        buttons[i].y = -static_cast<float>(i) - 0.75f;
        pointers[i] = &buttons[i];
    }
    Array<TouchButton *> row{12, pointers};
    MenuTouchWindow menu;
    ModStation station;
    station.dlcMenu = &menu;
    station.buttonRow = reinterpret_cast<int *>(&row);
    station.m_nStarMapWindowOpen.bytes[2] = 1;
    for (int i = 0; i < 10; ++i) {
        Globals::sub_menu_buttons_x[i] = -123;
        Globals::sub_menu_buttons_y[i] = -123;
    }
    int touch = 7;
    station.testDlcClose(17, 23, &touch);
    assert(menu.calls == 1 && menu.lastX == 17 && menu.lastY == 23);
    assert(menu.lastTouch == &touch && station.m_nStarMapWindowOpen.bytes[2] == 1);
    for (int i = 0; i < 10; ++i) {
        assert(Globals::sub_menu_buttons_x[i] == -123);
        assert(Globals::sub_menu_buttons_y[i] == -123);
        assert(buttons[i].reads == 0);
    }
    menu.result = 1;
    station.testDlcClose(31, 41, &touch);
    assert(menu.calls == 2 && menu.lastX == 31 && menu.lastY == 41);
    assert(station.m_nStarMapWindowOpen.bytes[2] == 0);
    for (int i = 0; i < 10; ++i) {
        assert(Globals::sub_menu_buttons_x[i] == i);
        assert(Globals::sub_menu_buttons_y[i] == -i);
        assert(buttons[i].reads == 2);
    }
    assert(buttons[10].reads == 0 && buttons[11].reads == 0);
}
