#include <cassert>
#include <cstdio>
#include <vector>

struct Rect { int x, y, w, h; };
struct ImageCall { unsigned image; int x, y, flags; };
struct PatternCall { unsigned image; int x, y, w, h; };
struct PaintCanvas {
    std::vector<unsigned> colors;
    std::vector<Rect> rects;
    std::vector<ImageCall> images;
    int GetImage2DWidth(unsigned) { return 8; }
    int GetImage2DHeight(unsigned) { return 6; }
    void SetColor(unsigned color) { colors.push_back(color); }
    void DrawRectangle(int x, int y, int w, int h) { rects.push_back({x, y, w, h}); }
    void DrawImage2D(unsigned image, int x, int y) { images.push_back({image, x, y, 0}); }
    void DrawImage2D(unsigned image, int x, int y, unsigned char flags) {
        images.push_back({image, x, y, flags});
    }
};
struct Layout;
struct Globals {
    static PaintCanvas *Canvas;
    static Layout *layout;
};
PaintCanvas *Globals::Canvas = nullptr;
Layout *Globals::layout = nullptr;
struct Layout {
    unsigned scrollBarImage = 100, scrollBarFillImage = 101;
    unsigned drawColor = 0xaabbccdd;
    int field_0x48 = 99, scrollBarInset = 3, scrollBarHandle = 2;
    std::vector<PatternCall> patterns;
    void drawBGPattern(unsigned image, int x, int y, int w, int h) {
        patterns.push_back({image, x, y, w, h});
    }
    void drawScrollBar(int x, int y, int trackH, int pos, int range);
};

// PRODUCTION_SCROLLBAR

int main() {
    PaintCanvas canvas;
    Layout layout, sharedLayout;
    sharedLayout.field_0x48 = 17;
    Globals::Canvas = &canvas;
    Globals::layout = &sharedLayout;

    layout.drawScrollBar(20, 30, 100, 10, 40);
    assert(canvas.colors.size() == 3);
    assert(canvas.colors[0] == 0x111417ffu);
    assert(canvas.colors[1] == 0x193043ffu);
    assert(canvas.colors[2] == layout.drawColor);
    assert(canvas.rects.size() == 1);
    assert(canvas.rects[0].x == 20 && canvas.rects[0].y == 33);
    assert(canvas.rects[0].w == 17 && canvas.rects[0].h == 94);
    assert(layout.patterns.size() == 1);
    assert(layout.patterns[0].image == 101);
    assert(layout.patterns[0].x == 23 && layout.patterns[0].y == 51);
    assert(layout.patterns[0].w == 8 && layout.patterns[0].h == 19);
    assert(canvas.images.size() == 2);
    assert(canvas.images[0].image == 100 && canvas.images[0].x == 23);
    assert(canvas.images[0].y == 45 && canvas.images[0].flags == 0);
    assert(canvas.images[1].image == 100 && canvas.images[1].x == 23);
    assert(canvas.images[1].y == 70 && canvas.images[1].flags == 2);

    canvas.images.clear();
    layout.patterns.clear();
    layout.drawScrollBar(20, 30, 50, 45, 8);
    assert(layout.patterns.empty());
    assert(canvas.images.size() == 2);
    assert(canvas.images[0].x == 23 && canvas.images[0].y == 68);
    assert(canvas.images[1].x == 23 && canvas.images[1].y == 66);
    std::puts("Layout scrollbar host smoke: OK");
}
