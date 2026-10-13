#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

struct String {
    std::string text;
    const char *data = nullptr;
    void Set(const char *value) { text = value; }
    void ctor_char(const char *value, bool) { text = value; }
};
template <typename T> using Array = std::vector<T>;
struct BoxCall { int style, x, y, w, h; std::string text; };
struct Layout {
    int buttonInsetX = 10, field_0xc = 10, field_0x2c = 8;
    int field_0x1c = 16, field_0x4 = 12, field_0x4c = 20;
    int field_0x10 = 4, field_0x24 = 6;
    std::vector<BoxCall> boxes;
    void drawBox(int style, int x, int y, int w, int h, const String &text, int) {
        boxes.push_back({style, x, y, w, h, text.text});
    }
};
struct PaintCanvas {
    unsigned color = 0;
    void SetColor(unsigned value) { color = value; }
};
struct TouchButton {
    int x = -1, y = -1, draws = 0, flags0 = 0x12;
    void setPosition(int nextX, int nextY) { x = nextX; y = nextY; }
    void draw() { ++draws; }
};
struct GameText {
    String text;
    String *getText(int id) {
        text.text = "T" + std::to_string(id);
        text.data = text.text.c_str();
        return &text;
    }
};
struct Globals {
    static GameText *gameText;
    static void *globals;
    int lineFont = -1, lineX = -1, lineY = -1, lineCount = -1;
    bool centered = true;
    void drawLines(unsigned font, Array<String *> *lines, int x, int y, bool center) {
        lineFont = static_cast<int>(font);
        lineX = x; lineY = y;
        lineCount = static_cast<int>(lines->size());
        centered = center;
    }
};
GameText *Globals::gameText = nullptr;
void *Globals::globals = nullptr;

struct StatusWindow {
    int boxWidth = 600, medalRowHeight = 30, medalCount = 5;
    int scrollOffset = 5, selectedMedal = -1;
    Array<TouchButton *> *medalButtons;
    Array<String *> *detailLines;
    void drawMedalsFixture(Layout *layout, PaintCanvas *canvas, unsigned font,
                           int top, int screenH, int drewStats, int tab) {
// PRODUCTION_MEDAL_GRID
    }
};

int main() {
    Layout layout;
    PaintCanvas canvas;
    GameText text;
    Globals globals;
    Globals::gameText = &text;
    Globals::globals = &globals;
    TouchButton buttons[5];
    Array<TouchButton *> buttonList;
    for (TouchButton &button : buttons) buttonList.push_back(&button);
    String line1, line2;
    Array<String *> lines{&line1, &line2};
    StatusWindow window;
    window.medalButtons = &buttonList;
    window.detailLines = &lines;

    window.drawMedalsFixture(&layout, &canvas, 7, 40, 400, 1, 0);
    assert(layout.boxes.size() == 1);
    const BoxCall &header = layout.boxes[0];
    assert(header.style == 0 && header.x == 620 && header.y == 45);
    assert(header.w == 610 && header.h == 16 && header.text == "T168");
    const int tabletX[] = {710, 910, 1110, 710, 910};
    const int tabletY[] = {62, 62, 62, 92, 92};
    for (int i = 0; i < 5; ++i) {
        assert(buttons[i].x == tabletX[i] && buttons[i].y == tabletY[i]);
        assert(buttons[i].draws == 1 && buttons[i].flags0 == 0x12);
    }

    layout.boxes.clear();
    window.scrollOffset = -40;
    window.selectedMedal = 2;
    window.drawMedalsFixture(&layout, &canvas, 7, 40, 400, 0, 1);
    assert(layout.boxes.size() == 2 && canvas.color == 0xffffffffu);
    for (const BoxCall &box : layout.boxes) {
        assert((box.style == 2 || box.style == 5) && box.x == 10);
        assert(box.y == 326 && box.w == 600 && box.h == 64);
    }
    const int phoneX[] = {110, 310, 510, 110, 310};
    const int phoneY[] = {-7, -7, -7, 23, 23};
    for (int i = 0; i < 5; ++i) {
        assert(buttons[i].x == phoneX[i] && buttons[i].y == phoneY[i]);
        assert(buttons[i].draws == (i < 3 ? 1 : 2));
        assert(buttons[i].flags0 == 0x12);
    }
    assert(globals.lineFont == 7 && globals.lineX == 30 && globals.lineY == 358);
    assert(globals.lineCount == 2 && !globals.centered);
    std::puts("StatusWindow medal-grid host smoke: OK");
}
