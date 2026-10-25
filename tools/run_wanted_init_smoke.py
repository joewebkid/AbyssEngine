"""Exercise the production WantedWindow::init body with bounded host fixtures."""

import subprocess
import tempfile
from pathlib import Path

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "src/game/ui/WantedWindow.cpp").read_text()
BODY = function(SOURCE, "int WantedWindow::init() {")

HOST = r'''
#include <cassert>
#include <cstdint>
#include <vector>

struct String {};
struct Wanted {
    int board;
    bool active;
    int getBoard() const { return board; }
    int isActive() const { return active; }
};
template<class T> struct Array {
    std::vector<T> values;
    uint32_t size() const { return static_cast<uint32_t>(values.size()); }
    T &operator[](uint32_t i) { return values.at(i); }
};
template<class T> void ArrayAdd(T value, Array<T> &array) { array.values.push_back(value); }
template<class T> void ArraySetLength(size_t length, Array<T> &array) { array.values.resize(length); }
struct SolarSystem { int race = 0; int getRace() const { return race; } };
struct Status {
    SolarSystem system;
    Array<Wanted *> wanted;
    int mission = 0;
    Array<Wanted *> *getWanted() { return &wanted; }
    SolarSystem *getSystem() { return &system; }
    int getCurrentCampaignMission() const { return mission; }
};
struct GameText { String label; String *getText(int) { return &label; } };
struct TouchButton {
    TouchButton(int, int, String, int, int, int) {}
    TouchButton(String, int, int, int, int, int, int) {}
    int getWidth() const { return 20; }
    void setAlwaysPressed(bool) {}
};
struct Layout {
    int field_0x10 = 10, field_0xc = 12, field_0x20 = 20;
    int field_0x24 = 24, field_0x5c = 30, field_0x2c = 8;
    int field_0x34 = 32, field_0x70 = 4, field_0x38 = 6;
    int field_0xcc = 120, buttonInsetX = 9, field_0x4c = 11;
    int dimensions = 0;
    int getHelpButtonOffset() const { return 15; }
    void setWindowDimensions(int, int, int, int) { ++dimensions; }
};
struct Globals {
    static Status *status;
    static Layout *layout;
    static GameText *gameText;
    static unsigned char iPad, iPadHD, iPadLarge;
    static int w, h;
};
Status *Globals::status = nullptr;
Layout *Globals::layout = nullptr;
GameText *Globals::gameText = nullptr;
unsigned char Globals::iPad = 0, Globals::iPadHD = 0, Globals::iPadLarge = 0;
int Globals::w = 1400, Globals::h = 1100;
struct WantedWindow {
    int scrollOffset = 0, lastDragY = 0, scrollOffsetSnapshot = 0, dragDelta = 0;
    Array<Wanted *> *wantedList = nullptr;
    int windowX = 0, windowY = 0, windowWidth = 0, windowHeight = 0;
    uint32_t selectedWanted = 0, highlightedWanted = 0;
    Array<TouchButton *> *buttons = nullptr;
    int contentHeight = 0, visibleHeight = 0;
    TouchButton *detailButton = nullptr;
    void *starMap = nullptr;
    int showingMap = 0, hangarUpdate = 0, lastButtonHit = 0;
    int selectionCalls = 0;
    void selectWanted(int) { ++selectionCalls; }
    int init();
    ~WantedWindow() {
        if (buttons) { for (auto *button : buttons->values) delete button; }
        delete buttons;
        delete detailButton;
        delete wantedList;
    }
};
'''

MAIN = r'''
int main() {
    Layout layout;
    GameText text;
    Status status;
    Globals::layout = &layout;
    Globals::gameText = &text;
    Globals::status = &status;
    Wanted board0{0, true}, board1{1, true}, board2{2, false};
    status.wanted.values = {&board0, &board1, &board2};

    auto check = [&](int race, int mission, int expectedCount, Wanted *expected) {
        status.system.race = race;
        status.mission = mission;
        WantedWindow window;
        assert(window.init() == 1);
        assert(static_cast<int>(window.wantedList->size()) == expectedCount);
        if (expectedCount) assert((*window.wantedList)[0] == expected);
        assert(window.selectionCalls == 1);
        assert(layout.dimensions > 0);
        return 0;
    };

    check(0, 127, 0, nullptr);
    check(0, 128, 1, &board0);
    check(1, 161, 0, nullptr);
    check(1, 162, 1, &board1);
    check(2, 162, 1, &board2);

    status.system.race = 0;
    status.mission = 128;
    {
        WantedWindow window;
        Globals::iPad = 0;
        window.init();
        assert(window.windowX == 0 && window.windowY == 0);
        assert(window.windowWidth == 1400 && window.windowHeight == 1100);
    }
    {
        WantedWindow window;
        Globals::iPad = 1; Globals::iPadHD = 1;
        window.init();
        assert(window.windowWidth == 914 && window.windowHeight == 703);
        assert(window.windowX == 243 && window.windowY == 199);
    }
    {
        WantedWindow window;
        Globals::iPadHD = 0; Globals::iPadLarge = 0;
        window.init();
        assert(window.windowWidth == 650 && window.windowHeight == 500);
        assert(window.windowX == 375 && window.windowY == 300);
    }
    {
        WantedWindow window;
        Globals::iPadLarge = 1;
        window.init();
        assert(window.windowWidth == 1300 && window.windowHeight == 1000);
        assert(window.windowX == 50 && window.windowY == 50);
    }
}
'''


def main() -> None:
    with tempfile.TemporaryDirectory(prefix="gof2-wanted-init-") as directory:
        cpp = Path(directory) / "wanted_init.cpp"
        cpp.write_text(HOST + BODY + MAIN)
        for optimization in ("-O0", "-O2"):
            executable = Path(directory) / ("wanted_init" + optimization + ".exe")
            subprocess.run(
                ["g++", "-std=c++17", optimization, str(cpp), "-o", str(executable)],
                check=True,
            )
            subprocess.run([str(executable)], check=True)
    print("WantedWindow init filters and dimensions: OK (-O0/-O2)")


if __name__ == "__main__":
    main()
