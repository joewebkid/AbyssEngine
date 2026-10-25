"""Exercise the recovered WantedWindow::draw body with host UI fixtures."""

import subprocess
import tempfile
from pathlib import Path

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]
BODY = function(
    (ROOT / "src/game/ui/WantedWindow.cpp").read_text(),
    "void WantedWindow::draw() {",
)

HOST = r'''
#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

struct String {
    std::string value;
    String() = default;
    String(const char *s, bool = false) : value(s) {}
    String(const String &s, bool) : value(s.value) {}
    String &operator+=(const String &s) { value += s.value; return *this; }
};
String operator+(const String &a, const String &b) { return String((a.value + b.value).c_str()); }
template<class T> struct Array {
    std::vector<T> values;
    uint32_t size() const { return static_cast<uint32_t>(values.size()); }
    T operator[](uint32_t i) const { return values.at(i); }
};
struct ImagePart {};
struct StarMap { int draws = 0; void draw() { ++draws; } };
struct TouchButton { int draws = 0; void draw() { ++draws; } };
struct ScrollTouchWindow { int draws = 0; void draw() { ++draws; } };
struct Wanted {
    String name;
    bool active;
    int isActive() const { return active; }
    String getName() const { return name; }
};
struct Status {
    int campaign = 0;
    int getCurrentCampaignMission() const { return campaign; }
};
struct GameText {
    String header{"Wanted"}, left{"Targets"}, right{"Details"};
    String detail{"State"}, reward{"Bounty"};
    String *getText(int id) {
        switch (id) {
        case 3219: return &header;
        case 3221: return &left;
        case 3222: return &right;
        case 3226: return &detail;
        case 3225: return &reward;
        default: assert(false); return nullptr;
        }
    }
};
struct PaintCanvas {
    int clips = 0, unclips = 0, images = 0;
    std::vector<uint32_t> colors;
    std::vector<std::string> strings;
    std::string measured;
    void EnableClip(int, int, int, int) { ++clips; }
    void DisableClip() { ++unclips; }
    void SetColor(uint32_t c) { colors.push_back(c); }
    int GetTextHeight(unsigned) { return 12; }
    int GetTextWidth(unsigned, const String &s) { measured = s.value; return int(s.value.size()); }
    void DrawString(unsigned, const String &s, int, int, bool) { strings.push_back(s.value); }
    void DrawImage2D(int, int, int) { ++images; }
};
struct Layout {
    int field_0xc = 12, field_0x20 = 20, field_0x5c = 92;
    int field_0x2c = 44, field_0x48 = 72, field_0x34 = 52;
    int field_0x70 = 112, field_0x44 = 68, field_0x4 = 4;
    int field_0x2d4 = 724, field_0x10 = 16, field_0x24 = 36;
    int buttonInsetX = 40, bars = 0, footer = 0;
    String header;
    std::vector<std::string> boxes;
    void drawScrollBar(int, int, int, int, int) { ++bars; }
    void drawBox(int, int, int, int, int, String s) { boxes.push_back(s.value); }
    void drawHeader(String s) { header = s; }
    void drawFooter() { ++footer; }
};
struct ImageFactory {
    int draws = 0;
    void drawChar(Array<ImagePart *> *, int, int, bool) { ++draws; }
};
struct Globals {
    static Layout *layout;
    static PaintCanvas *Canvas;
    static unsigned font;
    static Status *status;
    static GameText *gameText;
    static void *imageFactory;
};
Layout *Globals::layout = nullptr;
PaintCanvas *Globals::Canvas = nullptr;
unsigned Globals::font = 1;
Status *Globals::status = nullptr;
GameText *Globals::gameText = nullptr;
void *Globals::imageFactory = nullptr;

struct WantedWindow {
    uint8_t showingMap = 0;
    StarMap *starMap = nullptr;
    Array<ImagePart *> *imageParts = nullptr;
    Array<TouchButton *> *buttons = nullptr;
    TouchButton *detailButton = nullptr;
    ScrollTouchWindow *scrollWindow = nullptr;
    Array<Wanted *> *wantedList = nullptr;
    uint32_t selectedWanted = 0, highlightedWanted = UINT32_MAX;
    int windowX = 10, windowY = 20, windowWidth = 1200, windowHeight = 900;
    int visibleHeight = 100, scrollOffset = 0, bgImage = 7;
    String nameText{"Pilot"}, fromText{"Old from"}, toText{"Old to"};
    String detailText{"Active"}, rewardText{"750$"};
    float relativeStart = 0.0f, relativeHeight = 0.0f;
    float getRelativeScrollStartPos() const { return relativeStart; }
    float getRelativeScrollHeight() const { return relativeHeight; }
    void draw();
};
'''

MAIN = r'''
int main() {
    WantedWindow window;
    StarMap map;
    window.showingMap = 1;
    window.starMap = &map;
    window.draw();
    assert(map.draws == 1);

    Layout layout;
    PaintCanvas canvas;
    Status status;
    GameText text;
    ImageFactory factory;
    Globals::layout = &layout;
    Globals::Canvas = &canvas;
    Globals::status = &status;
    Globals::gameText = &text;
    Globals::imageFactory = &factory;
    Wanted first{String("First"), true}, second{String("Second"), false};
    Array<Wanted *> wanted{{&first, &second}};
    TouchButton tab, detail;
    Array<TouchButton *> buttons{{&tab}};
    ScrollTouchWindow scroll;
    Array<ImagePart *> parts;
    window.showingMap = 0;
    window.wantedList = &wanted;
    window.buttons = &buttons;
    window.detailButton = &detail;
    window.scrollWindow = &scroll;
    window.imageParts = &parts;
    status.campaign = 128;
    window.draw();
    assert(canvas.clips == 1 && canvas.unclips == 1);
    assert(layout.bars == 0 && layout.footer == 1);
    assert(layout.header.value == "Wanted");
    assert(layout.boxes.size() == 6);
    assert(layout.boxes[2] == "Targets" && layout.boxes[4] == "Details");
    assert(canvas.colors[0] == 0xffffffffu && canvas.colors[1] == 0x777777ffu);
    assert(canvas.images == 1 && canvas.measured == "First ");
    assert(canvas.strings.size() == 5);
    assert(canvas.strings[0] == "First" && canvas.strings[1] == "Second");
    assert(canvas.strings[2] == "Pilot");
    assert(canvas.strings[3] == "State Active");
    assert(canvas.strings[4] == "Bounty 750$");
    assert(factory.draws == 1 && scroll.draws == 1);
    assert(tab.draws == 1 && detail.draws == 1);

    window.relativeStart = 0.01f;
    window.selectedWanted = 1;
    status.campaign = 130;
    window.draw();
    assert(layout.bars == 1);
    assert(canvas.images == 2 && canvas.measured == "Second ");
    assert(detail.draws == 1);

    window.relativeStart = 0.0f;
    window.relativeHeight = 0.01f;
    window.draw();
    assert(layout.bars == 2);
}
'''


def main() -> None:
    with tempfile.TemporaryDirectory(prefix="gof2-wanted-draw-") as directory:
        cpp = Path(directory) / "wanted_draw.cpp"
        cpp.write_text(HOST + BODY + MAIN)
        for optimization in ("-O0", "-O2"):
            executable = Path(directory) / ("wanted_draw" + optimization + ".exe")
            subprocess.run(["g++", "-std=c++17", optimization, str(cpp), "-o", str(executable)], check=True)
            subprocess.run([str(executable)], check=True)
    print("WantedWindow draw branches: OK (-O0/-O2)")


if __name__ == "__main__":
    main()
