#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

struct String {
    std::string text;
    String() = default;
    String(const char *value) : text(value) {}
    void ctor_char(const char *value, bool) { text = value; }
    void Set(long long value) { text = std::to_string(value); }
};
String operator+(const String &a, const String &b) {
    String out;
    out.text = a.text + b.text;
    return out;
}

struct DrawCall { std::string text; int x, y; };
struct BoxCall { int style, x, y, w, h; std::string text; };
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
    int field_0x4 = 12, field_0x1c = 16, field_0x4c = 20, field_0x2d8 = 40;
    std::vector<BoxCall> boxes;
    void drawBox(int style, int x, int y, int w, int h, const String &text, int) {
        boxes.push_back({style, x, y, w, h, text.text});
    }
};
struct Status {
    int field_d8 = 33, field_118 = 66, field_a0 = 99, field_a4 = 100;
    int field_d4 = 111;
    int getMissionCount() { return 11; }
    int getKills() { return 22; }
    int getCapturedCrates() { return 44; }
    int getStationsVisited() { return 55; }
    int getJumpgateUsed() { return 77; }
    int getGoodsProduced() { return 88; }
};
struct GameText {
    String text;
    String *getText(int id) {
        text.text = "T" + std::to_string(id);
        return &text;
    }
};
struct Globals {
    static GameText *gameText;
    static Status *status;
};
GameText *Globals::gameText = nullptr;
Status *Globals::status = nullptr;

struct StatusWindow {
    int charImageHeight = 21;
    void drawStatsFixture(PaintCanvas *canvas, Layout *layout, unsigned font,
                          int x0, int boxW, int y, int pad, int lineH,
                          String &lbl, const String &sep, int &tw) {
// PRODUCTION_STATS_HEADER
        int standingRowY = y + layout->field_0x2d8 + layout->field_0x1c + pad * 3;
// PRODUCTION_STATS_ROWS
    }
};

int main() {
    PaintCanvas canvas;
    Layout layout;
    Status status;
    GameText gameText;
    Globals::gameText = &gameText;
    Globals::status = &status;
    StatusWindow window;
    String lbl, sep(":");
    int tw = 0;
    window.drawStatsFixture(&canvas, &layout, 1, 10, 600, 100, 8, 12,
                            lbl, sep, tw);

    assert(layout.boxes.size() == 6);
    const BoxCall boxes[] = {
        {0, 10, 148, 600, 16, "T576"},
        {5, 10, 172, 292, 49, ""}, {5, 318, 172, 292, 49, ""},
        {0, 10, 229, 600, 16, "T577"},
        {5, 10, 253, 292, 80, ""}, {5, 318, 253, 292, 80, ""}
    };
    for (unsigned i = 0; i < 6; ++i) {
        const BoxCall &got = layout.boxes[i];
        const BoxCall &want = boxes[i];
        assert(got.style == want.style && got.x == want.x && got.y == want.y);
        assert(got.w == want.w && got.h == want.h && got.text == want.text);
    }

    assert(canvas.calls.size() == 26);
    const DrawCall labels[] = {
        {"T406", 30, 209}, {"T407", 278, 209},
        {"T408", 338, 209}, {"T409", 586, 209}
    };
    for (unsigned i = 0; i < 4; ++i) {
        const DrawCall &got = canvas.calls[i];
        assert(got.text == labels[i].text && got.x == labels[i].x && got.y == labels[i].y);
    }
    const int ids[] = {568, 176, 552, 559, 557, 3235, 564, 558, 560, 561, 567};
    const int values[] = {11, 22, 33, 44, 55, 66, 77, 88, 99, 100, 111};
    for (unsigned i = 0; i < 11; ++i) {
        const DrawCall &label = canvas.calls[4 + i * 2];
        const DrawCall &value = canvas.calls[5 + i * 2];
        int row = static_cast<int>(i < 6 ? i : i - 6);
        int labelX = i < 6 ? 30 : 338;
        int valueRight = i < 6 ? 282 : 590;
        int rowY = 261 + row * 12;
        std::string expectedLabel = "T" + std::to_string(ids[i]) + ":";
        std::string expectedValue = std::to_string(values[i]);
        assert(label.text == expectedLabel && label.x == labelX && label.y == rowY);
        assert(value.text == expectedValue && value.x == valueRight -
               static_cast<int>(expectedValue.size()) && value.y == rowY);
    }
    std::puts("StatusWindow stats-table host smoke: OK");
}
