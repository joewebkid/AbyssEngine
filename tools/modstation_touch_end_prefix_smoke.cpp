#include <cassert>

struct Layout {
    unsigned char choiceWindowOpen = 0;
    int result = 0;
    int calls = 0;
    int lastX = 0;
    int lastY = 0;
    int OnTouchEnd(int x, int y) {
        ++calls;
        lastX = x;
        lastY = y;
        return result;
    }
};
struct Globals { static Layout *layout; };
Layout *Globals::layout = nullptr;

struct ModStation {
    struct FlagWord { unsigned char bytes[4] = {}; };
    void *activeTouch = nullptr;
    FlagWord m_nStarMapWindowOpen, scrollFlags;
    void OnTouchEnd(int x, int y, void *touch);
};

// PRODUCTION_PREFIX

int main() {
    int touch = 1;
    int other = 2;
    Layout layout;
    Globals::layout = &layout;
    ModStation station;
    station.activeTouch = &touch;
    station.scrollFlags.bytes[0] = 1;
    layout.choiceWindowOpen = 1;

    station.OnTouchEnd(10, 20, &other);
    assert(station.activeTouch == &touch && station.scrollFlags.bytes[0] == 1);
    assert(layout.calls == 0);

    station.m_nStarMapWindowOpen.bytes[0] = 1;
    station.OnTouchEnd(10, 20, &touch);
    assert(station.activeTouch == nullptr && station.scrollFlags.bytes[0] == 0);
    assert(layout.calls == 0 && layout.choiceWindowOpen == 1);

    station.m_nStarMapWindowOpen.bytes[0] = 0;
    station.activeTouch = &touch;
    station.scrollFlags.bytes[0] = 1;
    station.OnTouchEnd(30, 40, &touch);
    assert(layout.calls == 1 && layout.lastX == 30 && layout.lastY == 40);
    assert(layout.choiceWindowOpen == 1);

    station.activeTouch = &touch;
    layout.result = 1;
    station.OnTouchEnd(50, 60, &touch);
    assert(layout.calls == 2 && layout.choiceWindowOpen == 0);
    assert(station.activeTouch == nullptr);
}
