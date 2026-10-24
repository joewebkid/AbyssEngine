"""Exercise the production WantedWindow touch bodies with bounded host objects."""

import subprocess
import tempfile
from pathlib import Path

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "src/game/ui/WantedWindow.cpp").read_text()
BODIES = "".join(
    function(SOURCE, signature)
    for signature in (
        "int WantedWindow::OnTouchBegin(int x, int y) {",
        "int WantedWindow::OnTouchMove(int x, int y) {",
        "uint32_t WantedWindow::getWantedAtPosition(int x, int y) {",
    )
)

HOST = r'''
#include <cassert>
#include <cstdint>
#include <vector>

struct TouchButton {
    int begins = 0, moves = 0;
    void OnTouchBegin(int, int) { ++begins; }
    void OnTouchMove(int, int) { ++moves; }
};
struct ScrollTouchWindow : TouchButton {};
struct StarMap : TouchButton {};
struct Wanted {
    bool active = false;
    bool isActive() { return active; }
};
template<class T> struct Array {
    std::vector<T> values;
    uint32_t size() const { return static_cast<uint32_t>(values.size()); }
    T operator[](uint32_t i) { return values.at(i); }
};
struct Layout : TouchButton {
    int field_0xc = 10, field_0x10 = 10, field_0x20 = 0;
    int field_0x5c = 0, field_0x70 = 20, field_0x34 = 0;
};
struct Globals {
    static Layout *layout;
    static int w, h, mouse_wheel;
};
Layout *Globals::layout = nullptr;
int Globals::w = 400, Globals::h = 300, Globals::mouse_wheel = 0;
struct WantedWindow {
    uint8_t showingMap = 0, dragging = 0;
    StarMap *starMap = nullptr;
    ScrollTouchWindow *scrollWindow = nullptr;
    Array<TouchButton *> *buttons = nullptr;
    TouchButton *detailButton = nullptr;
    Array<Wanted *> *wantedList = nullptr;
    uint32_t selectedWanted = 0, highlightedWanted = UINT32_MAX;
    int windowX = 0, windowY = 0, windowWidth = 400;
    int lastDragY = 0, touchStartY = 0, dragDelta = 0, scrollOffset = 0;
    float scrollDamping = 0.0f;
    int OnTouchBegin(int x, int y);
    int OnTouchMove(int x, int y);
    uint32_t getWantedAtPosition(int x, int y);
};
'''

MAIN = r'''
int main() {
    Layout layout;
    Globals::layout = &layout;
    StarMap map;
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
    window.wantedList = &wantedList;
    window.detailButton = &detail;

    window.showingMap = 1;
    assert(window.OnTouchBegin(50, 20) == 0);
    assert(window.OnTouchMove(50, 21) == 0);
    assert(map.begins == 1 && map.moves == 1 && layout.begins == 0);
    window.showingMap = 0;

    assert(window.OnTouchBegin(50, 20) == 0);
    assert(window.dragging == 1 && window.lastDragY == 20);
    assert(window.highlightedWanted == 0);
    assert(scroll.begins == 1 && button.begins == 1 && layout.begins == 1);
    assert(detail.begins == 0);
    wanted.active = true;
    assert(window.OnTouchBegin(50, 20) == 0 && detail.begins == 1);

    assert(window.OnTouchMove(50, 21) == 0);
    assert(window.scrollOffset == 1 && window.dragDelta == 1);
    assert(window.highlightedWanted == 0 && layout.moves == 1 && detail.moves == 1);
    assert(window.OnTouchMove(50, 35) == 0);
    assert(window.scrollOffset == 15 && window.highlightedWanted == UINT32_MAX);

    wanted.active = false;
    assert(window.OnTouchMove(250, 35) == 0);
    assert(scroll.moves == 1 && detail.moves == 2);
    assert(window.scrollOffset == 15 && window.highlightedWanted == UINT32_MAX);
    assert(window.OnTouchMove(50, 9) == 0);
    assert(window.scrollOffset == 15);

    Globals::mouse_wheel = 1;
    assert(window.OnTouchMove(250, 40) == 0);
    assert(window.dragDelta == 5 && window.scrollOffset == 20);
    assert(window.scrollDamping == 1.0f);
}
'''


def main() -> None:
    with tempfile.TemporaryDirectory(prefix="wanted-touch-") as temp:
        source = Path(temp) / "touch.cpp"
        source.write_text(HOST + BODIES + MAIN)
        for optimization in ("-O0", "-O2"):
            binary = Path(temp) / f"touch-{optimization[2:]}.exe"
            subprocess.run(["g++", "-std=c++17", optimization, str(source), "-o", str(binary)], check=True)
            subprocess.run([str(binary)], check=True)
    print("WantedWindow touch routing: OK (-O0/-O2)")


if __name__ == "__main__":
    main()
