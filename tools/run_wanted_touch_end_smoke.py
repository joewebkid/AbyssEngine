"""Exercise the production WantedWindow touch-end body with host fixtures."""

import subprocess
import tempfile
from pathlib import Path

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "src/game/ui/WantedWindow.cpp").read_text()
BODY = function(SOURCE, "int WantedWindow::OnTouchEnd(int x, int y) {")

HOST = r'''
#include <cassert>
#include <cstdint>
#include <vector>

struct String {
    String() = default;
    String(const String &, bool) {}
};
struct GameText {
    int key = -1;
    String value;
    String *getText(int k) { key = k; return &value; }
};
struct Layout {
    int ends = 0, resets = 0, helps = 0;
    int endResult = 0, helpResult = 0;
    int OnTouchEnd(int, int) { ++ends; return endResult; }
    void resetWindowDimensions() { ++resets; }
    int helpPressed() { return helpResult; }
    void initHelpWindow(String) { ++helps; }
};
struct TouchButton {
    int ends = 0, result = 0;
    int OnTouchEnd(int, int) { ++ends; return result; }
};
struct ScrollTouchWindow : TouchButton {};
struct Mission { Mission(int, int, int) {} };
struct StarMap {
    int endResult = 0, ends = 0, inits = 0, starts = 0;
    StarMap() = default;
    StarMap(bool, Mission *, bool, int) {}
    int OnTouchEnd(int, int) { ++ends; return endResult; }
    void init(bool, Mission *, bool, int) { ++inits; }
    void setStart(int, int) { ++starts; }
};
struct Wanted {
    bool active = false;
    int isActive() { return active; }
    int getLastSeen() { return 7; }
    int getTravelsTo() { return 8; }
};
struct Station { int getSystem() { return 9; } };
struct Galaxy {
    int requested = -1;
    Station *getStation(int id) { requested = id; return new Station; }
};
struct ModStation { StarMap *starMap = nullptr; };
struct ApplicationManager {
    ModStation *module;
    void *GetApplicationModule(int id) { assert(id == 5); return module; }
};
template<class T> struct Array {
    std::vector<T> values;
    uint32_t size() const { return static_cast<uint32_t>(values.size()); }
    T operator[](uint32_t i) { return values.at(i); }
};
struct Globals {
    static unsigned char iPad, iPadHD, iPadLarge;
    static int w, h;
    static Layout *layout;
    static GameText *gameText;
    static void *appManager, *galaxy;
};
unsigned char Globals::iPad = 0, Globals::iPadHD = 0, Globals::iPadLarge = 0;
int Globals::w = 1400, Globals::h = 1100;
Layout *Globals::layout = nullptr;
GameText *Globals::gameText = nullptr;
void *Globals::appManager = nullptr, *Globals::galaxy = nullptr;
struct WantedWindow {
    uint8_t showingMap = 0, dragging = 1;
    StarMap *starMap = nullptr;
    ScrollTouchWindow *scrollWindow = nullptr;
    Array<TouchButton *> *buttons = nullptr;
    TouchButton *detailButton = nullptr;
    Array<Wanted *> *wantedList = nullptr;
    Mission *mission = nullptr;
    uint32_t selectedWanted = 0, highlightedWanted = UINT32_MAX, lastButtonHit = UINT32_MAX;
    uint32_t forcedHit = 0;
    int selectedCalls = 0;
    int windowX = 0, windowY = 0, windowWidth = 0, windowHeight = 0;
    int dragDelta = 0, scrollOffset = 0, scrollOffsetSnapshot = 0;
    float scrollDamping = 0, scrollVelocity = 0;
    uint32_t getWantedAtPosition(int, int) { return forcedHit; }
    void selectWanted(int) { ++selectedCalls; }
    int OnTouchEnd(int, int);
};
'''

MAIN = r'''
int main() {
    Layout layout;
    GameText text;
    Globals::layout = &layout;
    Globals::gameText = &text;
    StarMap map;
    ModStation module{&map};
    ApplicationManager app{&module};
    Galaxy galaxy;
    Globals::appManager = &app;
    Globals::galaxy = &galaxy;
    ScrollTouchWindow scroll;
    TouchButton button, detail;
    Array<TouchButton *> buttons;
    buttons.values.push_back(&button);
    Wanted wanted;
    Array<Wanted *> wantedList;
    wantedList.values.push_back(&wanted);
    WantedWindow window;
    window.starMap = &map;
    window.scrollWindow = &scroll;
    window.buttons = &buttons;
    window.detailButton = &detail;
    window.wantedList = &wantedList;

    window.showingMap = 1;
    assert(window.OnTouchEnd(1, 2) == 0 && window.showingMap == 1);
    map.endResult = 1;
    assert(window.OnTouchEnd(1, 2) == 0);
    assert(window.windowX == 0 && window.windowY == 0);
    assert(window.windowWidth == 1400 && window.windowHeight == 1100);
    assert(window.showingMap == 0);

    Globals::iPad = 1;
    map.endResult = 1;
    window.showingMap = 1;
    assert(window.OnTouchEnd(1, 2) == 0);
    assert(window.windowWidth == 650 && window.windowHeight == 500);
    assert(window.windowX == 375 && window.windowY == 300);
    Globals::iPadLarge = 1;
    window.showingMap = 1;
    assert(window.OnTouchEnd(1, 2) == 0);
    assert(window.windowWidth == 1300 && window.windowHeight == 1000);
    Globals::iPadHD = 1;
    window.showingMap = 1;
    assert(window.OnTouchEnd(1, 2) == 0);
    assert(window.windowWidth == 914 && window.windowHeight == 703);

    window.dragDelta = 4;
    window.scrollOffset = 10;
    layout.endResult = 1;
    assert(window.OnTouchEnd(3, 4) == 1);
    assert(window.scrollOffset == 14 && window.scrollOffsetSnapshot == 14);
    assert(window.scrollVelocity == 4 && window.dragging == 0);
    assert(scroll.ends == 1 && button.ends == 1 && layout.resets == 1);

    layout.endResult = 0;
    layout.helpResult = 1;
    button.result = 1;
    window.highlightedWanted = 0;
    window.forcedHit = 0;
    assert(window.OnTouchEnd(3, 4) == 0);
    assert(window.lastButtonHit == 0 && window.selectedCalls == 1);
    assert(layout.helps == 1 && text.key == 0x27b);

    wanted.active = true;
    detail.result = 1;
    assert(window.OnTouchEnd(3, 4) == 0);
    assert(window.showingMap == 1 && map.inits == 1 && map.starts == 1);
    assert(galaxy.requested == 7 && layout.resets == 2);
    window.showingMap = 0;
    module.starMap = nullptr;
    assert(window.OnTouchEnd(3, 4) == 0);
    assert(window.showingMap == 1 && module.starMap == window.starMap);
    assert(window.starMap != &map && window.starMap->starts == 1);
    delete window.starMap;
    delete window.mission;
}
'''


def main() -> None:
    with tempfile.TemporaryDirectory(prefix="wanted-end-") as temp:
        source = Path(temp) / "end.cpp"
        source.write_text(HOST + BODY + MAIN)
        for optimization in ("-O0", "-O2"):
            binary = Path(temp) / f"end-{optimization[2:]}.exe"
            subprocess.run(["g++", "-std=c++17", optimization, str(source), "-o", str(binary)], check=True)
            subprocess.run([str(binary)], check=True)
    print("WantedWindow touch-end branches: OK (-O0/-O2)")


if __name__ == "__main__":
    main()
