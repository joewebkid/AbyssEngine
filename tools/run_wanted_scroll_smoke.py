"""Check Wanted scrollbar fractions using the production method bodies."""

import subprocess
import tempfile
from pathlib import Path

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "src/game/ui/WantedWindow.cpp").read_text()
start = function(source, "float WantedWindow::getRelativeScrollStartPos() {")
height = function(source, "float WantedWindow::getRelativeScrollHeight() {")

HOST = r'''
#include <cassert>
#include <cmath>

struct WantedWindow {
    int scrollOffset = 0;
    int contentHeight = 200;
    int visibleHeight = 100;
    float getRelativeScrollStartPos();
    float getRelativeScrollHeight();
};
'''

MAIN = r'''
int main() {
    WantedWindow window;
    window.scrollOffset = 5;
    assert(window.getRelativeScrollStartPos() == 0.0f);
    assert(std::fabs(window.getRelativeScrollHeight() - 0.475f) < 0.0001f);

    window.scrollOffset = -50;
    assert(window.getRelativeScrollStartPos() == 0.25f);
    assert(window.getRelativeScrollHeight() == 0.5f);

    window.scrollOffset = -130;
    assert(window.getRelativeScrollStartPos() == 0.65f);
    assert(std::fabs(window.getRelativeScrollHeight() - 0.35f) < 0.0001f);

    window.scrollOffset = 0;
    window.contentHeight = 80;
    assert(window.getRelativeScrollHeight() == 0.0f);

    window.contentHeight = 100;
    assert(window.getRelativeScrollHeight() == 1.0f);
}
'''


def main() -> None:
    with tempfile.TemporaryDirectory(prefix="gof2-wanted-scroll-") as directory:
        cpp = Path(directory) / "wanted_scroll.cpp"
        cpp.write_text(HOST + start + height + MAIN)
        for optimization in ("-O0", "-O2"):
            executable = Path(directory) / ("wanted_scroll" + optimization + ".exe")
            subprocess.run(["g++", "-std=c++17", optimization, str(cpp), "-o", str(executable)], check=True)
            subprocess.run([str(executable)], check=True)
    print("Wanted scrollbar fractions: OK (-O0/-O2)")


if __name__ == "__main__":
    main()
