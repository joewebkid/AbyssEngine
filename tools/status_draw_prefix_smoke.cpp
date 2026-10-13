#include <cassert>
#include <cstdio>
#include <vector>

struct PaintCanvas {
    std::vector<unsigned> colors;
    int fillW = -1, fillH = -1;
    void SetColor(unsigned color) { colors.push_back(color); }
    void FillRectangle(int x, int y, int w, int h) {
        assert(x == 0 && y == 0);
        fillW = w;
        fillH = h;
    }
};

struct Layout {
    int field_0x48 = 20, buttonInsetX = 10, field_0x20 = 30;
    int field_0xc = 5, field_0x2c = 8;
    int backgroundCalls = 0, scrollbarCalls = 0;
    int barX = -1, barY = -1, barViewport = -1, barStart = -1, barHeight = -1;
    void drawBG() { ++backgroundCalls; }
    void drawScrollBar(int x, int y, int viewport, int start, int height) {
        ++scrollbarCalls;
        barX = x; barY = y; barViewport = viewport;
        barStart = start; barHeight = height;
    }
};

struct Globals {
    static PaintCanvas *Canvas;
    static Layout *layout;
    static unsigned font;
    static unsigned char iPad;
    static int w, h;
};
PaintCanvas *Globals::Canvas = nullptr;
Layout *Globals::layout = nullptr;
unsigned Globals::font = 0;
unsigned char Globals::iPad = 0;
int Globals::w = 0, Globals::h = 0;

struct StatusWindow {
    int viewportHeight = 100, scrollOffset = 9, boxWidth = 0;
    float getRelativeScrollStartPos() { return 0.25f; }
    float getRelativeScrollHeight() { return 0.5f; }
    void draw();
};

// PRODUCTION_DRAW_PREFIX

int main() {
    PaintCanvas canvas;
    Layout layout;
    StatusWindow window;
    Globals::Canvas = &canvas;
    Globals::layout = &layout;
    Globals::w = 800;
    Globals::h = 600;

    window.draw();
    assert((canvas.colors == std::vector<unsigned>{0xffu, 0xffffffffu}));
    assert(canvas.fillW == 800 && canvas.fillH == 600);
    assert(layout.backgroundCalls == 1 && layout.scrollbarCalls == 1);
    assert(layout.barX == 770 && layout.barY == 35);
    assert(layout.barViewport == 100 && layout.barStart == 25 && layout.barHeight == 50);
    assert(window.boxWidth == 752);

    Globals::iPad = 1;
    window.draw();
    assert(window.boxWidth == 352);
    assert(layout.backgroundCalls == 2 && layout.scrollbarCalls == 2);
    std::puts("StatusWindow draw-prefix host smoke: OK");
}
