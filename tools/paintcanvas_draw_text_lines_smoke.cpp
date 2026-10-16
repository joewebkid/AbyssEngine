#include <cassert>

namespace AbyssEngine {
struct String { int id; int width; };
}

template<class T> struct Array {
    unsigned int count;
    T *items;
    unsigned int size() const { return count; }
    T *data() { return items; }
};

struct PaintCanvas {
    void DrawTextLines(unsigned int, Array<AbyssEngine::String *> *, int, int, bool);
    void DrawTextLines(unsigned int, Array<AbyssEngine::String *> *, int, int,
                       unsigned int, bool);
    int GetTextWidth(unsigned int, const AbyssEngine::String &);
    void DrawString(unsigned int, const AbyssEngine::String &, int, int, bool);
    int GetTextHeight(unsigned int);
};

struct Draw { int id, x, y; };
static Draw draws[2];
static int widthCalls, drawCalls, heightCalls;

int PaintCanvas::GetTextWidth(unsigned int font, const AbyssEngine::String &str) {
    assert(font == 7);
    ++widthCalls;
    return str.width;
}
void PaintCanvas::DrawString(unsigned int font, const AbyssEngine::String &str,
                             int x, int y, bool flag) {
    assert(font == 7 && !flag && drawCalls < 2);
    draws[drawCalls++] = {str.id, x, y};
}
int PaintCanvas::GetTextHeight(unsigned int font) {
    assert(font == 7);
    ++heightCalls;
    return 9;
}

// PRODUCTION_METHODS

static void check(int widthCount, int x0, int x1) {
    assert(widthCalls == widthCount && drawCalls == 2 && heightCalls == 2);
    assert(draws[0].id == 1 && draws[0].x == x0 && draws[0].y == 100);
    assert(draws[1].id == 2 && draws[1].x == x1 && draws[1].y == 109);
    widthCalls = drawCalls = heightCalls = 0;
}

int main() {
    AbyssEngine::String first{1, 11}, second{2, 20};
    AbyssEngine::String *items[] = {&first, &second};
    Array<AbyssEngine::String *> lines{2, items};
    PaintCanvas canvas;

    canvas.DrawTextLines(7, &lines, 50, 100, false);
    check(0, 50, 50);
    canvas.DrawTextLines(7, &lines, 50, 100, true);
    check(2, 45, 40);
    canvas.DrawTextLines(7, &lines, 50, 100, 40u, false);
    check(2, 79, 70);
    canvas.DrawTextLines(7, &lines, 50, 100, 40u, true);
    check(0, 50, 50);

    lines.count = 0;
    canvas.DrawTextLines(7, &lines, 50, 100, true);
    canvas.DrawTextLines(7, &lines, 50, 100, 40u, false);
    assert(widthCalls == 0 && drawCalls == 0 && heightCalls == 0);
}
