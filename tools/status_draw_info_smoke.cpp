#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

struct String {
    std::string text;
    String() = default;
    String(const char *value) : text(value) {}
    void ctor_char(const char *value, bool) { text = value; }
    void Set(long long value) { text = std::to_string(value); }
    String SubString(unsigned start, unsigned end) const {
        return String(text.substr(start, end - start).c_str());
    }
    String &operator+=(int value) { text += std::to_string(value); return *this; }
};
String operator+(const String &a, const String &b) {
    String out;
    out.text = a.text + b.text;
    return out;
}
String operator+(int value, const String &b) {
    String out;
    out.text = std::to_string(value) + b.text;
    return out;
}

struct DrawCall { std::string text; int x, y; };
struct PaintCanvas {
    std::vector<DrawCall> calls;
    int GetTextWidth(unsigned, const String &value) {
        return static_cast<int>(value.text.size());
    }
    void DrawString(unsigned, const String &value, int x, int y, bool) {
        calls.push_back({value.text, x, y});
    }
};
struct Layout {
    int field_0x4 = 12, field_0x4c = 20, field_0x2d8 = 40, field_0x2cc = 4;
    String formatCredits(int) { return String("1,000"); }
    void drawBox(int, int, int, int, int, const String &, int) {}
};
struct Ship {
    int getIndex() { return 4; }
    int getFirePower() {
        float value = 12.5f;
        int bits;
        std::memcpy(&bits, &value, sizeof(bits));
        return bits;
    }
    int getCombinedHP() { return 250; }
};
struct Status {
    Ship ship;
    int getCredits() { return 1000; }
    int getLevel() { return 7; }
    long long getPlayingTime() { return 123000; }
    Ship *getShip() { return &ship; }
};
struct GameText {
    String text;
    int lastId = -1;
    String *getText(int id) {
        lastId = id;
        text.text = id == 321 ? "LEVEL" : id == 917 ? "SHIP" :
                    id == 569 ? "FIRE" : id == 570 ? "HP" : "UNKNOWN";
        return &text;
    }
};
struct ImageFactory {
    int shipIndex = -1, shipX = -1, shipY = -1;
    void drawShip(int index, int x, int y) {
        shipIndex = index; shipX = x; shipY = y;
    }
};
struct Globals {
    static Layout *layout;
    static Status *status;
    static GameText *gameText;
    static void *imageFactory;
    static void *globals;
    void longToTimeStringNoSeconds(long long, String &out) { out.text = "02:03"; }
};
Layout *Globals::layout = nullptr;
Status *Globals::status = nullptr;
GameText *Globals::gameText = nullptr;
void *Globals::imageFactory = nullptr;
void *Globals::globals = nullptr;

struct StatusWindow {
    void drawInfoFixture(PaintCanvas *canvas, Layout *layout, unsigned font,
                         int boxW, int x0, int pad, int y, String &creditStr,
                         String &lbl, const String &sep) {
// PRODUCTION_INFO_CARD
    }
};

int main() {
    Layout layout;
    Status status;
    GameText gameText;
    ImageFactory images;
    Globals globals;
    PaintCanvas canvas;
    StatusWindow window;
    Globals::layout = &layout;
    Globals::status = &status;
    Globals::gameText = &gameText;
    Globals::imageFactory = &images;
    Globals::globals = &globals;
    String creditStr, lbl, sep(":");
    window.drawInfoFixture(&canvas, &layout, 1, 600, 10, 8, 100,
                           creditStr, lbl, sep);
    assert(images.shipIndex == 4 && images.shipX == 326 && images.shipY == 100);
    assert(canvas.calls.size() == 8);
    const DrawCall expected[] = {
        {"1,000", 267, 108}, {"LEVEL 7", 265, 120}, {"02:03", 267, 132},
        {"SHIP", 338, 108}, {"FIRE:", 338, 120}, {"12.50", 585, 120},
        {"HP:", 338, 132}, {"250", 587, 132}
    };
    for (unsigned i = 0; i < 8; ++i) {
        assert(canvas.calls[i].text == expected[i].text);
        assert(canvas.calls[i].x == expected[i].x);
        assert(canvas.calls[i].y == expected[i].y);
    }
    std::puts("StatusWindow info-card host smoke: OK");
}
