"""Exercise the production WantedWindow::selectWanted body with host fixtures."""

import subprocess
import tempfile
from pathlib import Path

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]
BODY = function(
    (ROOT / "src/game/ui/WantedWindow.cpp").read_text(),
    "void WantedWindow::selectWanted(int idx) {",
)

HOST = r'''
#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

struct String {
    std::string value;
    String() = default;
    String(const char *text, bool = false) : value(text) {}
    explicit String(int number) : value(std::to_string(number)) {}
    String &operator+=(const String &other) { value += other.value; return *this; }
};
String operator+(const String &a, const String &b) { return String((a.value + b.value).c_str()); }
template<class T> struct Array {
    std::vector<T> values;
    T &operator[](size_t index) { return values.at(index); }
};
struct ImagePart {};
struct ImageFactory {
    Array<ImagePart *> *loadChar(int *) { return new Array<ImagePart *>; }
};
struct SolarSystem {
    String name;
    String getName() const { return name; }
};
struct Station {
    String name;
    int system;
    String getName() const { return name; }
    int getSystem() const { return system; }
};
struct Galaxy {
    SolarSystem systems[3]{{"Sol"}, {"Ymir"}, {"Thynome"}};
    Station *getStation(int id) {
        if (id == 7) return new Station{{"Earth"}, 0};
        if (id == 8) return new Station{{"Midgard"}, 1};
        assert(id == 9);
        return new Station{{"Hades"}, 2};
    }
    SolarSystem *getSystem(int index) { return &systems[index]; }
};
struct Wanted {
    bool active, terminated;
    int reward = 750, index = 4;
    int parts = 0;
    int *getImageParts() { return &parts; }
    String getName() const { return String("Target"); }
    int isActive() const { return active; }
    int isTerminated() const { return terminated; }
    int getLastSeen() const { return 7; }
    int getTravelsTo() const { return 8; }
    int getCurrentLocation() const { return 9; }
    int getReward() const { return reward; }
    int getIndex() const { return index; }
};
struct GameText {
    String strings[6]{{"Unknown"}, {"Terminated"}, {"Active"},
                      {"From"}, {"To"}, {"Wanted description"}};
    String *getText(int key) {
        if (key == 3229) return &strings[0];
        if (key == 3227) return &strings[1];
        if (key == 3228) return &strings[2];
        if (key == 3223) return &strings[3];
        if (key == 3224) return &strings[4];
        assert(key == 3178);
        return &strings[5];
    }
};
struct Layout {
    int field_0xc = 12, field_0x20 = 20, field_0x5c = 92;
    int field_0x2c = 44, field_0x10 = 16, field_0x2d8 = 728;
    int field_0x24 = 36, field_0x4c = 76, field_0x30 = 48;
    int buttonInsetX = 40;
};
struct ScrollTouchWindow {
    int x, y, width, height;
    String title, body;
    ScrollTouchWindow(int px, int py, int pw, int ph, bool)
        : x(px), y(py), width(pw), height(ph) {}
    void setText(String t, String b) { title = t; body = b; }
};
struct Globals {
    static void *imageFactory, *galaxy;
    static GameText *gameText;
    static Layout *layout;
};
void *Globals::imageFactory = nullptr, *Globals::galaxy = nullptr;
GameText *Globals::gameText = nullptr;
Layout *Globals::layout = nullptr;
struct WantedWindow {
    Array<ImagePart *> *imageParts = nullptr;
    ScrollTouchWindow *scrollWindow = nullptr;
    Array<Wanted *> *wantedList = nullptr;
    String nameText, fromText, toText, atText, rewardText, detailText;
    int windowX = 0, windowY = 0, windowWidth = 1300, windowHeight = 1000;
    ~WantedWindow() { delete imageParts; delete scrollWindow; }
    void selectWanted(int);
};
'''

MAIN = r'''
int main() {
    ImageFactory factory;
    Galaxy galaxy;
    GameText text;
    Layout layout;
    Globals::imageFactory = &factory;
    Globals::galaxy = &galaxy;
    Globals::gameText = &text;
    Globals::layout = &layout;
    Wanted active{true, false}, terminated{false, true}, pending{false, false};
    Array<Wanted *> wantedList{{&active, &terminated, &pending}};
    WantedWindow window;
    window.wantedList = &wantedList;

    window.selectWanted(0);
    assert(window.nameText.value == "Target");
    assert(window.fromText.value == "Earth (Sol)");
    assert(window.toText.value == "Midgard (Ymir)");
    assert(window.atText.value == "Hades (Thynome)");
    assert(window.rewardText.value == "750$");
    assert(window.detailText.value == "Active");
    assert(window.scrollWindow->body.value ==
           "From\nEarth (Sol)\n\nTo\nMidgard (Ymir)\n\nWanted description");

    window.selectWanted(1);
    assert(window.fromText.value == " -- ");
    assert(window.toText.value == " --");
    assert(window.atText.value == " --");
    assert(window.rewardText.value == " --");
    assert(window.detailText.value == "Terminated");
    assert(window.scrollWindow->body.value ==
           "From\n -- \n\nTo\n --\n\nWanted description");

    window.selectWanted(2);
    assert(window.fromText.value == "Unknown");
    assert(window.toText.value == "Unknown");
    assert(window.atText.value == "Unknown");
    assert(window.rewardText.value == "750$");
    assert(window.detailText.value == "Active");
    assert(window.scrollWindow->body.value ==
           "From\nUnknown\n\nTo\nUnknown\n\nWanted description");
}
'''


def main() -> None:
    with tempfile.TemporaryDirectory(prefix="gof2-wanted-select-") as directory:
        cpp = Path(directory) / "wanted_select.cpp"
        cpp.write_text(HOST + BODY + MAIN)
        for optimization in ("-O0", "-O2"):
            executable = Path(directory) / ("wanted_select" + optimization + ".exe")
            subprocess.run(
                ["g++", "-std=c++17", optimization, str(cpp), "-o", str(executable)],
                check=True,
            )
            subprocess.run([str(executable)], check=True)
    print("WantedWindow selected text branches: OK (-O0/-O2)")


if __name__ == "__main__":
    main()
