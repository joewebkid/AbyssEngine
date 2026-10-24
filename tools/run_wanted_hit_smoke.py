"""Run the production WantedWindow hit-test with bounded host fixtures."""

import subprocess
import tempfile
from pathlib import Path

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "src/game/ui/WantedWindow.cpp").read_text()
BODY = function(SOURCE, "uint32_t WantedWindow::getWantedAtPosition(int x, int y) {")

HOST = r'''
#include <cassert>
#include <cstdint>
#include <vector>

struct Wanted {};
template<class T> struct Array {
    std::vector<T> values;
    uint32_t size() const { return static_cast<uint32_t>(values.size()); }
};
struct Layout {
    int field_0xc = 10;
    int field_0x20 = 5;
    int field_0x5c = 3;
    int field_0x70 = 20;
    int field_0x34 = 10;
};
struct Globals { static Layout *layout; };
Layout *Globals::layout = nullptr;
struct WantedWindow {
    int windowX = 20;
    int windowY = 100;
    int windowWidth = 200;
    int scrollOffset = 0;
    Array<Wanted *> *wantedList = nullptr;
    uint32_t getWantedAtPosition(int x, int y);
};
'''

MAIN = r'''
int main() {
    Layout layout;
    Globals::layout = &layout;
    Array<Wanted *> list;
    list.values.resize(3);
    WantedWindow window;
    window.wantedList = &list;
    assert(window.getWantedAtPosition(119, 118) == 0);
    assert(window.getWantedAtPosition(119, 147) == 0);
    assert(window.getWantedAtPosition(119, 148) == 1);
    assert(window.getWantedAtPosition(119, 178) == 2);
    assert(window.getWantedAtPosition(119, 208) == UINT32_MAX);
    assert(window.getWantedAtPosition(120, 118) == UINT32_MAX);
    window.scrollOffset = 30;
    assert(window.getWantedAtPosition(119, 148) == 0);
    window.scrollOffset = 0;
    list.values.clear();
    // The ARM unsigned bound wraps for an empty list; callers must avoid indexing it.
    assert(window.getWantedAtPosition(119, 118) == 0);
}
'''


def main() -> None:
    with tempfile.TemporaryDirectory(prefix="wanted-hit-") as temp:
        source = Path(temp) / "hit.cpp"
        source.write_text(HOST + BODY + MAIN)
        for optimization in ("-O0", "-O2"):
            binary = Path(temp) / f"hit-{optimization[2:]}.exe"
            subprocess.run(["g++", "-std=c++17", optimization, str(source), "-o", str(binary)], check=True)
            subprocess.run([str(binary)], check=True)
    print("WantedWindow hit-test boundaries: OK (-O0/-O2)")


if __name__ == "__main__":
    main()
